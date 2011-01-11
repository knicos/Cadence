/*
 * src/dvm/processor.cpp
 * 
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 22/7/2007
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
#include <cadence/doste/processor.h>
#include <cadence/messages.h>
#include <cadence/doste/doste.h>
#include <cadence/doste/event.h>
#include <cadence/memory.h>
#include <iostream>

using namespace cadence;
using namespace cadence::doste;

Processor *Processor::s_processors[Processor::MAX_PROCESSORS];
int Processor::s_nextproc = 0;
Processor::Queues Processor::s_queue[Processor::NUM_QUEUES];
long long Processor::s_instant = 0;

long long Processor::s_evtcount = 0;

int curq = -1;

//Mutex proc_mutex;

//#ifdef LINUX
//pthread_barrier_t proc_barrier;
//pthread_key_t Processor::s_pkey;
//#endif

void Processor::init( int n) {
	//Arch::Processor::init();

	//Initialise the whole queue to null
	for (unsigned int i=0; i<NUM_QUEUES; i++) {
		s_queue[i].put = 0;
		s_queue[i].get = 0;
		for (unsigned int j=0; j<QUEUE_SIZE; j++) {
			s_queue[i].q[j].evt = 0;
		}
	}

	//#ifdef LINUX
	//pthread_barrier_init(&proc_barrier, 0, n);
	//pthread_key_create(&s_pkey,0);
	//#endif
}

void Processor::final() {
	//Arch::Processor::final();
	//#ifdef LINUX
	//pthread_barrier_destroy(&proc_barrier);
	//#endif
}

void Processor::dump() {
	/*DMsg msg(DMsg::INFO);
	msg << "Begin Queue Dump:\n";
	for (unsigned int i=0; i<NUM_QUEUES; i++) {
		for (unsigned int j=s_queue[i].get; j < s_queue[i].put; j++) {
			msg << "Event: " << s_queue[i].q[j].evt->type() << " " << s_queue[i].q[j].evt->dest() << "\n";
			msg << "       " << s_queue[i].q[j].evt->param<0>() << "," << s_queue[i].q[j].evt->param<1>() << "\n";
		}
	}
	msg << "End Queue Dump\n";*/
}

Processor *Processor::getProcessor(int id) {
	if (id < 0 || id >= s_nextproc) {
		return 0;
	}

	return s_processors[id];
}

Processor *Processor::getThisProcessor() {
	//#ifdef LINUX
	//long long id = (long long)pthread_getspecific(s_pkey);
	//#else
	int id = 0;
	//#endif
	return s_processors[id];
}

Processor::Processor() {
	s_processors[s_nextproc] = this;
	s_nextproc++;
	m_idle = false;
	m_id = s_nextproc-1;
	m_context = 0;
	#ifdef LINUX
	//m_pthread = pthread_self();
	//pthread_setspecific(s_pkey, (void*)m_id);

	//std::cout << "ThreadID: " << m_id << "\n";
	#endif
}

Processor::~Processor() {
}

int Processor::queueSize() {
	return (s_queue[0].put - s_queue[0].get) + (s_queue[1].put - s_queue[1].get) + (s_queue[2].put - s_queue[2].get);
}

bool Processor::send(Event *evt, int flags) {

	bool res;
	//ECallback *cb = evt->callback();

	//This means that it should be processed by this
	//processor now and block until done.
	if ((evt->type() & 0xF0) == 0) {
		//Process Now.
		res = Handler::route(*evt);
		
		//Delete if needed
		if ((flags & FLAGS_FREE) != 0) {
			delete evt;
		}
		
		return res;
	}
	
	//Otherwise select a processor and add to correct queue.
	int queue = ((evt->type() & 0xF0) >> 4) -1;

	//if (queue == curq) Warning(Warning::CURRENT_QUEUE, "You are sending events to the current queue, this is not good.");
	
	//proc_mutex.lock();
	int i = s_queue[queue].put;
	s_queue[queue].put = ((i+1) % QUEUE_SIZE);
	//proc->m_queue[queue].put;
	
	//info(String("Sent ") + String(proc->getID()));

	s_queue[queue].q[i].evt = evt;
	s_queue[queue].q[i].flags = flags;
	//proc_mutex.unlock();
	
	//#ifdef DOSTE
	//Arch::x86::APIC::sendIPI(0x82);
	//#endif

	return true;
}

bool Processor::processInstant() {
	//#ifdef LINUX
	//pthread_barrier_wait(&proc_barrier);
	//#endif
	curq = 0;
	processQueue(0);
	//#ifdef LINUX
	//pthread_barrier_wait(&proc_barrier);
	//#endif
	curq = 1;
	processQueue(1);
	//#ifdef LINUX
	//pthread_barrier_wait(&proc_barrier);
	//#endif
	curq = 2;
	processQueue(2);
	//#ifdef LINUX
	//pthread_barrier_wait(&proc_barrier);
	//#endif
	curq = -1;
	//Add to complete instants.
	/*if ((getThisProcessor()->getID() == 0)) {
		s_instant++;
		if (doste::dvm::debug & doste::dvm::DEBUG_INSTANTS) std::cout << "Instant: " << s_instant << "\n";
	}*/
	return (queueSize() != 0);
}

void Processor::processRemaining() {
	if (curq == 1) {
		processQueue(1);
		//#ifdef LINUX
		//pthread_barrier_wait(&proc_barrier);
		//#endif
		curq = 2;
		processQueue(2);
		curq = -1;

		if (queueSize() != 0) {
			//#ifdef LINUX
			//pthread_barrier_wait(&proc_barrier);
			//#endif
			curq = 0;
			processQueue(0);
			//#ifdef LINUX
			//pthread_barrier_wait(&proc_barrier);
			//#endif
			curq = 1;
		}
	} else {
		processInstant();
	}
}

void Processor::processQueue(int queue) {
	Event *evt;
	int flags;

	while (true) {
		//proc_mutex.lock();
		if (s_queue[queue].get == s_queue[queue].put) {
			//proc_mutex.unlock();
			return;
		}
	
		evt = s_queue[queue].q[s_queue[queue].get].evt;
		//s_queue[s_curq].q[s_queue[s_curq].get].evt = 0;
		flags = s_queue[queue].q[s_queue[queue].get++].flags;
			
		if (s_queue[queue].get == QUEUE_SIZE)
			s_queue[queue].get = 0;
		//}
		
		//Add to the count of processed events.
		//s_evtcount++;
		//proc_mutex.unlock();
		
		Handler::route(*evt);	//Actually process the event.
	
		if (flags & FLAGS_FREE) {
			delete evt;
		}
	}
}
