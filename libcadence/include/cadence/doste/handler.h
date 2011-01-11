/*
 * includes/doste/dvm/handler.h
 * 
 * An event router.
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

#ifndef _doste_DVM_HANDLER_H_
#define _doste_DVM_HANDLER_H_

#include <cadence/doste/oid.h>
#include <cadence/dll.h>
#include <cadence/file.h>

namespace cadence {
	namespace doste {

		class Event;
	
		/**
		* Handlers deal with all event processing. There may be several different implementations of a
		* handler depending on where the object is stored. For example: local storage using main memory,
		* disk storage or network storage. A handler registers which OIDs it is reponsible for and all
		* subsequent events sent to those objects go to the corresponding handler.
		* @author Nicolas Pope
		*/
		class DVMIMPORT Handler {
			public:
			/**
			* Use this to mean all events that match no other handler.
			* There can only be one such handler, you will receive warnings if
			* there are multiple global handlers and the most recently created
			* will be used.
			*/
			Handler();
	
			/**
			* Constructor for single object handler. Typically this is used for hardware
			* that only needs to receive the events of one object.
			* @param o OID to receive events for.
			*/
			Handler(const OID &o);
	
			/**
			* Constructor for multi-object handler. Receives events for a range of objects.
			* Used for storage handlers and math handlers.
			* @param l First OID in range, inclusive.
			* @param h Last OID in range, inclusive.
			*/
			Handler(const OID &l, const OID &h);
	
			virtual ~Handler();
	
			/**
			* Implement to handle object events. Note that if you return false for a bulk
			* operation event then that event will be converted into many smaller non-bulk
			* events and handle will be called again.
			* @param evt The event to be handled.
			* @return True if the event was handled, false if it could not be handled.
			*/
			virtual bool handle(Event &evt)=0;
			
			/**
			* Maps an OID range to this handler. Similar to constructor but allows additional
			* ranges to be mapped as well.
			* @param l First OID in range, inclusive
			* @param h Last OID in range, inclusive
			*/
			void map(const OID &l, const OID &h);
	
			/**
			* Will route the event to the correct handler or agent.
			* This will be called by each Processor.
			* @param evt The event to send to correct handler.
			* @return True if a handler was found.
			*/
			static bool route(Event &evt);
			
			//virtual bool save(LocalFile *f)=0;
			//virtual bool load(LocalFile *f)=0;
	
			private:
			
			static const unsigned int MAX_HANDLERS = 20;
	
			//Can be implemented more efficiently. Sort etc...
			struct SHandlerEntry {
				Handler *ha;
				OID l;
				OID h;
			};
			static SHandlerEntry s_handlers[MAX_HANDLERS];
			static Handler *s_otherwise;
			static unsigned int s_numhandlers;
	
			static void convertSetList(cadence::doste::Event &evt, Handler *ha);
			//static void convertGetRange(Event &evt, Handler *ha);
		};
	};
};

#endif
