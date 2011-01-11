/*
 * src/agenthandler.cpp
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

#include <cadence/agenthandler.h>
#include <cadence/agent.h>
#include <cadence/doste/event.h>
#include <cadence/memory.h>

using namespace cadence;
using namespace cadence::doste;

Vector<AgentHandler::AgentEntry*> AgentHandler::s_agents;
AgentHandler::AgentQueue AgentHandler::s_queues[2];
int AgentHandler::s_curq = 0;

AgentHandler::AgentHandler()
 : Handler(OID::local()+OID(0,0,15,0),OID::local()+OID(0,0,15,0xFF)) {
	
}

AgentHandler::~AgentHandler() {
	for (int i=0; i<s_agents.size(); i++) {
		if (s_agents[i] != 0)
			delete s_agents[i];
	}
}

bool AgentHandler::handle(Event &evt) {

	if (evt.type() == Event::DEFINE) {
		//Send notify message.
		Event *evt2 = NEW Event(Event::NOTIFY, OID::local()+OID(0,0,15,0));
		evt2->param<0>(evt.param<0>());
		evt2->send(Event::FLAG_FREE);
	} else if (evt.type() == Event::NOTIFY || evt.type() == Event::NOTIFY_IS) {
		
		if (s_agents[evt.param<0>()] == 0) return true;
		
		//Evaluate condition
		OID res = s_agents[evt.param<0>()]->cond.evaluate(OID::local()+OID(0,0,15,0), evt.param<0>());
		if (((s_agents[evt.param<0>()]->flags & AE_CONDITIONAL) == 0) || (res == doste::True)) {
			//Check immediate
			//if (s_agents[evt.param<0>()]->flags & AE_IMMEDIATE) {
				s_agents[evt.param<0>()]->agent->notify(evt.param<0>(), s_agents[evt.param<0>()]->localid);
			//} else {
				//Add to queue.
				//s_queues[s_curq].q[s_queues[s_curq].put++] = evt.param<0>();
			//}
		}
	}

	return true;
}

int AgentHandler::add(BaseAgent *agent, int id, Definition cond, unsigned int flags) {
	AgentEntry *newae = NEW AgentEntry;
	newae->agent = agent;
	newae->localid = id;
	newae->cond = cond;
	newae->flags = flags;
	s_agents.add(newae);

	//Evaluate condition and call notify if needed.
	Event *evt = NEW Event(Event::DEFINE, OID::local()+OID(0,0,15,0));
	evt->param<0>(s_agents.size()-1);
	evt->param<1>(cond);
	evt->send(Event::FLAG_FREE);

	return s_agents.size()-1;
}

void AgentHandler::update(int aid, BaseAgent *agent, int id, Definition cond, unsigned int flags) {
	AgentEntry *newae = s_agents[aid];
	newae->agent = agent;
	newae->localid = id;
	newae->cond = cond;
	newae->flags = flags;

	//Evaluate condition and call notify if needed.
	Event *evt = NEW Event(Event::DEFINE, OID::local()+OID(0,0,15,0));
	evt->param<0>(aid);
	evt->param<1>(cond);
	evt->send(Event::FLAG_FREE);
}

void AgentHandler::remove(BaseAgent *agent) {
	for (int i=0; i<s_agents.size(); i++) {
		if (s_agents[i] != 0 && s_agents[i]->agent == agent) {
			delete s_agents[i];
			s_agents.set(i, 0);
		}
	}
}

void AgentHandler::remove(BaseAgent *agent, int id) {

}

void AgentHandler::remove(int id) {

}

void AgentHandler::processAll() {
	int curq = s_curq;
	s_curq = (s_curq == 0) ? 1 : 0;
	s_queues[s_curq].put = 0;
	//Do some crap here.
	//Queue should be unique... ie each agent is only evaluated once.
	//Use two queues and switch between them.

	for (int i=0; i<s_queues[curq].put; i++) {
		//std::cout << "NOTIFY FROM QUEUE\n";
		//s_agents[s_queues[curq].q[i]]->agent->notify(s_agents[s_queues[curq].q[i]]->localid);
	}
}
