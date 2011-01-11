/*
 * src/doste.cpp
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

#include <cadence/cadence.h>
#include <cadence/doste/processor.h>
#include <cadence/object.h>
#include <cadence/agenthandler.h>
#include <cadence/notation.h>
#include <cadence/dstring.h>
#include <cadence/memory.h>
#include "module.h"
#include "softagent.h"
#include "dasm.h"
#include <cadence/file.h>
#include <cadence/directory.h>
#include <stdlib.h>
#include <string.h>

#ifdef LINUX
#include <unistd.h>
#include <sys/time.h>
#include <dlfcn.h>
//#include <pthread.h>
#include <fcntl.h>
//#include <ncurses.h>
//pthread_t uthread;
//pthread_attr_t uattr;
#endif

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#include <conio.h>
#endif

#ifndef WIN32
#include <errno.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
//#include <pthread.h>
#endif

#undef ERROR

using namespace cadence;
using namespace cadence::doste;

//DObject static data
cadence::Map Object::s_map;
cadence::Map Object::s_reg;
cadence::List<Object*> Object::s_list;

bool interactive = false;
bool settime = true;
bool service = false;
sockaddr_in localAddr;
int service_sock = -1;

OID dasm;

double ttime;
double ttime_last = 0.0;
double ttime_draw = 0.0;
double dtime = 0.0;
long long startticks;

extern "C" {
int cadence_check(int v) {
	return v*v;
}
}

void cadence::makepath(char *buffer, int path, const char *filename) {
	switch(path) {
		case PATH_SCRIPTS:	strcpy(buffer, getenv("CADENCE_SCRIPTS")); break;
		case PATH_MODULES:	strcpy(buffer, getenv("CADENCE_MODULES")); break;
		default:		strcpy(buffer, ".");
	}
	strcat(buffer, "/");
	strcat(buffer, filename);
}

namespace cadence {
	OnEvent(XAgent, evt_cwd) {
		dstring s((OID)((*this)["cwd"]));
		char buffer[400];
		s.toString(buffer,400);
		chdir(buffer);
	}
	
	OnEvent(XAgent, evt_softagent) {
		//Make all softagents
		OID ag = (*this)["agents"];
		for (OID::iterator i=ag.begin(); i!=ag.end(); i++) {
			(Object*)(ag.get(*i));
			//std::cout << "Found agent\n";
		}
	}
	
	OnEvent(XAgent, evt_notation) {
		//Make all softagents
		OID ag = (*this)["notations"];
		for (OID::iterator i=ag.begin(); i!=ag.end(); i++) {
			(Notation*)(ag.get(*i));
			//std::cout << "Found notation\n";
		}
	}

	OnEvent(XAgent, evt_modules) {
		//Make all softagents
		OID ag = (*this)["modules"];
		for (OID::iterator i=ag.begin(); i!=ag.end(); i++) {
			if (*i != This)
			(Module*)(ag.get(*i));
			//std::cout << "Found module\n";
		}
	}

	OnEvent(XAgent, evt_cout) {
		//DEPENDENCY(*this, "cout2");
		//std::cout << (const char*)dstring(get("cout"));
	}

	OnEvent(XAgent, evt_error) {
		if (get("error") == Null) return;

		#ifdef LINUX
		std::cout << "\e[31;1m";
		#else
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
		#endif

		std::cout << (const char*)dstring(get("error").get("message")) << "\n";;

		#ifdef LINUX
		std::cout << "\e[0m";
		#else
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		#endif

		std::cout.flush();
	}

	OnEvent(XAgent, evt_warning) {
		if (get("warning") == Null) return;

		#ifdef LINUX
		std::cout << "\e[33m";
		#else
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		#endif

		std::cout << (const char*)dstring(get("warning").get("message")) << "\n";;

		#ifdef LINUX
		std::cout << "\e[0m";
		#else
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		#endif

		std::cout.flush();
	}

	OnEvent(XAgent, evt_debug) {
		if (get("debug") == Null) return;

		#ifdef LINUX
		std::cout << "\e[34;1m";
		#else
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
		#endif

		std::cout << (const char*)dstring(get("debug").get("message")) << "\n";;

		#ifdef LINUX
		std::cout << "\e[0m";
		#else
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		#endif

		std::cout.flush();
	}

	OnEvent(XAgent, evt_info) {
		if (get("info") == Null) return;
		std::cout << (const char*)dstring(get("info").get("message")) << "\n";;
		std::cout.flush();
	}
	
	IMPLEMENT_EVENTS(XAgent, Agent);
}

long long getTicks() {
	#ifdef LINUX
	unsigned long long ticks;
	struct timeval now;
	gettimeofday(&now, NULL);
	ticks = ((unsigned long long)now.tv_sec) * (unsigned long long)1000000 + ((unsigned long long)now.tv_usec);
	return ticks;
	#endif

	#ifdef WIN32
	LARGE_INTEGER tks;
	QueryPerformanceCounter(&tks);
	return (((unsigned long long)tks.HighPart << 32) + (unsigned long long)tks.LowPart);
	#endif
}

void cadence::initialise(int argc, char *argv[]) {
	int n = 1;
	OID base = OID(1,0,0,0);
	int ai = 0;
	int arg_exec[20]; //NOTE: do not hard code.
	int port = 9001;
	
	startticks = getTicks();

	for (int i=1; i<argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'b':	base = OID(argv[++i]);
					break;
			case 'n':	n = atoi(argv[++i]);
					break;
			case 'i':	interactive = true;
					break;
			case 't':	settime = false;
					break;
			case 'd':	cadence::doste::debug = atoi(argv[++i]);
					break;
			case 'e':	arg_exec[ai++] = ++i;
					break;
			case 's':	service = true;
					break;
			case 'p':	port = atoi(argv[++i]);
					break;

			default:	break;
			}
		}
	}

	cadence::doste::initialise(base, n);
	cadence::LocalFile::initialise();
	
	new AgentHandler();
	root["running"] = true;
	char *buf = new char[1000];
	getcwd(buf,1000);
	root["cwd"] = dstring(buf);
	delete [] buf;
	root["agents"] = OID::create();
	OID notations = OID::create();
	root["notations"] = notations;
	root["workdir"] = OID(0,101,0,0);
	dasm = OID::create();
	dasm["type"] = OID("DASM");
	dasm["root"] = root;
	notations["dasm"] = dasm;
	Processor::processAll();

	new Directory();

	Object::registerType<SoftAgent>();
	Object::registerType<Notation>();
	Object::registerType<DASM>();
	Object::registerType<File>();
	Object::registerType<LocalFile>();
	Object::registerType<Module>();
	new XAgent(root);

	if (ai > 0) {
		//DASM dasm(doste::dvm::root);
		for (int i=0; i<ai; i++) {
			dasm["execute"] = dstring(argv[arg_exec[i]]);
			Processor::processAll();
		}
	}
	
	#ifdef win32
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	#endif
}

void cadence::finalise() {
	#ifndef WIN32
	close(service_sock);
	#else
	closesocket(service_sock);
	#endif

	Object::destroyAll();
	cadence::doste::finalise();
	
	if (interactive) {
		//endwin();
	}
	
	#ifdef DEBUG
	DisplayLeaks();
	#endif
}

void cadence::update() {
	//#ifdef LINUX
        ttime = ((double) getTicks() - (double) startticks) / 1000000.0;
	//#endif
	//#ifdef WIN32
	//LARGE_INTEGER fq;
	//QueryPerformanceFrequency(&fq);
	//ttime = ((double)getTicks() - (double)startticks) / (double)(((unsigned long long)fq.HighPart << 32) + (unsigned long long)fq.LowPart);
	//#endif

	if (settime) root["time"] = ttime;
	//root["frametime"] = ttime - ttime_last;
	//std::cout << "Update FPS: " << 1.0 / (ttime - ttime_last) << "\n";

	ttime_draw += (ttime - ttime_last);
	dtime = (ttime - ttime_last);
	ttime_last = ttime;

	//Processor::processInstant();
	Module::updateAll(dtime);
}

#ifdef WIN32
typedef int socklen_t;
#endif

void cadence::run() {
	char *ibuf = NEW char[10000];
	int pos = 0;
	int count;
	timeval block;
	int selres;
	sockaddr_in fromaddr;
	socklen_t fromlength;
	OID res;
	fd_set fdread;
	
	//DASM dasm;
	
	#ifdef LINUX
	fcntl(0, F_SETFL, O_NONBLOCK);
	#endif
	if (interactive) {
		std::cout << "dasm> ";
		std::cout.flush();
	}
	
	dasm.set("root", root);

	while (root["running"] == true) {
		
		#ifdef LINUX
		ttime = ((double) getTicks() - (double) startticks) / 1000000.0;
		#endif
		#ifdef WIN32
		LARGE_INTEGER fq;
		QueryPerformanceFrequency(&fq);
		ttime = ((double)getTicks() - (double)startticks) / (double)(((unsigned long long)fq.HighPart << 32) + (unsigned long long)fq.LowPart);
		#endif
		
		if (settime) root["time"] = ttime;
		//root["frametime"] = ttime - ttime_last;
		//std::cout << "Update FPS: " << 1.0 / (ttime - ttime_last) << "\n";

		ttime_draw += (ttime - ttime_last);
		dtime = (ttime - ttime_last);
		ttime_last = ttime;

		
		if (interactive) {
			/*if (pos == 0) {
				std::cout << "> ";
			} else {
				std::cout << "  ";
			}
			std::cout.flush();
			std::cin.getline(&ibuf[pos],10000);
			dasm.execute(ibuf, root);
			pos = 0;*/
			
			//count = std::cin.readsome(&ibuf[pos], 10000-pos);
			#ifdef LINUX
			count = read(0, &ibuf[pos], 10000-pos);
			#else
			
			if(_kbhit()) {
				ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), &ibuf[pos], 10000-pos, (unsigned long *)&count, 0);
			} else {
				count = 0;
			}

			#endif
			if (count > 0) {
				pos += count;
				ibuf[pos] = 0;
					
				if (ibuf[pos-1] == '\n') {
					//We have reached the end of an input statement
					pos = 0;
					((DASM*)dasm)->execute(ibuf);
					DMsg msg(DMsg::INFO);
					res = dasm.get("result");
					//res = ((Notation*)(doste::dvm::root.get("notations").get("dasm")))->execute(ibuf,root);
					if (!res.isReserved() && res.get(Size) != Null && res.get(0).isChar()) {
						dstring(res).toString(ibuf,1000);
						std::cout << "  \"" << ibuf << "\"\n";
					} else {
						res.toString(ibuf, 1000);
						std::cout << "  " << ibuf << "\n";
					}
					std::cout << "dasm> ";
					std::cout.flush();
				}
			}
		}

		//Processor::processAll();
		if (!Processor::processInstant() && settime) {
			root["stable"] = Void;
		}
		//AgentHandler::processAll();
		
		//if ((ttime_stat - ttime) >= 1.0) {
		//	ttime_stat = ttime;
		//	std::cout << "EPS: " << Processor::getEventCount() << "\n";
		//}
		
		//Every 60th of a second call Module::updateAll();
		//if (ttime_draw >= 0.016) {
			//if (settime) root["frametime"] = ttime_draw;
			//ttime_draw = 0.0;
			//std::cout << "UPDATE ALL\n";
			Module::updateAll(dtime);
		//}
	}

	delete [] ibuf;
}
