/*
 * src/dvm/handler.cpp
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

#include <cadence/doste/handler.h>
#include <cadence/doste/event.h>
#include <cadence/doste/buffer.h>
#include <cadence/messages.h>
#include <cadence/doste/doste.h>

#include <iostream>

using namespace cadence;
using namespace cadence::doste;

//Agent *Handler::s_agents[AGENT_HASH_SIZE] = {0};
Handler::SHandlerEntry Handler::s_handlers[Handler::MAX_HANDLERS];
Handler *Handler::s_otherwise = 0;
unsigned int Handler::s_numhandlers = 0;

//Mutex handle_lock;

Handler::Handler() {
	if (s_otherwise != 0) {
		Warning(Warning::GLOBAL_HANDLERS, "You are attempting to create multiple global handlers");
	}
	s_otherwise = this;
}

Handler::Handler(const OID &o) {
	if (s_numhandlers >= MAX_HANDLERS) {
		Warning(Warning::HANDLER_EXCEED, "There are too many handlers");
		return;
	}

	//handle_lock.lock();
	s_handlers[s_numhandlers].ha = this;
	s_handlers[s_numhandlers].l = o;
	s_handlers[s_numhandlers].h = o;
	s_numhandlers++;
	//handle_lock.unlock();
}

Handler::Handler(const OID &l, const OID &h) {
	if (s_numhandlers >= MAX_HANDLERS) {
		Warning(Warning::HANDLER_EXCEED, "There are too many handlers");
		return;
	}

	//handle_lock.lock();
	s_handlers[s_numhandlers].ha = this;
	s_handlers[s_numhandlers].l = l;
	s_handlers[s_numhandlers].h = h;
	s_numhandlers++;
	//handle_lock.unlock();
}

Handler::~Handler() {
	//Remove handler, but not possible yet.
}

void Handler::map(const OID &l, const OID &h) {
	if (s_numhandlers >= MAX_HANDLERS) {
		Warning(Warning::HANDLER_EXCEED, "There are too many handlers");
		return;
	}

	//handle_lock.lock();
	s_handlers[s_numhandlers].ha = this;
	s_handlers[s_numhandlers].l = l;
	s_handlers[s_numhandlers].h = h;
	s_numhandlers++;
	//handle_lock.unlock();
}

bool Handler::route(Event &evt) {

	//DEBUG CODE
	/*if (evt.dest() != doste::root) {
		char buf[100];
		evt.dest().toString(buf,100);
		std::cout << "Event " << evt.type() << ": dest=" << buf;
		evt.param<0>().toString(buf,100);
		std::cout << " 0=" << buf;
		evt.param<1>().toString(buf,100);
		std::cout << " 1=" << buf << "\n";
	}*/
	
	//Search for a matching handler.
	Handler *ha=s_otherwise;

	//if (!evt.dest().isReserved()) ha = s_handlers[0].ha;
	//else {
		for (unsigned int i=0; i<s_numhandlers; ++i) {
			//Match found.
			if ((s_handlers[i].l <= evt.dest()) && (s_handlers[i].h >= evt.dest())) {
				ha = s_handlers[i].ha;
				break;
			}
		}
	//}

	if (ha == 0) {
		/* if (evt.dest() != Null) {
			Warning(Warning::INVALID_DESTINATION, dstring("I don't know how to talk to '") + evt.dest() + "'");
		}*/
		evt.result(Null);
		evt.type(0);
		return false;
	}


	if (!ha->handle(evt)) {
		Warning(Warning::UNPROCESSED_EVENT, dstring("Event ") + evt.type() + " for " + evt.dest() + " could not be processed. Parameter 0 is " + evt.param<0>());
		return false;
	}
	evt.type(0);
	return true;
}


