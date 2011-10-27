/*
 * src/dvm/local.cpp
 * 
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 2008
 * Home Page : http://www.doste.co.uk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "local.h"
#include <cadence/doste/doste.h>
#include <cadence/doste/event.h>
#include <cadence/doste/buffer.h>
//#include <doste/object.h>
#include <cadence/doste/modifiers.h>
#include <cadence/messages.h>
#include <cadence/doste/definition.h>
#include <cadence/memory.h>
#include <cadence/dasm.h>

#include <iostream>
#include <string.h>

#ifdef LINUX
//#include <pthread.h>

//pthread_mutex_t attrib_lock = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t oid_lock = PTHREAD_MUTEX_INITIALIZER;

#endif

using namespace cadence;
using namespace cadence::doste;

VStore::Object *VStore::Object::s_hash[VStore::Object::OBJECT_HASH_SIZE] = {0};
VStore::Attribute *VStore::Attribute::s_hash[VStore::Attribute::ATTRIB_HASH_SIZE] = {0};
OID VStore::s_alloc;

VStore::VStore()
 : Handler(OID::local()+OID(0,0,1,0), OID::local()+OID(0,0,1,0xFFFFFFFF)) {
 
 	s_alloc = OID::local()+OID(0,0,1,0);
	Object::initialise();
	Attribute::initialise();
	
	map(True, False);
}

VStore::~VStore() {
	Attribute::finalise();
	Object::finalise();
}

VStore::Attribute *VStore::findAttribute(const OID &d, const OID &p) {
	Object *obj;
	Attribute *attrib = Attribute::get(d, p);
	if (attrib == 0) {
		//also need to add a key.
		obj = Object::get(d);
		if (obj == 0) obj = NEW Object(d);
		obj->add(p);
		attrib = NEW Attribute(d, p);
	}
	return attrib;
}

bool VStore::handle(Event &evt) {

	Object *obj;
	Attribute *attrib;
	OID temp;
	
	switch(evt.type()) {
	case Event::CREATE:
		temp = s_alloc;
		s_alloc++;
		evt.result(temp);
		break;

	case Event::COPY:
		obj = Object::get(evt.dest());
		if (obj != 0) {
			obj->copy(evt.param<0>());
		}
		break;
		
	case Event::GET:

		if (evt.param<0>().isSpecial() && evt.param<0>().d() >= 100) {
			obj = Object::get(evt.dest());
			if (obj != 0) {
				evt.result(obj->specialGet(evt.param<0>()));
			} else {
				evt.result(Null);
			}
		} else if (evt.param<0>() == Null) {
			evt.result(Null);
		} else {

			attrib = Attribute::get(evt.dest(), evt.param<0>());
			/* The Attribute does not exist so check for the existence of a $ attribute */
			if (attrib == 0) {
				attrib = Attribute::get(evt.dest(), Key);
				/* There is no $ so return Null */
				if (attrib == 0)
					evt.result(Null);
				/* Otherwise there is a $ so proceed... */
				else {
					/* If the $ attribute has a definition then evaluate it and create a NEW attribute with the resulting value and definition. */
					if ((attrib->getDefinition() != Null)) {
						if (attrib->evaluating) {
							//ERROR, cyclic
							Error(Error::CYCLIC, "Cyclic 'is' definition detected", evt.dest(), evt.param<0>());
							evt.result(Null);
							return true;
						}
						attrib->evaluating = true;
						evt.result(Definition(attrib->getDefinition()).evaluate(evt.dest(), evt.param<0>(), (attrib->getFlags() & OID::FLAG_FUNCTION)));
						attrib->evaluating = false;
						//TODO: Prevent evaluation of this the next time?
						if (attrib->getFlags() & OID::FLAG_FUNCTION) {
							evt.dest().define(evt.param<0>(), attrib->getDefinition(), true);
						} else {
							evt.dest().definefuture(evt.param<0>(), attrib->getDefinition(), evt.result(), true);
						}
					/* $ does not have a definition so instead just use it's value */
					} else {
						evt.result(attrib->getValue(evt.param<0>()));
					}
				}
				
			/* The attribute does exist and it is an out-of-date 'is' definition */
			/* THIS HAS SIDE-EFFECTS AND MUST NOT IN A PARALLEL ENVIRONMENT */
			} else if ((attrib->getFlags() & OID::FLAG_FUNCTION) && (attrib->getFlags() & OID::FLAG_OUT_OF_DATE)) {
				/* Give a warning if this particular 'is' definition is currentl being evaluated. Means it is cyclic */
				if (attrib->evaluating) {
					//ERROR, cyclic
					Error(Error::CYCLIC, "Cyclic 'is' definition detected", evt.dest(), evt.param<0>());
					attrib->setValue(Null);
					attrib->setFlags(attrib->getFlags() & (0xFF - OID::FLAG_OUT_OF_DATE));
					return true;
				} else {
					attrib->evaluating = true;
					/* Evaluate the out-of-date 'is' definition now */
					attrib->setValue(Definition(attrib->getDefinition()).evaluate(evt.dest(), evt.param<0>(), true));
					attrib->setFlags(attrib->getFlags() & (0xFF - OID::FLAG_OUT_OF_DATE));
					attrib->evaluating = false;
				}
				evt.result(attrib->getValue());
			/* Attribute exists but is not an out-of-date 'is' definition so simply return the value as cached. */
			} else {
				evt.result(attrib->getValue());
			}
		}

		break;
		
	case Event::GETDEF:
		attrib = Attribute::get(evt.dest(), evt.param<0>());
		if (attrib == 0) {
			evt.result(Null);
		} else
			evt.param<0>(attrib->getFlags());
			evt.result(attrib->getDefinition());
		break;

	case Event::GETFLAGS:
		attrib = Attribute::get(evt.dest(), evt.param<0>());
		if (attrib == 0) {
			evt.result(Null);
		} else
			evt.result(attrib->getFlags());
		break;
	
	case Event::SET:
		attrib = findAttribute(evt.dest(), evt.param<0>());
	
		//attrib->addDependent(evt.dest(), evt.param<0>());
		if (attrib->setValue(evt.param<1>()) && evt.param<0>() != Key) {
			temp = attrib->getValue();
			attrib = Attribute::get(evt.dest(), Key);
			if (attrib != 0) attrib->notifyAll(evt.param<0>(), temp);
		}

		break;

	case Event::SET_BYDEF:
		attrib = findAttribute(evt.dest(), evt.param<0>());
	
		if (attrib->setValue(evt.param<1>()) && evt.param<0>() != Key) {
			//attrib->addDependent(evt.dest(), evt.param<0>());
			temp = attrib->getValue();
			attrib = Attribute::get(evt.dest(), Key);
			if (attrib != 0) attrib->notifyAll(evt.param<0>(), temp);
		}

		break;
	
	case Event::DEFINE:
		attrib = findAttribute(evt.dest(), evt.param<0>());
		attrib->setDefinition(evt.param<1>(), 0, evt.param<2>());
		break;

	case Event::SETFLAGS:
		attrib = findAttribute(evt.dest(), evt.param<0>());
		attrib->setFlags(evt.param<1>());
		break;
	
	case Event::DEFINE_FUNC:
		attrib = findAttribute(evt.dest(), evt.param<0>());
		attrib->setDefinition(evt.param<1>(), OID::FLAG_FUNCTION | OID::FLAG_OUT_OF_DATE, Null);

		break;
	
	case Event::ADDDEP:
		attrib = findAttribute(evt.dest(), evt.param<0>());
		attrib->addDependent(evt.param<1>(), evt.param<2>());
		break;
		
	case Event::ADD_IS_DEP:
		attrib = findAttribute(evt.dest(), evt.param<0>());
		attrib->addDependent(evt.param<1>(), evt.param<2>(), true);
		break;
	
	case Event::NOTIFY_IS:
		attrib = Attribute::get(evt.dest(), evt.param<0>());
		if (attrib != 0 && (attrib->getFlags() & OID::FLAG_FUNCTION)) attrib->notify();
		if (evt.param<0>() != Key) {
			attrib = Attribute::get(evt.dest(), Key);
			if (attrib != 0) attrib->notifyAll(evt.param<0>(), Null);
		}
		break;
		
	case Event::NOTIFY:
		attrib = Attribute::get(evt.dest(), evt.param<0>());
		if (attrib != 0 && ((attrib->getFlags() & OID::FLAG_FUNCTION) == 0)) attrib->notify();
		if (evt.param<0>() != Key) {
			attrib = Attribute::get(evt.dest(), Key);
			if (attrib != 0) attrib->notifyAll(evt.param<0>(), Null);
		}
		break;
	
	case Event::GET_KEYS:
		obj = Object::get(evt.dest());
		if (obj == 0)
			evt.result(Null);
		else
			evt.result(obj->getKeysBuffer());
		break;
	
	case Event::GET_RANGE:
		obj = Object::get(evt.dest());
		if (obj == 0)
			evt.result(Null);
		else
			evt.result(obj->getRangeBuffer(evt.param<0>(), evt.param<1>()));
		break;

	default: return false;
	}

	return true;
}

