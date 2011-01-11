/*
 * src/library/dstring.cpp
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

#include <cadence/dstring.h>
#include <cadence/doste/processor.h>
#include <cadence/doste/event.h>
#include <string.h>
#include <cadence/doste/buffer.h>

using namespace cadence;
using namespace cadence::doste;

DString::DString() : m_buffer(0) {
	m_obj = OID::create();
}

DString::DString(const char *str, bool async) : m_buffer(0) {
	m_obj = OID::create();
	int size = strlen(str);
	
	for (int i=0; i<size; i++) {
		m_obj[i].set(str[i], true);
	}
	
	m_obj[Size].set(size, async);
	//Processor::processAll();
}

DString::DString(OID o) : m_buffer(0) {
	m_obj = o;
}

DString::~DString() {
	if (m_buffer != 0) delete [] m_buffer;
}

void DString::toString(char *str, int max) {

	//The whole object may be an integer etc...
	if (m_obj[Size] == Null || !((OID)m_obj[0]).isChar()) {
		m_obj.toString(str,max);
		return;
	}

	int size = (OID)m_obj[Size];

	if (size >= max) size = max-1;
	int j = 0;
	OID v;

	Event *evt = new Event(Event::GET_RANGE, m_obj);
	evt->param<0>(0);
	evt->param<1>(size);
	evt->send();
	OID boid = evt->result();
	Buffer *str2 = Buffer::lookup(boid);
	delete evt;
	if (str2 == 0) {
		str[0] = 0;
		return;
	}

	for (int i=0; i<size; i++) {
		v = str2->get(i);
		if (v.isChar())
			str[j++] = str2->get(i);
		else {
			int k = 0;
			char buf[500];
			v.toString(buf,500);
			while (buf[k] != 0) {
				str[j++] = buf[k++];
			}
		}
	}

	Buffer::free(boid);

	str[j] = 0;
};

DString::operator const char*() {
	if (m_buffer == 0) m_buffer = new char[size()+100];
	toString(m_buffer,size()+1);
	return m_buffer;
}

DString cadence::operator+(DString m, const char *str) {
	int s = m.size();
	int s2 = strlen(str);

	for (int i=0; i<s2; i++) {
		m.m_obj[s+i].set(str[i], true);
	}
	
	m.m_obj[Size].set(s+s2);

	return m;
}

DString cadence::operator+(DString m, const DString &str) {
	int s = m.size();
	int s2 = str.size();

	for (int i=0; i<s2; i++) {
		m.m_obj[s+i].set(str.m_obj.get(i), true);
	}

	m.m_obj[Size].set(s+s2);

	return m;
}

DString cadence::operator+(DString m, const OID &o) {
	char buf[200];
	o.toString(buf,200);

	int s = m.size();
	int s2 = strlen(buf);

	for (int i=0; i<s2; i++) {
		m.m_obj[s+i].set(buf[i], true);
	}
	
	m.m_obj[Size].set(s+s2);

	return m;
}
