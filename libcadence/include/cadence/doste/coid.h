/*
 * includes/doste/dvm/coid.h
 * 
 * OID Compression
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

#ifndef _doste_DVM_COID_H_
#define _doste_DVM_COID_H_

#include <cadence/dll.h>

namespace cadence {
	namespace doste {
		class OID;
		
		/**
		* Compressed OID. OIDs are 128 bit (16 byte) numbers in their fully expanded
		* state, so the methods in this class can be used to compress and decompress
		* these OIDs. It does this by replacing common OIDs with some reduced form.
		* @author Nicolas Pope
		*/
		class DVMIMPORT COID {
			public:
			/**
			* Compress an OID into a buffer. The buffer must be at least
			* 17 bytes big to fit the worst case compression.
			* @param buf Buffer to write the compressed OID into.
			* @param o The OID to compress.
			* @return Number of bytes for compressed OID.
			*/
			static int compress(unsigned char *buf, const OID &o);
			
			/**
			* Decompress and OID from a buffer. The buffer must contain
			* a COID generated from the compress function.
			* @param buf Compress OID buffer.
			* @param o The resulting OID is put in here.
			* @return Number of bytes read from buffer.
			*/
			static int decompress(const unsigned char *buf, OID &o);
			
			private:
			static const unsigned char COID_NULL = 0;
			static const unsigned char COID_THIS = 1;
			static const unsigned char COID_SPECIAL = 2;
			static const unsigned char COID_INT = 3;
			static const unsigned char COID_LONG = 4;
			static const unsigned char COID_FULL = 5;
			static const unsigned char COID_INDEX = 6;
			static const unsigned char COID_CHAR = 7;
			static const unsigned char COID_CINT = 8;
			static const unsigned char COID_SINT = 9;
			static const unsigned char COID_MODIF = 10;
			static const unsigned char COID_MOD_SET = 11;
			static const unsigned char COID_MOD_NODEP = 12;
			static const unsigned char COID_MOD_DEF = 13;
			static const unsigned char COID_BEGINDEF = 14;
			static const unsigned char COID_ENDDEF = 15;
			static const unsigned char COID_BEGINLIST = 16;
			static const unsigned char COID_ENDLIST = 17;
			static const unsigned char COID_MOD_SEQ = 18;
		};
	};
};

#endif