//-------------------
//Object
//-------------------

void VStore::Object::initialise() {
	
}

void VStore::Object::finalise() {
	//Delete all objects.
}

VStore::Object::Object()
 : next(0), m_refcount(0) {

	m_oid = s_alloc;
	s_alloc++;

	int hash = hashOID(m_oid);
	next = s_hash[hash];
	s_hash[hash] = this;
}

VStore::Object::Object(const OID &o)
 :	m_oid(o), next(0), m_refcount(0) {

	int hash = hashOID(o);

	next = s_hash[hash];
	s_hash[hash] = this;
}
 
VStore::Object::~Object() {

	Attribute *attrib;

	//Delete all attributes
	for (int i=0; i<m_keys.size(); i++) {
		attrib = Attribute::get(m_oid,m_keys[i]);
		if (attrib != 0) delete attrib;
	}

	int hash = hashOID(m_oid);

	Object *cur = s_hash[hash];
	Object *prev = 0;
	while (cur != 0) {
		if (cur == this) {
			if (prev == 0) {
				s_hash[hash] = cur->next;
			} else {
				prev->next = cur->next;
			}
		}
		prev = cur;
		cur = cur->next;
	}
}

OID VStore::Object::specialGet(const OID &k) {
	switch (k.d()) {
	case 100: break;	//delete
	case 101: break;	//size
	case 102: break;	//save
	case 103: break;
	case 104: return keys();
	case 105: break;	//Allkeys
	case 106: break;	//deep
	}

	return Null;
}

