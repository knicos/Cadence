/*
 * includes/doste/agenthandler.h
 * 
 * Manages and schedules all agents.
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

#ifndef _doste_AGENTHANDLER_H_
#define _doste_AGENTHANDLER_H_

#include <cadence/doste/handler.h>
#include <cadence/doste/definition.h>
#include <cadence/vector.h>

namespace cadence {
	namespace doste {
		class Event;
	};
	class BaseAgent;

	class XARAIMPORT AgentHandler : public doste::Handler {
		public:
		AgentHandler();
		~AgentHandler();

		bool handle(doste::Event &evt);

		static int add(BaseAgent *agent, int id, doste::Definition cond, unsigned int flags);
		static void update(int aid, BaseAgent *agent, int id, doste::Definition cond, unsigned int flags);
		static void remove(int id);
		static void remove(BaseAgent *agent, int id);
		static void remove(BaseAgent *agent);

		static void processAll();

		static const unsigned int AE_CONDITIONAL = 0x0001;
		static const unsigned int AE_IMMEDIATE = 0x0002;

		private:

		struct AgentEntry {
			BaseAgent *agent;
			int localid;
			doste::Definition cond;
			unsigned int flags;
		};

		static const unsigned int MAX_QUEUE_SIZE = 1000;

		static Vector<AgentEntry*> s_agents;
		//Queue of agent notify requests.
		struct AgentQueue {
			int put;
			int get;
			int q[MAX_QUEUE_SIZE];
		};

		static AgentQueue s_queues[2];
		static int s_curq;
	};
};

#endif
