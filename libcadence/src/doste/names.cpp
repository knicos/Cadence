/*
 * src/dvm/names.cpp
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

#include "names.h"
#include <string.h>
#include <cadence/doste/modifiers.h>
//#include <cadence/mutex.h>
#include <cadence/memory.h>
#include "../softagent.h"

using namespace cadence;
using namespace cadence::doste;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

//Mutex name_lock;

#undef This
Names::Names() {
	//m_hash = NEW NameEntry*[NAME_HASH_SIZE];
	for (unsigned int i=0; i<Names::NAME_HASH_SIZE; i++) {
		m_hash[i] = 0;
	}
	m_lastname = 0;

	add("null", Null);
	add("this", OID(0,0,0,5));
	add("self", Self);
	add("true", True);
	add("false", False);
	add("local", Local);
	add("all", All);
	add("void", Void);
	add("*", Multiply);
	add("size", Size);
	add("$", Key);
	add("type", Type);
	add("..", Parent);
	add(".", This);
	add("+", Add);
	add("-", Subtract);
	add("/", Divide);
	add("<", Less);
	add(">", Greater);
	add("&", And);
	add("|", Or);

	add("add", Add);
	add("sub", Subtract);
	add("div", Divide);
	add("less", Less);
	add("greater", Greater);
	add("mul", Multiply);

	add("_clone", _clone);
	//add("clone", _clone);
	add("_keys", _keys);


	//Also add the modifiers.
	add("SET", modifiers::Set);
	add("DEFINE", modifiers::Define);
	add("FUNC", modifiers::DefineFunc);
	add("CREATE", modifiers::Create);
	add("new", modifiers::Create);
	//add("ADDDEP", Modifiers::AddDep);
	add("SEQ", modifiers::Seq);
	//add("RETURN", Modifiers::Return);
	add("union", modifiers::Union);
	add("GETDEF", modifiers::GetDef);
	add("GETFLAGS", modifiers::GetFlags);
	add("compare", modifiers::Compare);

	//SoftAgent labels
	add("SSET", SoftAgent::SSET);
	add("ASET", SoftAgent::ASET);
	add("SDEF", SoftAgent::SDEF);
	add("ADEF", SoftAgent::ADEF);
	add("SGET", SoftAgent::SGET);
	add("CREA", SoftAgent::CREA);
	add("ISET", SoftAgent::ISET);
	add("SFUN", SoftAgent::SFUN);
	add("AFUN", SoftAgent::AFUN);
	//...
}

Names::~Names() {
	//Delete all name entries.
	for (unsigned int i=0; i<Names::NAME_HASH_SIZE; i++) {
		NameEntry *cur = m_hash[i];
		while(cur) {
			NameEntry *next = cur->next;
			delete cur;
			cur = next;
		}
	}
}

OID Names::lookup(const char *n) {
	int hash = hashString(n);

	//name_lock.lock();
	NameEntry *cur = m_hash[hash];
	while (cur != 0) {
		if (strcmp(cur->name, n) == 0) {
			//name_lock.unlock();
			return cur->id;
		}
		cur = cur->next;
	}

	NameEntry *newname = NEW NameEntry;
	strcpy(newname->name, n);
	newname->next = m_hash[hash];
	newname->id = OID(0,5,0,m_lastname++);
	m_hash[hash] = newname;

	//name_lock.unlock();

	return newname->id;
}

//#include <iostream>

void Names::add(const char *n, const OID &o) {

	int hash = hashString(n);
	NameEntry *newname = NEW NameEntry;
	strcpy(newname->name, n);

	//name_lock.lock();
	newname->next = m_hash[hash];
	newname->id = o;
	m_hash[hash] = newname;
	//name_lock.unlock();
}

const char *Names::lookup(const OID &o) {
	if (o == Null) return "null";

	NameEntry *cur;

	//name_lock.lock();
	for (unsigned int i=0; i<Names::NAME_HASH_SIZE; i++) {
		cur = m_hash[i];
		while (cur != 0) {
			if (cur->id == o) {
				//name_lock.unlock();
				return cur->name;
			}
			cur = cur->next;
		}
	}
	//name_lock.unlock();

	return 0;
}
