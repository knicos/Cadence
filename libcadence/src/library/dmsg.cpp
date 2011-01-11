/*
 * src/library/dmsg.cpp
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

#include <cadence/dmsg.h>
#include <cadence/doste/oid.h>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#undef ERROR
#endif

using namespace cadence;
using namespace cadence::doste;

namespace cadence {

	DMsg &operator<<(DMsg &m, DMsg::MsgFormatter f) {
		m.m_type = f;
		return m;
	}
	
	DMsg &operator<<(DMsg &m, int v) {
		m << OID(v);
		return m;
	}

	DMsg &operator<<(DMsg &m, const char *str) {

		#ifdef LINUX
		switch(m.m_type) {
		case DMsg::WARNING:	std::cout << "\e[33m"; break;
		case DMsg::ERROR:	std::cout << "\e[31;1m"; break;
		//case DMsg::DEBUG:	std::cout << "\e[34;1m"; break;
		case DMsg::FATAL:	std::cout << "\e[31;1m";
					break;
		default: break;
		}
		#else
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		switch(m.m_type) {
		case DMsg::WARNING:	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
		case DMsg::ERROR:	SetConsoleTextAttribute(hConsole, FOREGROUND_RED); break;
		case DMsg::DEBUG:	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE); break;
		default: break;
		}
		#endif

		std::cout << str;

		if (m.m_type != DMsg::INFO) {
			#ifdef LINUX
			std::cout << "\e[0m";
			#else
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			#endif
			std::cout.flush();
		}
		return m;
	}
	
	DMsg &operator<<(DMsg &m, const OID &o) {

		#ifdef LINUX
		switch(m.m_type) {
		case DMsg::WARNING:	std::cout << "\e[33m"; break;
		case DMsg::ERROR:	std::cout << "\e[31;1m"; break;
		//case DMsg::DEBUG:	std::cout << "\e[34;1m"; break;
		case DMsg::FATAL:	std::cout << "\e[31;1m";
					break;
		default: break;
		}
		#else
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		switch(m.m_type) {
		case DMsg::WARNING:	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
		case DMsg::ERROR:	SetConsoleTextAttribute(hConsole, FOREGROUND_RED); break;
		//case DMsg::DEBUG:	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE); break;
		default: break;
		}
		#endif

		char buf[40];
		o.toString(buf,40);
		std::cout << buf;

		if (m.m_type != DMsg::INFO) {
			#ifdef LINUX
			std::cout << "\e[0m";
			#else
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			#endif
			std::cout.flush();
		}
		return m;
	}
};
