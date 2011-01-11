/*
 * src/softagent.h
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

#ifndef _doste_SOFTAGENT_H_
#define _doste_SOFTAGENT_H_

#include <cadence/agent.h>

namespace cadence {
	namespace doste {
		class OID;
	};
	
	class SoftAgent : public Agent {
		public:
		
		OBJECT(Agent,SoftAgent);
		
		SoftAgent();
		SoftAgent(const doste::OID &obj);
		~SoftAgent();
		
		bool notify(int aid, int id);
		
		PROPERTY_RF(bool, immediate, "immediate");
		PROPERTY_WF(bool, immediate, "immediate");
		
		PROPERTY_RF(bool, conditional, "condition");
		PROPERTY_WF(bool, conditional, "condition");
		
		PROPERTY_RF(doste::OID, code, "code");
		PROPERTY_WF(doste::OID, code, "code");
		
		//Events to detect object, observable or code changes.
		void onTriggerChange();
		void onCodeChange();
		void onExecute();
		
		static const unsigned int MAX_REGISTERS = 10;
		static const int SSET = 0;
		static const int ASET = 1;
		static const int SDEF = 2;
		static const int ADEF = 3;
		static const int SFUN = 4;
		static const int AFUN = 5;
		static const int SGET = 6;
		static const int GDEF = 7;
		static const int GFUN = 8;
		static const int CREA = 9;
		static const int ISET = 10;
		
		private:
		int m_aid;
		//Compiled code (JIT) might go here.
	};
};

#endif
