/*
 * src/dvm/names.h
 * 
 * Maps strings to OIDs and vice versa.
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

#ifndef _doste_DVM_NAMES_H_
#define _doste_DVM_NAMES_H_

#include <cadence/doste/oid.h>

namespace cadence {
	namespace doste {
		class Names {
			public:
			Names();
			~Names();
	
			OID lookup(const char *n);
			const char *lookup(const OID &o);
			void add(const char *n, const OID &o);
	
			private:

			static const unsigned int MAX_NAME_SIZE = 80;
			static const unsigned int NAME_HASH_SIZE = 1000;
	
			struct NameEntry {
				char name[MAX_NAME_SIZE];
				OID id;
				NameEntry *next;
			};
	
			NameEntry *m_hash[NAME_HASH_SIZE];
			int m_lastname;
	
			int hashString(const char *value) {
				unsigned long hash = 0;
				int c;
			
				while ((c = *value++))
				hash = c + (hash << 6) + (hash << 16) - hash;
			
				return hash % NAME_HASH_SIZE;
			}
		};
	};
};

#endif
