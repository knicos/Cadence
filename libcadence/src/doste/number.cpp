/*
 * src/dvm/number.cpp
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

#include "number.h"
#include <cadence/doste/event.h>
#include <cadence/doste/handler.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>

using namespace cadence::doste;

NumOp::NumOp()
 : Handler(OID(0,16,0,0), OID(0,70,0xFFFFFFFF,0xFFFFFFFF)) {
}

NumOp::~NumOp() {
}

bool NumOp::handle(Event &evt) {
	if (evt.type() == Event::GET) {

		//std::cout << "Numop\n";

		//if (!evt.param<0>().isLongLong()) {
		//	evt.result(Null);
		//	return true;
		//}
		
		switch(evt.dest().b()) {
		case 16:	(evt.param<0>().isDouble()) ? evt.result((double)evt.dest().m_ll + (double)evt.param<0>()) : evt.result(evt.dest().m_ll + (long long)evt.param<0>()); break;
		case 17:	(evt.param<0>().isDouble()) ? evt.result((double)evt.dest().m_ll - (double)evt.param<0>()) : evt.result(evt.dest().m_ll - (long long)evt.param<0>()); break;
		case 18:	evt.result((float)evt.dest().m_ll / (float)evt.param<0>()); break;
		case 19:	(evt.param<0>().isDouble()) ? evt.result((double)evt.dest().m_ll * (double)evt.param<0>()) : evt.result(evt.dest().m_ll * (long long)evt.param<0>()); break;
		case 20:	evt.result(evt.dest().m_ll & (long long)evt.param<0>()); break;
		case 21:	evt.result(evt.dest().m_ll | (long long)evt.param<0>()); break;
		case 22:	evt.result(evt.dest().m_ll < (long long)evt.param<0>()); break;
		case 23:	evt.result(evt.dest().m_ll > (long long)evt.param<0>()); break;
		case 24:	evt.result(evt.dest().m_ll << (long long)evt.param<0>()); break;
		case 25:	evt.result(evt.dest().m_ll >> (long long)evt.param<0>()); break;
		case 26:	evt.result(evt.dest().m_ll % (long long)evt.param<0>()); break;
		
		case 30:	evt.result(evt.dest().m_dbl + (double)evt.param<0>()); break;
		case 31:	evt.result(evt.dest().m_dbl - (double)evt.param<0>()); break;
		case 32:	evt.result(evt.dest().m_dbl / (double)evt.param<0>()); break;
		case 33:	evt.result(evt.dest().m_dbl * (double)evt.param<0>()); break;
		case 34:	evt.result(evt.dest().m_dbl < (double)evt.param<0>()); break;
		case 35:	evt.result(evt.dest().m_dbl > (double)evt.param<0>()); break;
		
		case 50:	if (evt.dest() == OID(0,50,0,1)) evt.result(tan(evt.param<0>().m_dbl));	//TAN
				else if (evt.dest() == OID(0,50,0,2)) evt.result(atan(evt.param<0>().m_dbl));	//ATAN
				else if (evt.dest() == OID(0,50,0,3)) evt.result(OID(0,51, evt.param<0>().c(), evt.param<0>().d()));
				else if (evt.dest() == OID(0,50,0,4)) evt.result(sqrt(evt.param<0>().m_dbl));	//SQRT
				else if (evt.dest() == OID(0,50,0,5)) evt.result(sin(evt.param<0>().m_dbl));	//SIN
				else if (evt.dest() == OID(0,50,0,6)) evt.result(cos(evt.param<0>().m_dbl));	//COS
				else if (evt.dest() == OID(0,50,0,7)) evt.result((float)rand() / (float)RAND_MAX); //random
				break;
				
		case 51:	evt.result(atan2(evt.dest().m_dbl, evt.param<0>().m_dbl)); break;
		}
	
	}

	return true;
}

OID Xor;
OID Shiftleft;
OID Shiftright;
OID Trunc;
OID Lower;
OID Upper;
OID Round;


Number::Number()
 : Handler(OID(0,1,0,0), OID(0,2,0xFFFFFFFF,0xFFFFFFFF)) {
	Xor = OID("xor");
	Shiftleft = OID("shiftleft");
	Shiftright = OID("shiftright");
	Lower = OID("lower");
	Upper = OID("upper");
	Round = OID("round");
	new NumOp();
	
	srand ( (unsigned int)time(NULL) );
}

Number::~Number() {
}

bool Number::handle(Event &evt) {
	if (evt.type() == Event::GET) {

		if (evt.dest().isLongLong()) {
			if (evt.param<0>() == Type) evt.result("Integer");
			else if (evt.param<0>() == Add) evt.result(OID(0,15,0,0)+evt.dest());
			else if (evt.param<0>() == Subtract) evt.result(OID(0,16,0,0)+evt.dest());
			else if (evt.param<0>() == Divide) evt.result(OID(0,17,0,0)+evt.dest());
			else if (evt.param<0>() == Multiply) evt.result(OID(0,18,0,0)+evt.dest());
			else if (evt.param<0>() == And) evt.result(OID(0,19,0,0)+evt.dest());
			else if (evt.param<0>() == Or) evt.result(OID(0,20,0,0)+evt.dest());
			else if (evt.param<0>() == Less) evt.result(OID(0,21,0,0)+evt.dest());
			else if (evt.param<0>() == Greater) evt.result(OID(0,22,0,0)+evt.dest());
			else if (evt.param<0>() == Shiftleft) evt.result(OID(0,23,0,0)+evt.dest());
			else if (evt.param<0>() == Shiftright) evt.result(OID(0,24,0,0)+evt.dest());
			else if (evt.param<0>() == Percent) evt.result(OID(0,25,0,0)+evt.dest());
		} else if (evt.dest().isDouble()) {
			//std::cout << "wtf\n";
			//evt.result(Null);
			if (evt.param<0>() == Add) evt.result(OID(0,28,0,0)+evt.dest());
			else if (evt.param<0>() == Subtract) evt.result(OID(0,29,0,0)+evt.dest());
			else if (evt.param<0>() == Divide) evt.result(OID(0,30,0,0)+evt.dest());
			else if (evt.param<0>() == Multiply) evt.result(OID(0,31,0,0)+evt.dest());
			else if (evt.param<0>() == Less) evt.result(OID(0,32,0,0)+evt.dest());
			else if (evt.param<0>() == Greater) evt.result(OID(0,33,0,0)+evt.dest());
			else if (evt.param<0>() == Lower) evt.result((int)evt.dest());
		}/* else {
			if (evt.param<0>() == Tan) evt.result(OID(0,50,0,1));
			else if (evt.param<0>() == ATan) evt.result(OID(0,50,0,2));
			else if (evt.param<0>() == ATan2) evt.result(OID(0,50,0,3));
			else if (evt.param<0>() == Sin) evt.result(OID(0,50,0,4));
			else if (evt.param<0>() == ASin) evt.result(OID(0,50,0,5));
			else if (evt.param<0>() == Cos) evt.result(OID(0,50,0,6));
			else if (evt.param<0>() == ACos) evt.result(OID(0,50,0,7));
			else if (evt.param<0>() == Sqrt) evt.result(OID(0,50,0,8));
		}*/
	}

	return true;
}
