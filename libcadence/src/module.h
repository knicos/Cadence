/*
 * src/module.h
 *
 * Loads DLL modules at run-time.
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

#ifndef _doste_MODULE_H_
#define _doste_MODULE_H_

#include <cadence/agent.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef LINUX
#include <dlfcn.h>
#endif

namespace cadence {
	class Module : public Agent {
		public:

		OBJECT(Agent, Module);

		Module(const OID &obj);
		~Module();

		void update();
		
		BEGIN_EVENTS(Agent);
		EVENT(evt_chgfreq, (*this)("frequency"));
		END_EVENTS;
		
		static void updateAll(double dtime);

		private:
		#ifdef LINUX
		void *m_handle;
		#else
		HMODULE m_handle;
		#endif
		void *m_update;
		void *m_init;
		bool m_docreate;
		bool m_doupdate;
		doste::OID m_base;
		double m_ltime;
		double m_freq;
		
		static const unsigned int MAX_MODULES = 50;
		static Module *s_modules[MAX_MODULES];
	};
};

#endif
