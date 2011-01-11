/*
 * src/dvm/event.cpp
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
#include <cadence/doste/processor.h>
#include <cadence/messages.h>
#include <cadence/memory.h>

using namespace cadence;
using namespace cadence::doste;

Event *Event::s_pool[EVENT_POOL_SIZE] = {0};
unsigned int Event::s_ppos = 0;
Event *Event::s_block;
//SpinLock Event::event_lock;

void *cadence::doste::Event::operator new (size_t s) {
	//event_lock.wait();
	/*if (s_ppos >= EVENT_POOL_SIZE-1) {
		std::cerr << "================================================================================\n" <<
			"FATAL ERROR: Event pool underflow. Pool size is: " << EVENT_POOL_SIZE << ". This generally means either you have a single very massive definition (a massive script?) or an infinite loop has occured. Possible also that you have more than " << EVENT_POOL_SIZE << " definitions evaluating in parallel.\n" <<
					"================================================================================\n";
		exit(-1);
	}*/
	void *v = s_pool[s_ppos++];
	//event_lock.free();
	return v;
}

void *cadence::doste::Event::operator new (size_t size, const char *fname, int line) {
	void *v = s_pool[s_ppos++];
	return v;
}

void cadence::doste::Event::operator delete(void *p) {
	//event_lock.wait();
	/*if (s_ppos == 0) {
		//event_lock.free();
		Warning(Warning::EVENT_OVERFLOW, "Too many events are being deleted!");
		return;
	}*/
	s_pool[--s_ppos] = (Event*)p;
	//event_lock.free();
}

void Event::initialise() {
	s_block = NEW Event[EVENT_POOL_SIZE];
	for (unsigned int i=0; i<EVENT_POOL_SIZE; i++) {
		s_pool[i] = &s_block[i];
	}
}

void Event::finalise() {
	delete [] s_block;
}
