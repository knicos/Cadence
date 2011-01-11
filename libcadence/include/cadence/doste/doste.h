/*
 * includes/doste/dvm/dvm.h
 * 
 * Dependency Virtual Machine
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

#ifndef _doste_DVM_H_
#define _doste_DVM_H_

#include <cadence/doste/oid.h>
#include <cadence/doste/processor.h>
#include <cadence/doste/definition.h>
#include <cadence/dll.h>

namespace cadence {

	/**
	 * The Dependency Virtual Machine namespace. All core database functionality is kept in here.
	 * @author Nicolas Pope
	 * @see OID
	 * @see Event
	 */
	namespace doste {
		void DVMIMPORT initialise(const OID &base, int n);
		void DVMIMPORT finalise();
		
		/** Your computers root database object */
		extern DVMIMPORT cadence::doste::OID root;
		/** Change value to DEBUG_ constants to output database
		 * debug information. Caution, this outputs a massive amount
		 * of data. */
		extern DVMIMPORT unsigned int debug;
	
		static const unsigned int DEBUG_SET_EVENTS = 0x00000001;
		static const unsigned int DEBUG_NOTIFY_EVENTS = 0x00000002;
		static const unsigned int DEBUG_DEFINE_EVENTS = 0x00000004;
		static const unsigned int DEBUG_ADDDEP_EVENTS = 0x00000008;
		static const unsigned int DEBUG_INSTANTS = 0x00000010;
		static const unsigned int DEBUG_GET_EVENTS = 0x00000020;
	};
};

#endif
