#include "virtualobject.h"
#include <cadence/doste/event.h>
#include <cadence/dmsg.h>

using namespace cadence;
using namespace cadence::doste;

VirtualObject::VirtualObject() : m_convert(false) {
	for (unsigned int i=0; i<VO_HASHSIZE; i++) {
		m_hash[i] = 0;
	}
}

VirtualObject::~VirtualObject() {
	//Delete everything
	Attribute *cur, *temp;
	for (unsigned int i=0; i<VO_HASHSIZE; i++) {
		cur = m_hash[i];
		while (cur != 0) {
			temp = cur->next;
			delete cur;
			cur = temp;
		}
	}
}

void VirtualObject::set(const OID &key, const OID &value) {
	int hash = key.d() % VO_HASHSIZE;
	Attribute *cur = m_hash[hash];
	while (cur != 0) {
		if (cur->key == key) {
			cur->value = value;
			return;
		}
		cur = cur->next;
	}

	cur = new Attribute;
	cur->key = key;
	cur->def = Null;
	cur->flags = 0;
	cur->value = value;
	cur->next = m_hash[hash];
	m_hash[hash] = cur;
}

void VirtualObject::define(const OID &key, const OID &def) {
	int hash = key.d() % VO_HASHSIZE;
	Attribute *cur = m_hash[hash];
	while (cur != 0) {
		if (cur->key == key) {
			cur->def = def;
			cur->flags = 0;
			return;
		}
		cur = cur->next;
	}

	cur = new Attribute;
	cur->key = key;
	cur->def = def;
	cur->flags = 0;
	cur->value = Null;
	cur->next = m_hash[hash];
	m_hash[hash] = cur;
}

void VirtualObject::function(const OID &key, const OID &def) {
	int hash = key.d() % VO_HASHSIZE;
	Attribute *cur = m_hash[hash];
	while (cur != 0) {
		if (cur->key == key) {
			cur->def = def;
			cur->flags = 1;
			return;
		}
		cur = cur->next;
	}

	cur = new Attribute;
	cur->key = key;
	cur->def = def;
	cur->flags = 1;
	cur->value = Null;
	cur->next = m_hash[hash];
	m_hash[hash] = cur;
}

const OID &VirtualObject::get(const OID &key) {
	int hash = key.d() % VO_HASHSIZE;
	Attribute *cur = m_hash[hash];
	while (cur != 0) {
		if (cur->key == key) {
			return cur->value;
		}
		cur = cur->next;
	}
	return Null;
}

OID VirtualObject::convert() {
	Attribute *cur;
	OID real = getRealOID();
	Event *evt;

	for (unsigned int i=0; i<VO_HASHSIZE; i++) {
		cur = m_hash[i];
		while (cur != 0) {
			evt = new Event(Event::SET, real);
			evt->param<0>(cur->key);
			evt->param<1>(cur->value);
			evt->send(Event::FLAG_FREE);

			if (cur->def != Null) {
				if (cur->flags == 0)
					evt = new Event(Event::DEFINE, real);
				else
					evt = new Event(Event::DEFINE_FUNC, real);
				evt->param<0>(cur->key);
				evt->param<1>(cur->def);
				evt->send(Event::FLAG_FREE);
			}

			cur = cur->next;
		}
	}

	return real;
}

OID VirtualObject::getRealOID() {
	if (m_real == Null) {
		m_real = OID::create();
	}
	return m_real;
}
