/*
 * includes/doste/dvm/modifiers.h
 * 
 * Available definition modifiers.
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

#ifndef _doste_DVM_MODIFIERS_H_
#define _doste_DVM_MODIFIERS_H_

namespace cadence {
	namespace doste {
		/**
		* Modifiers are used to control definition evaluation. For example the SET modifier will
		* cause a SET event to be used with the following two elements as arguments.
		*/
		namespace modifiers {
			static const int SET = 1;		/**< Use a SET event with next elements as key and value. */
			static const int DEFINE = 2;		/**< Use a DEFINE event with next elements as key and definition */
			//static const int DEFINEAGENT = 3;	/**< Use a DEFINE_AGENT event with next elements as key and definition */
			static const int DEFINEFUNC = 4;	/**< Use a DEFINE_FUNC event with next elements as key and definition */
			static const int CREATE = 5;		/**< Create a new OID using the CREATE event */
			//static const int ADDDEP = 6;		/**< Add a dependency using the ADDDEP event */
			static const int SEQ = 7;		/**< Set next element as the current object */
			//static const int RETURN = 8;		/**< Not useful currently. */
			static const int NODEP = 9;		/**< Do not add dependencies on subsequent GETS */
			static const int SYNC = 10;		/**< Not used */
			static const int COMPARE = 11;		/**< Compare two OIDS and put boolean result in current object. */
			//static const int GETRANGE = 12;		/**< Return a buffer object containing a range */
			//static const int GETMULTI = 13;		/**< Return a buffer object after a mult-get EVENT */
			//static const int FORALL = 14;		/**< */
			//static const int FORRANGE = 15;		/**< */
			static const int NOCONTEXT = 16;
			static const int UNION = 17;		
			//static const int LIST = 18;
			//static const int ISET = 19;		/**< Init SET for new objects. */
			//static const int IDEFINE = 20;
			//static const int IFUNC = 21;
			//Bulk operations.
			//static const int CLONE = 22;
			static const int GETDEF = 23;
			static const int GETFLAGS = 24;
			static const int BEGINSUB = 25;
			static const int ENDSUB = 26;
	
			static const OID Set = OID(0,0,1,SET);
			static const OID Define = OID(0,0,1,DEFINE);
			//static const OID DefineAgent = OID(0,0,1,DEFINEAGENT);
			static const OID DefineFunc = OID(0,0,1,DEFINEFUNC);
			static const OID Create = OID(0,0,1,CREATE);
			//static const OID AddDep = OID(0,0,1,ADDDEP);
			static const OID Seq = OID(0,0,1,SEQ);
			//static const OID Return = OID(0,0,1,RETURN);
			static const OID NoDep = OID(0,0,1,NODEP);
			static const OID Sync = OID(0,0,1,SYNC);
			static const OID Compare = OID(0,0,1,COMPARE);
			//static const OID GetRange = OID(0,0,1,GETRANGE);
			//static const OID GetMulti = OID(0,0,1,GETMULTI);
			//static const OID ForAll = OID(0,0,1,FORALL);
			//static const OID ForRange = OID(0,0,1,FORRANGE);
			static const OID NoContext = OID(0,0,1,NOCONTEXT);
			static const OID Union = OID(0,0,1,UNION);
			//static const OID List = OID(0,0,1,LIST);
			//static const OID ISet = OID(0,0,1,ISET);
			//static const OID IDefine = OID(0,0,1,IDEFINE);
			//static const OID IFunc = OID(0,0,1,IFUNC);
			//Bulk operations.
			//static const OID Clone = OID(0,0,1,CLONE);
			static const OID GetDef = OID(0,0,1,GETDEF);
			static const OID GetFlags = OID(0,0,1,GETFLAGS);
			static const OID BeginSub = OID(0,0,1,BEGINSUB);
			static const OID EndSub = OID(0,0,1,ENDSUB);
		};
	};
};

#endif
