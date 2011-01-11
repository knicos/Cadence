/*
 * includes/doste/agent.h
 * 
 * Provides a base class for custom event handling
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 23/11/2007
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
 
#ifndef _doste_AGENT_H_
#define _doste_AGENT_H_

#include <cadence/doste/oid.h>
#include <cadence/vector.h>
#include <cadence/doste/event.h>
#include <cadence/object.h>
#include <cadence/agenthandler.h>

#define IMPLEMENT_EVENTS(A,B)	template <int I> inline void meta_reg_##A(A *o) { \
					o->regEvt<I>(); \
					meta_reg_##A<I+1>(o); \
				} \
				template <> inline void meta_reg_##A<A::num_evt_last-1>(A *o) { \
					o->regEvt<A::num_evt_last-1>(); \
				} \
				template <int I> inline void meta_eval_##A(A *o, int aid, int id) { \
					if (id == I) { \
						o->evalEvt<I>(aid, id); return; \
					} \
					meta_eval_##A<I+1>(o,aid, id); \
				} \
				template <> inline void meta_eval_##A<A::num_evt_last-1>(A *o, int aid, int id) { \
					if (id == A::num_evt_last-1) { \
						o->evalEvt<A::num_evt_last-1>(aid, id); return; \
					} \
				} \
				bool A::notify(int aid, int id) { \
					B::notify(aid, id); \
					if (id == -1) meta_reg_##A<num_evt_parent+1>(this); \
					else if (id >= 10000) return A::handler(id-10000); \
					else meta_eval_##A<num_evt_parent+1>(this, aid, id); \
					return true; \
				}

#define BEGIN_EVENTS(A)	static const int num_evt_parent = A::num_evt_last; \
			template <int I> void regEvt() {}; \
			template <int I> void evalEvt(int aid, int eid) {}; \
			virtual bool notify(int aid, int id); \
			static const int num_evt_first = __LINE__;

#define END_EVENTS static const int num_evt_last = __LINE__ - num_evt_first + num_evt_parent;

//evaluate(num_##A);
#define EVENT(A,K) static const int num_##A = __LINE__ - num_evt_first + num_evt_parent; void reg_##A() { cadence::AgentHandler::add(this, num_##A, K, 0); }

#define EVT(A) static const int num_##A = __LINE__ - num_evt_first + num_evt_parent; void reg_##A() { cadence::AgentHandler::add(this, num_##A, (*this)(#A), 0); }

//#define SHARE_EVENT(j,k)	{ j::evalEvt<j::num_##k>(obj,key,value); }

#define OnEvent(j,i) 	template <> void j::regEvt<j::num_##i>() { reg_##i(); } \
			template <> void j::evalEvt<j::num_##i>(int aid, int eid)

#define E(A) (A)(cadence::doste::modifiers::Seq)
#define DEPENDENCY(A,B) (OID::local() + OID(0,0,15,0)).dependency(A,B,aid);

namespace cadence {

	class XARAIMPORT BaseAgent {
		public:
		
		/**
		 * Constructor. You should call registerEvents() in your constructor
		 * when inheriting an Agent.
		 */
		BaseAgent() {};
		virtual ~BaseAgent() { AgentHandler::remove(this); }

		/**
		 * Implement to be notified of events. You can request to be notified
		 * whenever an attribute within an object is changed.
		 * @param id A number associated with the listen event.
		 * @return Was this event handled by this agent.
		 */
		virtual bool notify(int aid, int id) { return false; }

		virtual bool handler(int id) { return false; }
		void addEvent(int id, doste::Definition def) { AgentHandler::add(this, id+10000, def, 0); }

		/**
		 * Yield your agent thread. This can only be called from within an event.
		 */
		static void yield();

		//protected:

		/**
		 * Register all events with database. This must be called to inform the database
		 * that this agent wants to be triggered on certain events. Usually it would be
		 * called in the constructor of your sub class.
		 */
		void registerEvents() { notify(0, -1); };

		static const int num_evt_last = 0;

	};
	
	/**
	 * The interface for an agent. An agent is something which observes attributes
	 * in the database and performs various tasks when certain observables change.
	 * You can implement this class to be notifed when particular object attributes
	 * are changed. Also implement it if you wish to simply make changes to the
	 * database as an external source, such as an interrupt handler.<br><br>
	 * 
	 * To help with the trigger mechanism several macros have been provided. These
	 * allow you to register and respond to events. Here is an example which watches
	 * the <code>cout</code> attribute of the root object:<br><br>
	 *
	 * <code>
	 * class ConsoleAgent : public doste::Agent {
	 *     public:
	 *     ConsoleAgent() { registerEvents(); }
	 *
	 *     BEGIN_EVENTS(Agent);
	 *     EVENT(evt_cout, doste::root("cout"));
	 *     END_EVENTS;
	 * };
	 *
	 * OnEvent(ConsoleAgent, evt_cout) {
	 *     std::cout << (const char*)doste::dstring(value) << "\n";
	 * }
	 *
	 * IMPLEMENT_EVENTS(ConsoleAgent,Agent);
	 * </code>
	 *
	 * The OnEvent macro must be placed inside the same namespace that the ConsoleAgent
	 * class was declared in. It expands to a function which has three parameters: obj,
	 * key and value, all of which are OIDs. In the above example obj is doste::root, key
	 * is cout and value is whatever was assigned to cout.
	 * @author Nicolas Pope
	 * @see doste::Object
	 * @see doste::OID
	 * @see doste::Definition
	 */
	class XARAIMPORT Agent : public Object, public BaseAgent {
		public:
		
		OBJECT(Object, Agent);
		
		/**
		 * Constructor. You should call registerEvents() in your constructor
		 * when inheriting an Agent.
		 */
		Agent() {};
		Agent(const OID &obj) : Object(obj) {}
		virtual ~Agent() { }
	};
};

#endif
