/*
 * includes/doste/notation.h
 * 
 * Base class for custom notations
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


#ifndef _doste_NOTATION_H_
#define _doste_NOTATION_H_

#include <cadence/stream.h>
#include <cadence/vector.h>
#include <cadence/doste/oid.h>
#include <cadence/agent.h>

namespace cadence {
	class XARAIMPORT Notation : public Agent {
		public:
		
		OBJECT(Agent, Notation);
		
		Notation() { registerEvents(); };
		Notation(const OID &obj) : Agent(obj) { registerEvents(); };
		virtual ~Notation() {};
		
		Stream *stream;
		
		virtual bool statement(const doste::OID &) { return false; };	//Implement this are the first parse point.

		OID run(const char *filename, const OID &base);
		OID execute(const char *str) { return execute(str, get("root")); };
		OID execute(const char *str, const OID &base);
		OID execute(Stream *s, const OID &base);
		
		const OID &result() const { return m_result; };
		
		//Events to trigger on execute and run
		BEGIN_EVENTS(Agent);
		EVENT(evt_run, (*this)("run"));
		EVENT(evt_execute, (*this)("execute"));
		END_EVENTS;
		
		protected:

		template <typename T>
		bool parse(T tok) {
			return tok.parse(*stream);
		};

		template <typename T>
		bool parseValue(T &tok) {
			return tok.parse(*stream);
		};
		
		protected:
		OID m_result;
		int m_lines;
		const char *m_source;
	};
};

/** Macro to write dasm code in c++ */
#include <cadence/doste/doste.h>
#define _DASM(A) ((cadence::Notation*)(cadence::doste::root.get("notations").get("dasm")))->execute(#A, *this)
#define _DASM2(A) ((cadence::Notation*)(cadence::doste::root.get("notations").get("dasm")))->execute(#A, cadence::doste::root)


#endif
