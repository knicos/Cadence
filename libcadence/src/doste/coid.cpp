/*
 * src/dvm/coid.cpp
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

#include <cadence/doste/coid.h>
#include <cadence/doste/oid.h>
#include <cadence/doste/modifiers.h>
//#include <doste/dof.h>

using namespace cadence::doste;

int COID::compress(unsigned char *buf, const OID &o) {
	//char code;
	//char tempc;
	//short temps;
	//int tempi;
	int i=0;
	
	if (o == Null) {
		buf[i] = COID_NULL;
		return 1;
	} else if (o == This) {
		buf[i] = COID_THIS;
		return 1;
	} else if (o == modifiers::Set) {
		buf[i] = COID_MOD_SET;
		return 1;
	} else if (o == modifiers::Define) {
		buf[i] = COID_MOD_DEF;
		return 1;
	} else if (o == modifiers::NoDep) {
		buf[i] = COID_MOD_NODEP;
		return 1;
	} else if (o == modifiers::Seq) {
		buf[i] = COID_MOD_SEQ;
		return 1;
	/*} else if (o == DOF::BeginDef) {
		buf[i] = COID_BEGINDEF;
		return 1;
	} else if (o == DOF::EndDef) {
		buf[i] = COID_ENDDEF;
		return 1;
	} else if (o == DOF::BeginList) {
		buf[i] = COID_BEGINLIST;
		return 1;
	} else if (o == DOF::EndList) {
		buf[i] = COID_ENDLIST;
		return 1;*/
	} else if (o.isSpecial() && o.c() == 1) {
		buf[i++] = COID_MODIF;
		buf[i] = o.d();
		return 2;
	} else if (o.isSpecial()) {
		buf[i++] = COID_SPECIAL;
		buf[i++] = o.d() >> 8;
		buf[i] = o.d();
		return 3;
	} else if (o.isName()) {
		buf[i++] = COID_INDEX;
		buf[i++] = o.d() >> 8;
		buf[i] = o.d();
		return 3;
	} else if (o.isChar()) {
		buf[i++] = COID_CHAR;
		buf[i++] = o.d();
		return 2;
	} else if (o.isLongLong()) {
		if (o.c() == 0) {
			if ((int)o.d() < 128 && (int)o.d() > -128) {
				buf[i++] = COID_CINT;
				buf[i] = o.d();
				return 2;
			} else if ((int)o.d() < 32000 && (int)o.d() > -32000) {
				buf[i++] = COID_SINT;
				buf[i++] = o.d() >> 8;
				buf[i] = o.d();
				return 3;
			} else { 
				buf[i++] = COID_INT;
				buf[i++] = o.d() >> 24;
				buf[i++] = o.d() >> 16;
				buf[i++] = o.d() >> 8;
				buf[i++] = o.d();
				return 5;
			}
		} else {
			buf[i++] = COID_LONG;
			//TODO
			return 1;
		}
	} else {
		buf[i++] = COID_FULL;
		buf[i++] = o.a() >> 24;
		buf[i++] = o.a() >> 16;
		buf[i++] = o.a() >> 8;
		buf[i++] = o.a();
		
		buf[i++] = o.b() >> 24;
		buf[i++] = o.b() >> 16;
		buf[i++] = o.b() >> 8;
		buf[i++] = o.b();
		
		buf[i++] = o.c() >> 24;
		buf[i++] = o.c() >> 16;
		buf[i++] = o.c() >> 8;
		buf[i++] = o.c();
		
		buf[i++] = o.d() >> 24;
		buf[i++] = o.d() >> 16;
		buf[i++] = o.d() >> 8;
		buf[i++] = o.d();
		
		return 17;
	}
}

int COID::decompress(const unsigned char *buf, OID &o) {
	switch (buf[0]) {
	case COID_NULL:
		o = Null;
		return 1;
	case COID_THIS:
		o = This;
		return 1;
	case COID_SPECIAL:
		o = OID(0,0,0,((unsigned int)buf[1]<<8)+buf[2]);
		return 3;
	case COID_MODIF:
		o = OID(0,0,1,buf[1]);
		return 2;
	case COID_MOD_SET:
		o = modifiers::Set;
		return 1;
	case COID_MOD_DEF:
		o = modifiers::Define;
		return 1;
	case COID_MOD_NODEP:
		o = modifiers::NoDep;
		return 1;
	case COID_MOD_SEQ:
		o = modifiers::Seq;
		return 1;
	/*case COID_BEGINDEF:
		o = DOF::BeginDef;
		return 1;
	case COID_ENDDEF:
		o = DOF::EndDef;
		return 1;
	case COID_BEGINLIST:
		o = DOF::BeginList;
		return 1;
	case COID_ENDLIST:
		o = DOF::EndList;
		return 1;*/
	case COID_INDEX:
		o = OID(0,5,0,((unsigned int)buf[1] << 8) + buf[2]);
		return 3;
	case COID_CHAR:
		o = OID(0,3,0,buf[1]);
		return 2;
	case COID_CINT:
		o = OID(0,1,0,buf[1]);
		return 2;
	case COID_SINT:
		o = OID(0,1,0,((unsigned int)buf[1] << 8) + buf[2]);
		return 3;
	case COID_INT:
		o = OID(0,1,0,((unsigned int)buf[1] << 24) + ((unsigned int)buf[2] << 16) + ((unsigned int)buf[3] << 8) + (unsigned int)buf[4]);
		return 5;
	case COID_FULL:
		o = OID(	(buf[1]<<24) + (buf[2]<<16) + (buf[3]<<8) + (buf[4]),
				(buf[5]<<24) + (buf[6]<<16) + (buf[7]<<8) + (buf[8]),
				(buf[9]<<24) + (buf[10]<<16) + (buf[11]<<8) + (buf[12]),
				(buf[13]<<24) + (buf[14]<<16) + (buf[15]<<8) + (buf[16]));
		return 17;
	}

	return 1;
}

