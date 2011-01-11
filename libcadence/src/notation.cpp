/*
 * src/notation.cpp
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

#include <cadence/notation.h>
#include <cadence/file.h>
#include <cadence/messages.h>
#include <cadence/doste/doste.h>
#include <cadence/messages.h>
#include <cadence/memory.h>
#include <string.h>

using namespace cadence;
using namespace cadence::doste;

namespace cadence {
	OnEvent(Notation, evt_run) {
		if (get("run") == Null) return;

		File *s = get("run");
		
		dstring sfile(s->get("filename"));
		char buf[1000];
		sfile.toString(buf,1000);
		const char *source = strrchr(buf, '/');
		if (source == 0) source = buf;
		m_source = source;

		if (s == 0 || !s->open(File::READ)) {
			Warning(Warning::SCRIPT_NOT_FOUND, "Failed to load script " + ((s!=0) ? sfile : ""));
			return;
		}

		Info(Info::LOADING, "Loading script " + sfile);
		OID res = execute(s, cadence::doste::root);
		s->close();
	}
	
	OnEvent(Notation, evt_execute) {
		if (get("execute") == Null) return;

		dstring s(get("execute"));
		//std::cout << "Executing: " << (const char*)s << "\n";
		execute((const char *)s, cadence::doste::root);
	}
	
	IMPLEMENT_EVENTS(Notation, Agent);
};

OID Notation::run(const char *filename, const OID &base) {

	File *s = NEW LocalFile(filename);
	
	const char *source = strrchr(filename, '/');
	if (source == 0) source = filename;
	m_source = source;

	if (!s->open(File::READ)) {
		Warning(Warning::SCRIPT_NOT_FOUND, dstring("Failed to load script ") + filename);
		return Null;
	}
	Info(Info::LOADING, dstring("Loading script ") + filename);
	OID res = execute(s, base);
	s->close();
	delete s;
	return res;
}

OID Notation::execute(const char *str, const OID &base) {
	StringStream *s = NEW StringStream(str);
	m_source = "execute";
	OID res = execute(s, base);
	delete s;
	return res;
}

OID Notation::execute(Stream *s, const OID &base) {
	Stream *oldstream = stream;
	OID oldres = m_result;
	int oldlines = m_lines;
	m_lines = 0;
	stream = s;
	while (statement(base));
	stream = oldstream;
	OID newres = m_result;
	m_result = oldres;
	m_lines = oldlines;
	return newres;
}