void VStore::Object::copy(const OID &nobj) {
	//OID nobj = OID::create();
	OID res, value;
	int flags;
	Attribute *attrib;
	char *dbuf = new char [5000];
	char *dbuf2;
	DASM *dasm = (DASM*)(root.get("notations").get("dasm"));

	for (int i=0; i<m_keys.size(); i++) {
		if (m_keys[i] == This) continue;
		attrib = Attribute::get(m_oid, m_keys[i]);
		if (attrib == 0) {
			res = Null;
			value = Null;
		} else {
			flags = attrib->getFlags();
			res = attrib->getDefinition();
		}

		/*if (flags & OID::FLAG_OUT_OF_DATE) {
			attrib->setFlags(flags & (0xFF - OID::FLAG_OUT_OF_DATE));
			flags = attrib->getFlags();
			attrib->setValue(Definition(res).evaluate(m_oid, m_keys[i], false));
			value = attrib->getValue();
		}*/
		
		if (res != Null) {
			
			//Clone the definition
			//value = res;
			//res = s_alloc;
			//s_alloc++;
			//value.copy(res);
			//std::cout << "CLONING DEF\n";
			
			dbuf2 = dbuf;
			
			Definition d = res;
			nobj.toString(dbuf2, 100);
			dbuf2 += strlen(dbuf2);
			strcpy(dbuf2, " ");
			dbuf2 += strlen(dbuf2);
			m_keys[i].toString(dbuf2,100);
			dbuf2 += strlen(dbuf2);
			
			if (flags && 0x01) {
				//nobj.define(m_keys[i], res, true);
				strcpy(dbuf2, " is { ");
				dbuf2 += strlen(dbuf2);
			} else {
				value = attrib->getValue();
				//nobj.set(m_keys[i], value, true);
				//nobj.definefuture(m_keys[i], res, true);
				strcpy(dbuf2, " = ");
				dbuf2 += strlen(dbuf2);
				value.toString(dbuf2, 100);
				dbuf2 += strlen(dbuf2);
				strcpy(dbuf2, "\n");
				dbuf2 += strlen(dbuf2);
				m_keys[i].toString(dbuf2, 100);
				dbuf2 += strlen(dbuf2);
				strcpy(dbuf2, " := { ");
				dbuf2 += strlen(dbuf2);
			}
			
			d.toString(dbuf2, 4000);
			dbuf2 += strlen(dbuf2);
			strcpy(dbuf2, " }\n");
			
			//std::cout << "DBUF: " << dbuf << "\n";
			
			//Execute
			dasm->execute(dbuf);
		} else {
			if (flags && OID::FLAG_DEEP) {

				//#ifdef LINUX
				//SPINLOCK
				//pthread_mutex_lock(&oid_lock);
				//#endif
				res = s_alloc;
				s_alloc++;
				//#ifdef LINUX
				//SPINLOCK
				//pthread_mutex_unlock(&oid_lock);
				//#endif

				value = attrib->getValue();
				value.copy(res);
				res.set(This, nobj, true);
				nobj.set(m_keys[i], res, true);
			} else {
				value = attrib->getValue();
				if (value != Null) nobj.set(m_keys[i], value, true);
			}
			nobj.flags(m_keys[i],flags, true);
		}

		//delete evt;
	}
	
	delete [] dbuf;
}

