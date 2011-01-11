/*
 * includes/cadence/doste/processor.h
 * 
 * The main processing class for the Doste transaction system.
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 21/7/2007
 * Home Page : http://www.cadence.co.uk
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

#ifndef _cadence_DVM_PROCESSOR_H_
#define _cadence_DVM_PROCESSOR_H_

#include <cadence/doste/definition.h>
//#include <cadence/mutex.h>
#include <cadence/dll.h>


namespace cadence {
	namespace doste {
		class Context;
		class Event;

		/**
		* There should be one instance of the processor class for
		* each processor. All event processing is initiated here and
		* it stores all the event queues. If you wish to block until
		* an event has completed then use processRemaining(). Generally
		* there should be little need to use this class directly.
		* @author Nicolas Pope
		* @see Event
		*/
		class DVMIMPORT Processor {
	
			public:
			Processor();
			virtual ~Processor();
	
			/**
			* Causes the run method to return. Do not use directly.
			*/
			void shutdown() { m_running = false; };
	
			//bool isIdle() const { return m_idle; };
			//void notIdle() { m_idle = false; };
			
			/** @return The number of events still in all queues. TODO: UPDATE TO CHECK ALL QUEUES */
			static int queueSize();
	
			/** @return This processors unique ID number. */
			int getID() const { return m_id; };
	
			/**
			 * Process all events in the specified queue. This should not be called
			 * directly because on multi-processor systems this will cause deadlock.<br>
			 * Queue 0 = SET events.<br>
			 * Queue 1 = NOTIFY events.<br>
			 * Queue 2 = ADDDEP events.<br>
			 * @param queue The queue number, 0-2.
			 */
			static void processQueue(int queue);
			/**
			 * Causes all queues to be processed in order and synchronised with other
			 * processors. You may call this but not from inside an event handler. Use
			 * processRemaining() in an event handler.
			 * @return True if there are still more events to be processed.
			 */
			static bool processInstant();
			/**
			 * Finish processing queues. This is a version of processInstant() which works
			 * inside event handlers without causing deadlocks.
			 */
			static void processRemaining();
			/** @return Number of instants since start. */
			static long long getInstant() { return s_instant; };
			/** @return Number of events processed since start. */
			static long long getEventCount() { long long temp = s_evtcount; s_evtcount = 0; return temp; };

			Context *context() const { return m_context; }
			void context(Context *con) { m_context = con; }
	
			/** Simple wrapper that calls processInstant() until no more events exist in the queues. */
			static void processAll() { while (processInstant()); };
			
			virtual void dbInit(const OID &proc) {};
	
			/**
			 * Get a processor from is unique id number.
			 * @param id Processor ID
			 * @return Processor corresponding to ID or NULL.
			 */
			static Processor *getProcessor(int id);
			/**
			 * Get the processor object for the processor which is calling this function.
			 * @return This processors processor object.
			 */
			static Processor *getThisProcessor();
	
			/**
			* @return The number of active processors on this machine
			*/
			static int numProcessors() { return s_nextproc; };
	
			/**
			 * Add an event to the correct queue for processing. This returns immediately.
			 * @param evt A reference to an event.
			 * @param flags The only valid flag is Event::FLAG_FREE which deletes the object when processed.
			 * @return True if the event was sent correctly.
			 */
			static bool send(Event &evt, int flags=0) { return send(&evt,flags); };
			/**
			 * Add an event to the correct queue for processing. This returns immediately.
			 * @param evt A pointer to an event.
			 * @param flags The only valid flag is Event::FLAG_FREE which deletes the object when processed.
			 * @return True if the event was sent correctly.
			 */
			static bool send(Event *evt, int flags=0);
			
			static void init(int n);
			static void final();
			
			static void dump();
			
			static const int FLAGS_BLOCK = 0x01;
			static const int FLAGS_FREE = 0x02;	/**< Delete the event object when it has been processed */
			
			static const unsigned int MAX_PROCESSORS = 8;
			static const unsigned int QUEUE_SIZE = 100000;
			static const unsigned int NUM_QUEUES = 3;
			
			protected:
			int m_id;
			#ifdef LINUX
			//pthread_t m_pthread;
			//static pthread_key_t s_pkey;
			#endif
			bool m_running;
			bool m_idle;
			Context *m_context;
	
			struct EvtQueue {
				Event *evt;
				int flags;
			};
			
			struct Queues {
				unsigned int get;
				unsigned int put;
				EvtQueue q[QUEUE_SIZE];
			};
	
			static Queues s_queue[NUM_QUEUES];
			static long long s_instant;
			static long long s_evtcount;
	
			static Processor *s_processors[MAX_PROCESSORS];
			static int s_nextproc;
		};
	};
};

#endif
