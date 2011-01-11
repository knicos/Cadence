/*
 * src/dvm/doste.cpp
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

#include <cadence/doste/doste.h>
//#include <doste/local.h>
//#include <doste/handlers/number.h>
#include <cadence/doste/processor.h>
#include <cadence/doste/event.h>
#include <cadence/dll.h>
#include <cadence/map.h>
#include <cadence/memory.h>
#include <cadence/doste/buffer.h>
//#include <doste/object.h>
//#include <doste/notations/dasm.h>
//#include <doste/file.h>
#include "local.h"
#include "number.h"

cadence::doste::OID DVMIMPORT cadence::doste::root;
unsigned int DVMIMPORT cadence::doste::debug = 0;
cadence::doste::Processor *proc;
bool running = true;

#include "names.h"
extern cadence::doste::Names *names;

#ifdef LINUX
//#include <pthread.h>
//#define MAX_THREADS 4
//pthread_t thread[MAX_THREADS];
//pthread_attr_t pattr;
#endif

#undef True

void *thread_func(void *p) {
	//doste::Processor *proc = (doste::Processor*)p;
	cadence::doste::Processor *proc = NEW cadence::doste::Processor();

	while (running) {
		//std::cout << "PROC2\n";
		cadence::doste::Processor::processAll();
		//usleep(1);
	}
	delete proc;

	//#ifdef LINUX
	//pthread_exit(NULL);
	//#endif
	return NULL;
}

void cadence::doste::initialise(const OID &base, int n) {
        names = NEW Names;

	cadence::doste::OID rbase(base.a(),base.b(),0,0);
	cadence::doste::OID::local(rbase);

	Event::initialise();
	Processor::init(n);
	
	new cadence::doste::Processor();

	new VStore();
	new Number();
	root = OID::create(); //Make the root object.

	Event *evt = NEW Event(Event::ADD_REF, root);
	evt->send(Event::FLAG_FREE);

	//Now register the file objects
	//Object::registerType<File>();
	//Object::registerType<LocalFile>();

	#ifdef LINUX
	//pthread_attr_init(&pattr);
	//pthread_attr_setscope(&pattr, PTHREAD_SCOPE_SYSTEM);
	//pthread_attr_setdetachstate(&pattr, PTHREAD_CREATE_DETACHED);
	//One for each secondary processor that the system has.
	//for (int i=1; i<n; i++) {
	//	std::cout << "Making a processor\n";
	//	pthread_create(&thread[i-1], &pattr, thread_func, 0);
	//	usleep(10);
	//}
	//usleep(1000);
	//pthread_create(&thread2, &pattr, thread_func, 0);
	#endif
	
	//Now make the math functions
	root["math"] = OID::create();
	root["math"]["tan"] = OID(0,50,0,1);
	root["math"]["atan"] = OID(0,50,0,2);
	root["math"]["atan2"] = OID(0,50,0,3);
	root["math"]["sqrt"] = OID(0,50,0,4);
	root["math"]["sin"] = OID(0,50,0,5);
	root["math"]["cos"] = OID(0,50,0,6);
	root["math"]["_random"] = OID(0,50,0,7);
	//((OID)(root["math"])).function("random", OID(0,50,0,7)(Void));
	
	//Now create the builtin notations
	//doste::Object::registerType<DASMAgent>();
	//OID notations = OID::create();
	//root["notations"] = notations;
	//OID dasm = OID::create();
	//notations["dasm"] = dasm;
	//DASMAgent *dagent = NEW DASMAgent(dasm);
}

void cadence::doste::finalise() {
	running = false;

	//#ifdef LINUX
	//pthread_attr_destroy(&pattr);
	//pthread_join(thread, NULL);
	//#endif

	Processor::final();
	Event::finalise();
	Buffer::finalise();
	//Delete the handler.
	//Delete the processor.
	
	delete names;
}
