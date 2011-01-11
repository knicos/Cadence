/*
 * src/messages.cpp
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

#include <cadence/messages.h>
#include <cadence/doste/doste.h>

using namespace cadence;
using namespace cadence::doste;

Error::Error(int id, dstring msg) {
	OID err = OID::create();
	err.set("message", msg);
	err.set("number", id);
	//Timestamp?
	root.set("error", err, true);
}

Error::Error(int id, dstring msg, OID obj, OID key) {
	OID err = OID::create();
	err.set("message", msg);
	err.set("number", id);
	err.set("object", obj);
	err.set("key", key);
	//Timestamp?
	root.set("error", err, true);
}

Warning::Warning(int id, dstring msg) {
	OID warn = OID::create();
	warn.set("message", msg);
	warn.set("number", id);
	//Timestamp?
	root.set("warning", warn, true);
}

Debug::Debug(int id, dstring msg) {
	OID dbg = OID::create();
	dbg.set("message", msg);
	dbg.set("number", id);
	//Timestamp?
	root.set("debug", dbg, true);
}

Info::Info(int id, dstring msg) {
	OID info = OID::create();
	info.set("message", msg);
	info.set("number", id);
	//Timestamp?
	root.set("info", info, true);
}