OID VStore::Object::keys() {
	OID nobj = OID::create();
	for (int i=0; i<m_keys.size(); i++) {
		nobj.set(i, m_keys[i], true);
	}
	nobj.set(Size, m_keys.size(), true);
	return nobj;
}

VStore::Object *VStore::Object::get(const OID &o) {
	int hash = hashOID(o);

	Object *cur = s_hash[hash];
	while (cur != 0) {
		if (cur->getOID() == o) {
			return cur;
		}
		cur = cur->next;
	}

	return 0;
}

OID VStore::Object::getKeysBuffer() {
	if (m_keys.size() == 0) return Null;
	return Buffer::create(m_keys.data(), m_keys.size());
}

OID VStore::Object::getRangeBuffer(int a, int b) {
	OID boid = Buffer::create(B_OID, b-a);
	Buffer *buf = Buffer::lookup(boid);
	Attribute *attrib;

	for (int i=a; i<b; i++) {
		attrib = Attribute::get(m_oid, i);
		if (attrib != 0) {
			if (attrib->getFlags() & OID::FLAG_OUT_OF_DATE) {
				attrib->setFlags(attrib->getFlags() & (0xFF - OID::FLAG_OUT_OF_DATE));
				attrib->setValue(Definition(attrib->getDefinition()).evaluate(m_oid, i, true));
			}
			buf->seti(i-a, attrib->getValue());
		} else
			buf->seti(i-a, Null);
	}
	return boid;
}

//---------------------
//Attribute
//---------------------

void VStore::Attribute::initialise() {

}

void VStore::Attribute::finalise() {
	//Delete all attributes.
}

VStore::Attribute::Attribute(const OID &obj, const OID &key)
 :	m_obj(obj), m_key(key), m_value(Null), m_instant(0xFF), m_def(Null), m_flags(0), next(0) {
 	int hash = hashOIDS(obj,key);

	next = s_hash[hash];
	s_hash[hash] = this;
	evaluating = false;
}
 
VStore::Attribute::~Attribute() {
	int hash = hashOIDS(m_obj,m_key);

	Attribute *cur = s_hash[hash];
	Attribute *prev = 0;
	while (cur != 0) {
		if (cur == this) {
			if (prev == 0) {
				s_hash[hash] = cur->next;
			} else {
				prev->next = cur->next;
			}
		}
		prev = cur;
		cur = cur->next;
	}
}

VStore::Attribute *VStore::Attribute::get(const OID &obj, const OID &key) {
	int hash = hashOIDS(obj,key);

	Attribute *cur = s_hash[hash];
	while (cur != 0) {
		if (cur->getObject() == obj && cur->getKey() == key) {
			return cur;
		}
		cur = cur->next;
	}
	return 0;
}

