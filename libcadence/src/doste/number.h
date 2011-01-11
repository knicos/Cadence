/*
 * src/dvm/number.h
 * 
 * Provides numerical operations on integer objects.
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

#ifndef _doste_DVM_NUMBER_H_
#define _doste_DVM_NUMBER_H_

#include <cadence/doste/handler.h>

namespace cadence {
	namespace doste {
		/**
		 * Internal DOSTE class for number processing. It receives events
		 * relating to integer or floating point operations and calculates
		 * the result.
		 * @author Nicolas Pope
		 */
		class Number : public Handler {
			public:
			Number();
			~Number();
			
			bool handle(Event &evt);
		};
	
		/**
		 * Internal DOSTE class for number processing. This receives events
		 * directly from numbers and returns an OID corresponding to the requested
		 * operation. This returned OID will later be handled by the Number class.
		 * @see doste::Handlers::Number
		 * @author Nicolas Pope
		 */
		class NumOp : public Handler {
			public:
			NumOp();
			~NumOp();
			
			bool handle(Event &evt);
		};
	};
};

#endif
