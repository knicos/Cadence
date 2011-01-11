/*
 * includes/doste/vector.h
 * 
 * A generic dynamic array.
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


#ifndef _doste_VECTOR_H_
#define _doste_VECTOR_H_

#include <iostream>
#include <cadence/dll.h>

namespace cadence {

	/**
	 * Implements a classic vector data structure. It is a dynamically resizeable array.
	 * @author Nicolas Pope
	 */
	template <typename T>
	class XARAIMPORT Vector {
		public:
		/**
		 * Constructor.
		 * @param cap Initial capacity, defaults to 120
		 * @param inc Increment amount when space runs out.
		 */
		Vector(int cap=120, int inc=30)
		 : m_cap(cap), m_size(0), m_inc(inc) {
			m_array = new T[m_cap];
		};

		/** Destructor */
		~Vector() {
			delete [] m_array;
		};

		/**
		 * Clear and delete the array. Remember that it does not delete the elements if
		 * they are points, you must do this first.
		 */
		void clear() {
			delete [] m_array;
			//Might want to reset cap.
			m_size = 0;
			m_array = new T[m_cap];
		}

		/**
		 * Add a single element to the end of the array. It will automatically resize
		 * the array if needed.
		 * @param element The element to add.
		 */
		void add(T element) {
			if (m_size < m_cap) {
				m_array[m_size++] = element;
			} else {
				resize(increment());
				m_array[m_size++] = element;
			}
		};

		/**
		 * Remove a specific element and return it. This will move all other elements
		 * above it down by one but will not shrink the array.
		 * @param index Index, starting from 0, of the element to remove.
		 * @return The element at this index before removing.
		 */
		T remove(int index) {
			if (index < 0 || index >= m_size) return T();
			//Move everything above this point down one
			for (int i=index; i<m_size-1; i++) {
				m_array[i] = m_array[i+1];
			}
			
			m_size--;
			return m_array[m_size];
		};
		
		/**
		 * Remove an element from the end of the array.
		 * @return The last element.
		 */
		T remove() {
			if (m_size == 0) return T();
			m_size--;
			return m_array[m_size];
		}

		/** @return Number of elements in the array. */
		int size() const { return m_size; };
		/** @return Actual size of the array. */
		int capacity() const { return m_cap; };
		/** @return Increment value */
		int increment() const { return m_inc; };

		/**
		 * Increase the array by a specified amount.
		 * @param inc Amount to add to the array.
		 */
		void resize(int inc) {
			T *tarray = new T[m_cap+inc];
			for (int i=0; i<m_cap; i++)
				tarray[i] = m_array[i];
			delete [] m_array;
			m_cap += inc;
			m_array = tarray;
		}

		/**
		 * Query if an element is in the array. This performs a linear search.
		 * @param value The value to look for.
		 * @return True if found, otherwise false.
		 */
		bool contains(T value) const {
			for (int i=0; i<m_size; i++) {
				if (m_array[i] == value) return true;
			}
			return false;
		}

		/**
		 * Get an element at a specified index.
		 * @param index The index to get the element from
		 * @return The element at that index or some default constructed element.
		 */
		T get(int index) {
			return (index >= 0 && index < m_size) ? m_array[index] : T();
		};
		/** Same as get() */
		T operator[](int index) {
			return (index >= 0 && index < m_size) ? m_array[index] : T();
		};
		
		/**
		 * Change the value at a particular index. If the index is beyond the end of
		 * the current capacity then the capacity will be increased.
		 * @param index The index.
		 * @param value The element to add to this index.
		 */
		void set(int index, T value) {
			if (index < 0) return;
			if (index >= m_size) {
				m_size = index+1;
				if (m_cap < m_size) resize((m_size-m_cap) + m_inc);
			}
			m_array[index] = value;
		};

		/** @return Pointer to internal array. */
		T *data() { return m_array; };

		/**
		 * C++ style iterator.
		 */
		class iterator {
			public:
			iterator(int position, T *array) : m_pos(position), m_array(array) {};
			~iterator() {};
			iterator operator++() {
				m_pos++;
				return *this;
			};
			iterator operator++(int) {
				++m_pos;
				return *this;
			};
			T operator*() {
				return m_array[m_pos];
			};

			bool operator==(const iterator &i) {
				return i.m_pos == m_pos;
			};

			bool operator!=(const iterator &i) {
				return i.m_pos != m_pos;
			};

			private:
			int m_pos;
			T *m_array;
		};

		/** @return A new iterator starting at the beginning of the vector. */
		iterator begin() const { return iterator(0, m_array); };
		/** @return An iterator corresponding to the end of the vector. */
		iterator end() const { return iterator(m_size, m_array); };

		private:
		T *m_array;
		int m_cap;
		int m_size;
		int m_inc;
	};
};

#endif
