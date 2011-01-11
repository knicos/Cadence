/*
 * includes/doste/doste.h
 * 
 * Global common doste header.
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

#ifndef _doste_H_
#define _doste_H_

#include <cadence/doste/doste.h>
#include <cadence/agent.h>
#include <cadence/notation.h>

/**
 * All doste components are within the doste namespace. Before using
 * doste you must call doste::initialise() and when finished call
 * doste::finalise().
 * @author Nicolas Pope
 */
namespace cadence {
	extern XARAIMPORT void initialise(int argc, char *argv[]);
	extern XARAIMPORT void finalise();
	extern XARAIMPORT void update();
	extern XARAIMPORT void run();
	
	void makepath(char *buffer, int path, const char *filename);
	
	static const int PATH_SCRIPTS = 1;
	static const int PATH_MODULES = 2;
	
	class XAgent : public Agent {
		public:
		XAgent(const OID &obj) : Agent(obj) { registerEvents(); };
		~XAgent() {};
		
		BEGIN_EVENTS(Agent);
		EVENT(evt_cwd, (*this)("cwd"));
		EVENT(evt_softagent, (*this)("agents")(cadence::doste::All));
		EVENT(evt_notation, (*this)("notations")(cadence::doste::All));
		EVENT(evt_modules, (*this)("modules")(cadence::doste::All));
		EVENT(evt_cout, (*this)("cout"));
		EVENT(evt_error, (*this)("error"));
		EVENT(evt_warning, (*this)("warning"));
		EVENT(evt_debug, (*this)("debug"));
		EVENT(evt_info, (*this)("info"));
		//Notation event
		END_EVENTS;
	};
};

#endif
