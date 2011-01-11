/*
 * includes/doste/dvm/event.h
 * 
 * Event class for object communication.
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 9/11/2007
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
 
#ifndef _doste_DVM_EVENT_H_
#define _doste_DVM_EVENT_H_

//#include <cadence/spinlock.h>
#include <cadence/doste/oid.h>
#include <iostream>
#include <cadence/dll.h>
#include <cadence/doste/processor.h>

#ifdef X86_64
typedef long unsigned int size_t;
#else
//typedef unsigned long size_t;
#endif
 
namespace cadence {
	namespace doste {

		typedef int EventType;
	
		/**
		* Represents an object event. All communication between objects is done via these events.
		* Each event is given a destination, type and an optional number of parameters and is then
		* sent to be routed and processed. Event sends may be synchronous or asynchronous.
		* @author Nicolas Pope
		*/
		class DVMIMPORT Event {
			public:
	
			Event() {};
		
			/**
			* Constructor. You can specify event details here but they can be overridden later.
			* @param type Event type, e.g. SET
			* @param dest The destination object.
			*/
			Event(EventType type, const OID &dest) : m_type(type), m_dest(dest) {}
	
			/** Destructor */
			~Event() {};
			
			/**
			* Send this event for processing. This is non-blocking for all events
			* except for GET events (GET, GET_KEYS,GETDEF,CREATE and GETFLAGS).
			* All events are guaranteed to be processed after one instant.
			* @param flags Flags to control how it is processed. See Event:FLAG_.
			* @return True if it was sent correctly.
			*/
			bool send(int flags=0) { return Processor::send(this,flags); };
			
			//void callback(ECallback *cb) { m_cb = cb; };
			//ECallback *callback() const { return m_cb; };
			
			/** Get a parameter. The template argument is the parameter number. */
			template <unsigned int I>
			inline const OID &param();
			
			/** Set a parameter. The template argument is the parameter number */
			template <unsigned int I>
			inline void param(const OID &p);
			
			/** @return The type of event. Type becomes 0 once the event has been processed. */
			EventType type() const { return m_type; };
			/**
			 * Change the event type.
			 * @param t New event type.
			 */
			void type(EventType t) { m_type = t; };
			
			/** @return The result returned by this event if applicable. */
			const OID &result() const { return m_res; };
			/** Change the result value */
			void result(const OID &res) { m_res = res; };
			
			/** @return The destination object OID */
			const OID &dest() const { return m_dest; };
			/**
			 * Change the destination object.
			 * @param d The new destination.
			 */
			void dest(const OID &d) { m_dest = d; };
	
			void* operator new(size_t s);
			void *operator new (size_t size, const char *fname, int line);
	
			void operator delete(void *p);
		
			//The GET operations are evaluated immediately
			static const EventType GET = 		0x01;	/**< Get an object attribute value */
			static const EventType GET_KEYS =	0x02;	/**< Get all object keys */
			static const EventType GETDEF =		0x03;	/**< Get the definition of an attribute */
			static const EventType CREATE =		0x04;	/**< Create a new object based on another objects OID */
			static const EventType GETFLAGS =	0x05;
			static const EventType GET_RANGE =	0x06;
			static const EventType COPY =		0x07;
			static const EventType NOTIFY_IS =	0x08;	/**< Notify an 'is' definition immediately */
			//GET_FLAGS
			//GET_DEPENDENTS
			//Various custom GETs for statistics, security etc...
			
			//The SET operations always go into Q1
			static const EventType SET = 		0x10;	/**< Set an object attribute */
			static const EventType DEFINE =		0x11;	/**< Define a single object attribute */
			static const EventType DEFINE_FUNC =	0x12;	/**< Define an attribute as a function */
			//static const EventType DEFINE_AGENT =	0x13;	/**< Define an attribute as an agent */
			//Maybe some custom SETs for security...
			static const EventType SETFLAGS =	0x14;
			static const EventType SET_BYDEF =	0x15;
			
			//The NOTIFY operations always go into Q2
			static const EventType NOTIFY =		0x20;	/**< Notify that a definition is out-of-date */

			static const EventType ADDDEP =		0x30;	/**< Add a single dependency */
			static const EventType ADD_REF =	0x31;	/**< Add a reference to prevent garbage collection */
			static const EventType REMOVE_REF =	0x32;	/**< Remove a reference and garbage collect if needed. */
			static const EventType ADD_IS_DEP =	0x33;	/**< Add a dependency from an 'is' definition */
			
			//See also the Processor flags.
			static const int FLAG_BLOCK = 0x01;	
			static const int FLAG_FREE = 0x02;	/**< Automatically delete the event when processed. */
			static const int FLAG_AGENT = 0x04;	/**< Deprecated. */
	
			static void initialise();
			static void finalise();
	
			private:
			static const unsigned int MAX_PARAMETERS = 4;
			static const unsigned int EVENT_POOL_SIZE = 100000;
			//static cadence::SpinLock event_lock;
			
			EventType m_type;
			//ECallback *m_cb;
			OID m_dest;
			//union {
			OID m_p[MAX_PARAMETERS];
			OID m_res;
			//};
	
			static unsigned int s_ppos;
			static Event *s_pool[EVENT_POOL_SIZE];
			static Event *s_block;	//Block of preallocated events
		};
	};
};

template <unsigned int I>
inline const cadence::doste::OID &cadence::doste::Event::param() {
	if (I > MAX_PARAMETERS)	return cadence::doste::Null;
	return m_p[I];
}
	
template <unsigned int I>
inline void cadence::doste::Event::param(const cadence::doste::OID &p) {
	if (I > MAX_PARAMETERS) return;
	m_p[I] = p;
}
#endif
