/*
 * src/dasm.h
 * 
 * The DOSTE notation parser.
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

#ifndef _doste_DASM_H_
#define _doste_DASM_H_

#include <cadence/notation.h>

#ifdef _MSC_VER
#pragma warning ( disable : 4996 )
#endif

namespace cadence {
	class DASM : public Notation {
		public:
		
		OBJECT(Notation, DASM);
		
		DASM();
		DASM(const OID &obj);
		~DASM();
		
		bool statement(const doste::OID &);
		
		PROPERTY_RF(doste::OID, variables, "variables");
		PROPERTY_WF(doste::OID, variables, "variables");

		void error(const char *message);
		
		private:
		bool parseObject(OID &cur);
		bool parseSubExpr(OID &cur);
		bool parseDefinition(OID &cur, int &i, bool real, bool ncontext=false, bool endnl=false);
		bool parseString(OID &cur);
		bool parseIf(int &i, OID &cur, bool imed=false);
		bool parseSelect(int &i, OID &cur, bool imed=false);
		
		//current base object
		doste::OID m_base;
	};
};

#endif
