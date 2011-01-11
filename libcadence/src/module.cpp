/*
 * src/module.cpp
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

#include "module.h"

#include <cadence/file.h>
#include <cadence/messages.h>
#include <cadence/memory.h>

using namespace cadence;
using namespace cadence::doste;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

Module *Module::s_modules[Module::MAX_MODULES] = {0};

namespace cadence {
	OnEvent(Module, evt_chgfreq) {
		m_freq = get("frequency");
	}
};

IMPLEMENT_EVENTS(Module, Agent);

#undef ERROR
Module::Module(const OID &obj)
 : Agent(obj), m_ltime(0.0) {
	char buf[300];
	File *f = get("file");

	if (f == 0) {
		Error(Error::MODULE_NOT_FOUND, "Invalid module file");
		return;
	}
	f->getLocalFilename(buf,300);
	Info(Info::LOADING, "Loading module: " + f->filename());
	std::cout.flush();

	#ifdef LINUX
	m_handle = dlopen(buf, RTLD_NOW);
	#else
	#ifdef UNICODE
	int len = strlen(buf) + 1;
	wchar_t *wbuf = NEW wchar_t[len];
	mbstowcs(wbuf, buf, len);
	m_handle = LoadLibrary((LPCWSTR)wbuf);
	#else
	m_handle = LoadLibrary(buf);
	#endif
	#endif
	if (m_handle == 0) {
		DMsg msg(DMsg::ERROR);
		#ifdef LINUX
		Error(Error::INVALID_MODULE, dlerror());
		#else
		Error(Error::INVALID_MODULE, dstring("Could not load library [") + (int)GetLastError() + "]");
		#endif
		return;
	}

	//Initialise the module
	#ifdef LINUX
	m_init = dlsym(m_handle, "initialise");
	#else
	m_init = (void*)GetProcAddress(m_handle, "initialise");
	#endif
	if (m_init == 0) {
		Error(Error::INVALID_MODULE, "The module is missing an 'initialise' function");
		return;
	}
	m_base = get("base");
	m_doupdate = get("update");
	m_freq = (double)get("frequency");
	#ifdef LINUX
	//This is a fix for EDEN which already has an 'update' method.
	m_update = dlsym(m_handle, "cadence_update");
	if (m_update == 0)
		m_update = dlsym(m_handle, "update");
	#else
	m_update = (void*)GetProcAddress(m_handle, "update");
	#endif
	//m_docreate = true;
	
	((void (*)(const OID &))m_init)(m_base);
	
	for (unsigned int i=0; i<MAX_MODULES; i++) {
		if (s_modules[i] == 0) {
			s_modules[i] = this;
			break;
		}
	}
	
	registerEvents();
}

Module::~Module() {
	//Finalise the module
	#ifdef LINUX
	void (*finalise)() = (void (*)())dlsym(m_handle, "finalise");
	#else
	void (*finalise)() = (void (*)())GetProcAddress(m_handle, "finalise");
	#endif
	if (finalise == 0) {
		Error(Error::INVALID_MODULE, "The module is missing a 'finalise' function");
		return;
	}
	finalise();

	#ifdef LINUX
	dlclose(m_handle);
	#else
	FreeLibrary(m_handle);
	#endif
}

void Module::update() {
	//if (m_docreate) {
	//	((void (*)(const OID &))m_init)(m_base);
	//	m_docreate = false;
	//}
	if (m_doupdate && m_update) {
		((void (*)())m_update)();
	}
}

void Module::updateAll(double dtime) {
	for (unsigned int i=0; i<MAX_MODULES; i++) {
		if (s_modules[i] != 0) {
			s_modules[i]->m_ltime += dtime;
			if (s_modules[i]->m_ltime >= s_modules[i]->m_freq) {
				s_modules[i]->m_ltime = 0.0;
				s_modules[i]->update();
			}
		} else {
			return;
		}
	}
}
