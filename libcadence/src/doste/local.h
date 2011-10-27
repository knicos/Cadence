/*
 * src/dvm/local.h
 * 
 * Internal RAM storage for DOSTE database objects.
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

#ifndef _doste_DVM_VSTORE_H_
#define _doste_DVM_VSTORE_H_

#include <cadence/doste/handler.h>
#include <cadence/doste/oid.h>
#include <cadence/list.h>
#include <cadence/vector.h>
#include <cadence/doste/definition.h>
#include <cadence/doste/processor.h>
//#include <cadence/spinlock.h>

class OIDPair {
	public:
	friend inline bool operator==(const OIDPair &p1, const OIDPair &p2);

	cadence::doste::OID obj;
	cadence::doste::OID key;
};

class Dependency : public OIDPair {
	public:
	bool isdep;
};

namespace cadence {
	namespace doste {
		class VStore : public Handler {
			public:
			VStore();
			~VStore();
			
			bool handle(Event &evt);
			
			private:
			
			static OID s_alloc;
			
			class Attribute;
			
			Attribute *findAttribute(const cadence::doste::OID &d, const cadence::doste::OID &p);

			class Object {
				public:
				Object();
				Object(const OID &o);
				~Object();
				
				const OID &getOID() const { return m_oid; };
				OID getKeysBuffer();
				OID getRangeBuffer(int a, int b);
				void add(const OID &key) { m_keys.add(key); };
				bool contains(const OID &key) const { return m_keys.contains(key); };
				void all(Attribute *a) { m_all = a; };
				Attribute *all() const { return m_all; };	

				OID specialGet(const OID &k);

				void copy(const OID &nobj);
				OID keys();
				OID size();

				void addReference() { m_refcount++; };
				void removeReference() {
					m_refcount--;
					if (m_refcount <= 0) {
						//std::cout << "Deleting Object: <" << m_oid.a() << ":" << m_oid.b() << ":" << m_oid.c() << ":" << m_oid.d() << ">\n";
						//delete this;
					}
				}

				static Object *get(const OID &o);

				static void initialise();
				static void finalise();
				
				private:
				//doste::ReadWriteLock m_rwlock;	//Lock for key modifications.
				OID m_oid;
				Object *next;
				Attribute *m_all;
				int m_refcount;

				static const unsigned int OBJECT_HASH_SIZE = 10000;
				
				Vector<OID> m_keys;
				static Object *s_hash[OBJECT_HASH_SIZE];
				//static doste::ReadWriteLock s_hlock;
				
				static int hashOID(const OID &o) { return o.d() % OBJECT_HASH_SIZE; };
			};
			
			class Attribute {
				public:
				Attribute(const OID &obj, const OID &key);
				~Attribute();
				
				const OID &getKey() const { return m_key; };
				const OID &getValue() {
					//if ((m_flags & OID::FLAG_FUNCTION) != 0) {
					//	m_value = Definition(m_def).evaluate(m_obj, m_key, true);
					//}
					return m_value;
				}

				const OID &getValue(const OID &key) {
					//if ((m_flags & OID::FLAG_FUNCTION) != 0) {
					//	if (evaluating) {
							//ERROR, cyclic
					//		Error(Error::CYCLIC, "Cyclic 'is' definition detected");
					//		m_value = Null;
					//	} else {
					//		evaluating = true;
					//		m_value = Definition(m_def).evaluate(m_obj, key, true);
					//		evaluating = false;
					//	}
					//}
					return m_value;
				}

				const OID &getObject() const { return m_obj; };

				bool setValue(const OID &value);

				void setDefinition(const OID &def, int flags, const OID &init);
				const OID &getDefinition() const { return m_def; };
				int getFlags() const { return m_flags; };
				void setFlags(int flag);
				//bool isAgent() const { return (m_flags & OID::FLAG_AGENT) != 0; };
				bool isFunction() const { return (m_flags & OID::FLAG_FUNCTION) != 0; };
				bool isDefinition() const { return m_def != Null; };
				void notify();
				void addDependent(const OID &obj, const OID &key, bool is=false);

				void notifyAll();
				void notifyAll(const OID &key, const OID &value);
			
				static Attribute *get(const OID &obj, const OID &key);
					
				static void initialise();
				static void finalise();

				bool evaluating;

				private:
				OID m_obj;
				OID m_key;
				OID m_value;
				unsigned char m_instant;

				List<Dependency> m_deps;
				OID m_def;
				void *m_compdef;
				unsigned char m_flags;
				Attribute *next;
				//SpinLock m_lock;

				static const unsigned int ATTRIB_HASH_SIZE = 200000;

				void evaluate(const OID &me);
				
				static Attribute *s_hash[ATTRIB_HASH_SIZE];
				static int hashOIDS(const OID &o, const OID &k) {
					return ((k.d() << 16)+o.d()) % ATTRIB_HASH_SIZE;
				};
			};
		};
	};
};

bool operator==(const OIDPair &p1, const OIDPair &p2) {
	return (p1.obj == p2.obj && p1.key == p2.key);
}

#endif 
