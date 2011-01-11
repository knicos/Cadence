/*
 * includes/doste/dmsg.h
 * 
 * Deprecated.
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

#ifndef _doste_DMSG_H_
#define _doste_DMSG_H_

#include <cadence/dstring.h>
#include <cadence/dll.h>

#undef ERROR

namespace cadence {
	/**
	 * DOSTE Message Class. Used throughout DOSTE as a simple way to output
	 * information, errors and warnings to the console using colours. This
	 * will be changed or replaced with a better system soon.
	 * @author Nicolas Pope
	 */
	class XARAIMPORT DMsg {
		public:
		DMsg() : m_type(INFO) {};
		DMsg(unsigned int type) : m_type(type) {
		};
		~DMsg() {};

		typedef unsigned int MsgFormatter;

		friend XARAIMPORT DMsg &operator<<(DMsg &m, const char *str);
		friend XARAIMPORT DMsg &operator<<(DMsg &m, const doste::OID &o);
		friend XARAIMPORT DMsg &operator<<(DMsg &m, int v);
		friend XARAIMPORT DMsg &operator<<(DMsg &m, DMsg::MsgFormatter f);
		
		static const MsgFormatter WARNING=1;
		static const MsgFormatter ERROR=2;
		static const MsgFormatter INFO=0;
		//static const MsgFormatter DEBUG=3;
		static const MsgFormatter FATAL=4;
		
		private:
		unsigned int m_type;
	};
};

#endif
