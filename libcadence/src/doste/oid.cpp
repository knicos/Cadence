/*
 * src/dvm/oid.cpp
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

#include <cadence/doste/event.h>
#include <cadence/doste/buffer.h>
#include <cadence/doste/definition.h>
#include "names.h"
#include <cadence/doste/processor.h>
#include <cadence/memory.h>
#include <string.h>
//#include <doste/dvm/object.h>
//#include <doste/ecounter.h>
#include <cadence/dmsg.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace cadence;
using namespace cadence::doste;

Names *names;

OID cadence::doste::OID::s_local = OID(1,0,0,0);

OID::OID(const char *v) {
	//First check if its a number or explicit OID.
	
	//Explicit OID
	if ((v[0] == '<') || (v[0] == '[')) {
		int i = 1;
		
		//Read Integer
		int a = 0;
		while (v[i] != ':') {
			a *= 10;
			a += v[i] - '0';
			i++;
		}
		//Read :
		i++;
		
		//Read Integer
		int b = 0;
		while (v[i] != ':') {
			b *= 10;
			b += v[i] - '0';
			i++;
		}
		
		//Read :
		i++;
		
		//Read Integer
		int c = 0;
		while (v[i] != ':') {
			c *= 10;
			c += v[i] - '0';
			i++;
		}
		
		//Read :
		i++;
		
		//Read Integer
		int d = 0;
		while (v[i] != '>') {
			d *= 10;
			d += v[i] - '0';
			i++;
		}
		
		//Read >
		
		*this = OID(a,b,c,d);
	} else if ((v[0] == '-') || (v[0] >= '0' && v[0] <= '9')) {
		if (strchr(v, '.')) {
			*this = atof(v);
		} else {
			int i=0;
			int num=0;
			
			bool neg = false;
			if (v[0] == '-') {
				i++;
				neg = true;
			}
			
			while (v[i] != 0) {
				num *= 10;
				num += v[i] - '0';
				i++;
			}
			if (neg) num = -num;
			*this = OID(num);
		}
	} else if (v[0] == '\'') {
		m_a = 0;
		m_b = 3;
		m_c = 0;
		m_d = v[1];
	} else {
		*this = names->lookup(v);
	}
}

OID::OID(const OIDAccessor &a) {
	*this = a.m_obj.get(a.m_key);
}

void mapLabel(const OID &id, const char* name){
	names->add(name, id);
}

bool cadence::doste::operator==(const OIDAccessor &acc, const OID &o) {
	return (OID)acc == o;
}

OID cadence::doste::OID::get(const OID &o) const {
	Event *evt = NEW Event(Event::GET, *this);
	evt->param<0>(o);
	evt->send();
	OID res(evt->result());
	delete evt;
	return res;
}

void OID::set(const OID &key, const OID &value, bool async) const {
	Event *evt = NEW Event(Event::SET, *this);
	evt->param<0>(key);
	evt->param<1>(value);
	evt->send(Event::FLAG_FREE);
	if (!async) { // Processor::processAll();
		//while (evt->type() != 0) {
		//	Processor::processInstant();
		//}
		//delete evt;
		Processor::processRemaining();
	}
}

void OID::definefuture(const OID &key, const OID &def, bool async) const {
	Event *evt = NEW Event(Event::DEFINE, *this);
	evt->param<0>(key);
	evt->param<1>(def);
	evt->param<2>(Null);
	evt->send(Event::FLAG_FREE);
	if (!async) { // Processor::processAll();
		//while (evt->type() != 0) {
			//Processor::processInstant();
		//}
		//delete evt;
		Processor::processRemaining();
	}
}

void OID::definefuture(const OID &key, const OID &def, const OID &init, bool async) const {
	Event *evt = NEW Event(Event::DEFINE, *this);
	evt->param<0>(key);
	evt->param<1>(def);
	evt->param<2>(init);
	evt->send(Event::FLAG_FREE);
	if (!async) { // Processor::processAll();
		//while (evt->type() != 0) {
			//Processor::processInstant();
		//}
		//delete evt;
		Processor::processRemaining();
	}
}

void OID::define(const OID &key, const OID &def, bool async) const {
	Event *evt = NEW Event(Event::DEFINE_FUNC, *this);
	evt->param<0>(key);
	evt->param<1>(def);
	evt->send(Event::FLAG_FREE);
	if (!async) { // Processor::processAll();
		//while (evt->type() != 0) {
		//	Processor::processInstant();
		//}
		//delete evt;
		Processor::processRemaining();
	}
}

OID OID::definition(const OID &key) const {
	Event *evt = NEW Event(Event::GETDEF, *this);
	evt->param<0>(key);
	evt->send();
	OID res = evt->result();
	delete evt;
	return res;
}

int OID::flags(const OID &key) const {
	Event *evt = NEW Event(Event::GETFLAGS, *this);
	evt->param<0>(key);
	evt->send();
	OID res = evt->result();
	delete evt;
	return res;
}

void OID::flags(const OID &key, int f, bool async) const {
	Event *evt = NEW Event(Event::SETFLAGS, *this);
	evt->param<0>(key);
	evt->param<1>(f);
	evt->send(Event::FLAG_FREE);
	if (!async) {
		Processor::processRemaining();
	}
}

void OID::dependency(const OID &dest, const OID &attrib, const OID &key) {
	Event *evt = NEW Event(Event::ADDDEP, dest);
	evt->param<0>(attrib);
	evt->param<1>(*this);
	evt->param<2>(key);
	evt->send(Event::FLAG_FREE);
}

Definition cadence::doste::OID::operator()(const OID &o) const {
	return (Definition()(*this)(o));
}

OID OID::copy(const OID &n) {
	Event evt(Event::COPY, *this);
	evt.param<0>(n);
	evt.send();
	return n;
}

OID OID::create() {
	Event *evt = NEW Event(Event::CREATE, local()+OID(0,0,1,0));
	evt->send();
	OID res(evt->result());
	delete evt;
	return res;
}

OID::iterator OID::begin() const {
	return iterator(*this, 0);
}

OID::iterator OID::end() const {
	return iterator(*this, -1);
};

OIDAccessor &OIDAccessor::operator=(const Definition &d) {
	m_obj.define(m_key, d, false);
	return *this;
}

OID::iterator::iterator(const OID &o, int c)
 : m_object(o), m_cur(c) {
 	if (c >= 0) {
		Event *evt = NEW Event(Event::GET_KEYS, o);
		evt->send();
		m_buf = evt->result();
		delete evt;
		m_buffer = Buffer::lookup(m_buf);
		if (m_buffer == 0) {
			m_cur = -1;
			return;
		}
		
		if (m_buffer->count() == 0) {
			Buffer::free(m_buf);
			m_buffer = 0;
			m_cur = -1;
		}
	}
};

OID::iterator::~iterator() { if (m_buffer != 0) Buffer::free(m_buf); };

OID OID::iterator::operator*() {
	if (m_buffer == 0) return Null;
	return m_buffer->get(m_cur);
};

OID::iterator &OID::iterator::operator++() {
	if (m_buffer == 0) return *this;
	m_cur++;
	if (m_cur >= m_buffer->count()) m_cur = -1;
	if (m_buffer->get(m_cur) == Parent) {
		m_cur++;
		if (m_cur >= m_buffer->count()) m_cur = -1;
	}
	return *this;
};
OID::iterator &OID::iterator::operator++(int) {
	if (m_buffer == 0) return *this;
	m_cur++;
	if (m_cur >= m_buffer->count()) m_cur = -1;
	if (m_buffer->get(m_cur) == Parent) {
		m_cur++;
		if (m_cur >= m_buffer->count()) m_cur = -1;
	}
	return *this;
};

void OID::toString(char *buf, int len) const {

	int i = 0;
	const char *temp;
	

	if (*this == True) {
		strcpy(buf, "true");
		return;
	} else if (*this == False) {
		strcpy(buf, "false");
		return;
	} else if (isLongLong()) {
		#ifdef X86_64
		long long divisor = 1000000000000000000;
		#else
		long long divisor = 1000000000;
		#endif
		long long rem;
		char map[] = {'0','1','2','3','4','5','6','7','8','9', 0, 0, 0};
		long long v = m_ll;

		if (v < 0) {
			buf[i++] = '-';
			v = 0-v;
		}
	
		bool beginout = false;
	
		while (divisor > 1)
		{
			rem = v / divisor;
			v = v % divisor;
			divisor /= 10;
	
			if (rem != 0)
				beginout = true;
			if (beginout) {
				buf[i] = map[rem];
				i++;
			}
		}
		buf[i] = map[v];
		i++;
		buf[i] = 0;
		return;
	} else if (isDouble()) {
		sprintf(buf, "%0.4f", (float)(*this));
		return;
	} else if (isChar()) {
		buf[i++] = '\'';
		buf[i++] = (char)m_d;
		buf[i++] = '\'';
		buf[i] = 0;
		return;
	} else if (isName()) {
		temp = names->lookup(*this);
		if (temp != 0) {
			strcpy(buf, temp);
			return;
		}
	} else if (isSpecial()) {
		temp = names->lookup(*this);
		if (temp != 0) {
			strcpy(buf, temp);
			return;
		}
	} else if (isModifier()) {
		temp = names->lookup(*this);
		if (temp != 0) {
			strcpy(buf, temp);
			return;
		}
	}

	buf[0] = '<';
	//buf[1] = '[';
	
	i+=1;
	int divisor = 1000000000;
	int rem;
	char map[] = {'0','1','2','3','4','5','6','7','8','9'};
	int v = m_a;

	bool beginout = false;

	while (divisor > 1)
	{
		rem = v / divisor;
		v = v % divisor;
		divisor /= 10;

		if (rem != 0)
			beginout = true;
		if (beginout) {
			buf[i] = map[rem];
			i++;
		}
	}
	buf[i] = map[v];
	i++;
	
	buf[i++] = ':';
	v = m_b;
	divisor = 1000000000;

	beginout = false;

	while (divisor > 1)
	{
		rem = v / divisor;
		v = v % divisor;
		divisor /= 10;

		if (rem != 0)
			beginout = true;
		if (beginout) {
			buf[i] = map[rem];
			i++;
		}
	}
	buf[i] = map[v];
	i++;

	buf[i++] = ':';
	v = m_c;
	divisor = 1000000000;

	beginout = false;

	while (divisor > 1)
	{
		rem = v / divisor;
		v = v % divisor;
		divisor /= 10;

		if (rem != 0)
			beginout = true;
		if (beginout) {
			buf[i] = map[rem];
			i++;
		}
	}
	buf[i] = map[v];
	i++;

	buf[i++] = ':';
	v = m_d;
	divisor = 1000000000;

	beginout = false;

	while (divisor > 1)
	{
		rem = v / divisor;
		v = v % divisor;
		divisor /= 10;

		if (rem != 0)
			beginout = true;
		if (beginout) {
			buf[i] = map[rem];
			i++;
		}
	}
	buf[i] = map[v];
	i++;
	//buf[i++] = ']';
	buf[i++] = '>';

	buf[i] = 0;
}

