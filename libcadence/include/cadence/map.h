/*
 * includes/doste/map.h
 * 
 * Map an OID to an OID
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

#ifndef _doste_MAP_H_
#define _doste_MAP_H_

#include <cadence/vector.h>
#include <cadence/doste/oid.h>

namespace cadence {

	class MapPair {
		public:
		
		MapPair() {};
		MapPair(const doste::OID &k, const doste::OID &v) : key(k), value(v) {};
		
		doste::OID key;
		doste::OID value;
	};

	/**
	 * Maps and OID to another OID. Not a general template class.
	 * @author Nicolas Pope
	 */
	class XARAIMPORT Map {
		public:
		Map() {};
		~Map() {};

		static const unsigned int MAP_SIZE = 1000;
		
		doste::OID operator[](const doste::OID &o) {
			int hash = hashOID(o);
			
			for (int i=0; i<m_table[hash].size(); i++) {
				if (m_table[hash].get(i).key == o)
					return m_table[hash].get(i).value;
			}
			
			return doste::Null;
		};
		
		void add (const doste::OID &o, const doste::OID &v) {
			int hash = hashOID(o);
			m_table[hash].add(MapPair(o,v));
		};
		
		void remove(const doste::OID &o) {
			int hash = hashOID(o);
			for (int i=0; i<m_table[hash].size(); i++) {
				if (m_table[hash].get(i).key == o)
					m_table[hash].remove(i);
					return;
			}
		}

		void clear() {
			for (unsigned int i=0; i<MAP_SIZE; i++) m_table[i].clear();
		}

		class iterator {
			public:
			iterator(int position, const cadence::Vector<MapPair> *table, cadence::Vector<MapPair>::iterator it) : m_pos(position), m_it(it), m_table(table) {
			};
			~iterator() {};
			iterator operator++() {
				if (m_it != m_table[m_pos].end()) {
					m_it++;
				} else {
					m_pos++;
					m_it = m_table[m_pos].begin();
				}
				return *this;
			};
			iterator operator++(int) {
				if (m_it != m_table[m_pos].end()) {
					m_it++;
				} else {
					m_pos++;
					m_it = m_table[m_pos].begin();
				}
				return *this;
			};
			MapPair operator*() {
				return *m_it;
			};

			bool operator==(const iterator &i) {
				return (i.m_pos == m_pos) && (m_it == i.m_it);
			};

			bool operator!=(const iterator &i) {
				return (i.m_pos != m_pos) || (m_it != i.m_it);
			};

			private:
			int m_pos;
			cadence::Vector<MapPair>::iterator m_it;
			const cadence::Vector<MapPair> *m_table;
		};

		iterator begin() const { return iterator(0, m_table, m_table[0].begin()); };
		iterator end() const { return iterator(MAP_SIZE-1, m_table, m_table[MAP_SIZE-1].end()); };
		
		private:
		
		int hashOID(const doste::OID &o) {
			return o.d() % MAP_SIZE;
		};
		
		cadence::Vector< MapPair > m_table[MAP_SIZE];
	};
};

#endif