bool VStore::Attribute::setValue(const OID &value) {
	if ((m_value != value || value == Void)) {
		m_value = value;
		notifyAll();
		return true;
	}
	return false;
}

void VStore::Attribute::setFlags(int flag) {
	m_flags = (flag & 0xFFFFFFF8) | (m_flags & 0x7);
}

void VStore::Attribute::addDependent(const OID &obj, const OID &key, bool is) {
	Dependency pair;
	pair.key = key;
	pair.obj = obj;
	pair.isdep = is;

	m_deps.addFront(pair);
	m_deps.unique(pair);
}

void VStore::Attribute::notify() {

	//If there is a definition then evaluate it now
	if (m_def != Null && (m_flags & OID::FLAG_FUNCTION) == 0) {
		//Generate a SET event with the definitions result.

		Event *evt = NEW Event(Event::SET_BYDEF, m_obj);
		evt->param<0>(m_key);
		if (evaluating) {
			//ERROR, cyclic
			Error(Error::CYCLIC, "Cyclic 'is' definition detected", m_obj, m_key);
			return;
		}
		evaluating = true;
		evt->param<1>(Definition(m_def).evaluate(m_obj, m_key, false));
		evaluating = false;
		evt->send(Event::FLAG_FREE);
	} else if (m_def != Null) {
		/* THIS HAS SIDE-EFFECTS AND SHOULD NOT IN A PARALLEL SYSTEM */
		m_flags |= OID::FLAG_OUT_OF_DATE;
		notifyAll();
	}
};

void VStore::Attribute::setDefinition(const OID &def, int flags, const OID &init) {


	m_def = def;
	//Maybe compile definition here.
	
	m_flags = flags;
	
	
	if (init != Null) setValue(init);

	if (m_def != Null && ((m_flags & OID::FLAG_FUNCTION) == 0) && m_key != Key && init == Null) {
		Event *evt = NEW Event(Event::NOTIFY, m_obj);
		evt->param<0>(m_key);
		evt->param<1>(m_obj);
		evt->param<2>(m_key);
		evt->param<3>(m_value); //NOTE: UNRELIABLE, DO NOT USE
		evt->send(Event::FLAG_FREE);
	} else if (m_def != Null && ((m_flags & OID::FLAG_FUNCTION) != 0) && m_key != Key && init == Null) {
		Event *evt = NEW Event(Event::NOTIFY_IS, m_obj);
		evt->param<0>(m_key);
		evt->param<1>(m_obj);
		evt->param<2>(m_key);
		evt->param<3>(m_value); //NOTE: UNRELIABLE, DO NOT USE
		evt->send(Event::FLAG_FREE);
	}
}

void VStore::Attribute::notifyAll() {
	Event *evt;
	
	if (evaluating) return;
	evaluating = true;

		
	//For all attributes dependent upon this one...
	for (List<Dependency>::iterator i=m_deps.begin(); i!=m_deps.end(); i++) {
		//Generate the notify event for this object.
		if ((*i).isdep)
			evt = NEW Event(Event::NOTIFY_IS, (*i).obj);
		else
			evt = NEW Event(Event::NOTIFY, (*i).obj);
		evt->param<0>((*i).key);
		
		//Agents rely on the following but they really should not.
		evt->param<1>(m_obj);
		evt->param<2>(m_key);
		evt->param<3>(m_value); //NOTE: TO BE REMOVED

		evt->send(Event::FLAG_FREE);
	}
	
	evaluating = false;

	//Remove all notifications
	m_deps.clear();
}

void VStore::Attribute::notifyAll(const OID &key, const OID &value) {
	Event *evt;
	//Notify list must be unique.

	for (List<Dependency>::iterator i=m_deps.begin(); i!=m_deps.end(); i++) {

		if ((*i).isdep)
			evt = NEW Event(Event::NOTIFY_IS, (*i).obj);
		else
			evt = NEW Event(Event::NOTIFY, (*i).obj);
		evt->param<0>((*i).key);

		//Only needed by agents, should be removed.
		evt->param<1>(m_obj);
		evt->param<2>(key);
		evt->param<3>(value);	//NOTE: TO BE REMOVED

		evt->send(Event::FLAG_FREE);
	}

	m_deps.clear();
}
