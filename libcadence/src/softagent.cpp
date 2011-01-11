/*
 * src/softagent.cpp
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

#include "softagent.h"
#include <cadence/messages.h>

using namespace cadence;
using namespace cadence::doste;

SoftAgent::SoftAgent() : m_aid(-1) {
	//std::cout << "Making Soft Agent2\n";
	//AgentHandler::add(this, 0, (*this)("observable"), 0);
	AgentHandler::add(this, 0, (*this)("condition"), AgentHandler::AE_CONDITIONAL);
	AgentHandler::add(this, 1, (*this)("code"), 0);
}

SoftAgent::SoftAgent(const OID &obj)
 : Agent(obj), m_aid(-1) {
 	//std::cout << "Making Soft Agent\n";
	//AgentHandler::add(this, 0, (*this)("observable"), 0);
	AgentHandler::add(this, 0, (*this)("condition"), AgentHandler::AE_CONDITIONAL);
	AgentHandler::add(this, 1, (*this)("code"), 0);
} 

SoftAgent::~SoftAgent() {

}

bool SoftAgent::notify(int aid, int id) {
	if (id == 0) onExecute();
	else if (id == 1) onCodeChange();
	return true;
}

void SoftAgent::onTriggerChange() {
	//Remove previous triggers?
	//if (m_aid == -1)
	//	m_aid = AgentHandler::add(this, 2, object()(observable()), (immediate()) ? AgentHandler::AE_IMMEDIATE : 0);
	//else
	//	AgentHandler::update(m_aid, this, 2, object()(observable()), (immediate()) ? AgentHandler::AE_IMMEDIATE : 0);
	//if (
}

void SoftAgent::onExecute() {
	//Loop through code
	std::cout << "EXECUTE\n";
	
	OID c = code();
	int size = c.get(Size);
	int t1,t2,t3;
	
	OID reg[MAX_REGISTERS];
	
	for (int i=0; i<size; i++) {
		switch ((int)c.get(i)) {
		case SSET:	t1 = (int)c.get(++i);
				t2 = (int)c.get(++i);
				t3 = (int)c.get(++i);
				reg[t1].set(reg[t2], reg[t3]); break;
				
		case ASET:	t1 = (int)c.get(++i);
				t2 = (int)c.get(++i);
				t3 = (int)c.get(++i);
				reg[t1].set(reg[t2], reg[t3], true); break;

		case SGET:	t1 = (int)c.get(++i);
				t2 = (int)c.get(++i);
				t3 = (int)c.get(++i);
				reg[t1] = reg[t2].get(reg[t3]); break;

		case SDEF:	t1 = (int)c.get(++i);
				t2 = (int)c.get(++i);
				t3 = (int)c.get(++i);
				reg[t1].definefuture(reg[t2], reg[t3]); break;

		case ADEF:	t1 = (int)c.get(++i);
				t2 = (int)c.get(++i);
				t3 = (int)c.get(++i);
				reg[t1].definefuture(reg[t2], reg[t3], true); break;

		case GDEF:	break; //reg[c.get(++i)] = reg[c.get(++i)].get(reg[c.get(++i)]); break;
		case GFUN:	break; //reg[c.get(++i)] = reg[c.get(++i)].get(reg[c.get(++i)]); break;

		case SFUN:	t1 = (int)c.get(++i);
				t2 = (int)c.get(++i);
				t3 = (int)c.get(++i);
				reg[t1].define(reg[t2], reg[t3]); break;

		case AFUN:	t1 = (int)c.get(++i);
				t2 = (int)c.get(++i);
				t3 = (int)c.get(++i);
				reg[t1].define(reg[t2], reg[t3], true); break;

		case CREA:	t1 = (int)c.get(++i);
				reg[t1] = OID::create(); break;
		
		case ISET:	t1 = (int)c.get(++i);
				reg[t1] = c.get(++i); break;
				
		default:	Error(0, "An invalid agent instruction was encountered");
		}
	}
}

void SoftAgent::onCodeChange() {
	//Recompile JIT?
}
