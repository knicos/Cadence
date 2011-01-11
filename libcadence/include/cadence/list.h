/*
 * includes/doste/list.h
 * 
 * A list template that corresponds to but is not the same as the C++ stl list.
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 18/7/2007
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

#ifndef _doste_LIST_H_
#define _doste_LIST_H_

#ifdef _MSC_VER
#pragma warning ( disable : 4251 )
#endif

namespace cadence {

	/**
	 * A general purpose doubly-linked list template for use in DOSTE. It has some
	 * similarities to the STL list but is not the same.
	 */
	template <typename T>
	class List {
		class ListEntry {
			public:
			T value;
			ListEntry *prev;
			ListEntry *next;
		};

		public:

		class iterator {
			public:
			iterator(ListEntry *position) : m_pos(position) {};
			~iterator() {};
			iterator operator++() {
				ListEntry *temp = m_pos;
				if (m_pos != 0)
					m_pos = m_pos->next;
				return iterator(temp);
			};
			iterator operator++(int) {
				if (m_pos != 0)
					m_pos = m_pos->next;
				return iterator(m_pos);
			};
			T operator*() {
				return (m_pos != 0) ? m_pos->value : T();
			};

			bool operator==(const iterator &i) {
				return i.m_pos == m_pos;
			};

			bool operator!=(const iterator &i) {
				return i.m_pos != m_pos;
			};

			private:
			ListEntry *m_pos;
		};

		List()
		 :	m_first(0),
			m_last(0),
			m_size(0) {
		};
		~List() {
			clear();
		};

		/**
		 * Put an item at the front of the list.
		 * @param value The value to put here.
		 */
		void addFront(T value) {
			ListEntry *entry = new ListEntry();
			entry->value = value;
			entry->next = m_first;
			entry->prev = 0;
			if (m_first != 0) {
				m_first->prev = entry;
				m_first = entry;
			} else {
				m_last = entry;
				m_first = entry;
			}
		
			++m_size;
		};

		/**
		 * Put an item at the back of the list.
		 * @param value The value to put there.
		 */
		void addBack(T value) {
			ListEntry *entry = new ListEntry();
			entry->value = value;
			entry->next = 0;
			entry->prev = m_last;
			if (m_last != 0) {
				m_last->next = entry;
				m_last = entry;
			} else {
				m_last = entry;
				m_first = entry;
			}
		
			++m_size;
		};
		/**
		 * @return The first item in the list.
		 */
		T getFront() const { return m_first; };
		/**
		 * @return The last item in the list.
		 */
		T getBack() const { return m_last; };
		/**
		 * This will remove the first item and return it. The following item
		 * will then become the first item.
		 * @return Original first item.
		 */
		T removeFront() {
			if (m_first != 0) {
				T temp = m_first->value;
				m_first = m_first->next;
				if (m_first != 0) {
					m_first->prev = 0;	
				} else {
					m_last = 0;
				}
		
				--m_size;
				return temp;
			}
		
			//This will not always work.
			return (T)0;
		};

		/**
		 * This will remove the last item and return it. The previous item will then
		 * become the last item.
		 * @return Original last item.
		 */
		T removeBack() {
			if (m_last != 0) {
				T temp = m_last->value;
				m_last = m_last->prev;
				if (m_last != 0) {
					m_last->next = 0;	
				} else {
					m_first = 0;
				}
		
				--m_size;
				return temp;
			}
		
			//This will not always work.
			return (T)0;
		};

		/**
		 * Retrieve the item at a specific index. Note that for lists this is slow
		 * as it counts from the front.
		 * @param index The index from 0 to size()-1.
		 * @return The value at that index or corresponding null.
		 */
		T get(int index) const {
			int i=0;
			ListEntry *current = m_first;
		
			while (i < index && current != 0) {
				current = current->next;
				++i;
			}
		
			if (current != 0)
				return current->value;
			else
				return (T)0;
		};

		/**
		 * Makes sure that there is only one of this value in the list.
		 * @param value The value to make unique in this list.
		 */
		void unique(T value) {
			int count = 0;
			ListEntry *current = m_first;
			ListEntry *temp;
		
			while (current != 0) {
				if (current->value == value) {
					++count;
		
					if (count > 1) {
						if (current->next != 0) {
							current->next->prev = current->prev;
						} else {
							m_last = current->prev;
						}
						if (current->prev != 0) {
							current->prev->next = current->next;
						} else {
							m_first = current->next;
						}
						temp = current;
						current = current->next;
						delete temp;
						continue;
					}
				}

				current = current->next;
			}
		};

		/**
		 * Removes all items in the list so that size becomes 0.
		 */
		void clear() {
			ListEntry *current = m_first;
			ListEntry *temp;
		
			while (current != 0) {
				temp = current;
				current = current->next;
				delete temp;
			}

			m_size = 0;
			m_first = 0;
			m_last = 0;
		};

		/**
		 * @return Number of items in the list.
		 */
		int size() const { return m_size; };
		/**
		 * Put an item at a specific location, this causes all items above to be
		 * shifted by one index.
		 * @param index Position to index, starts at 0 for front.
		 * @param value Item to put at that position.
		 */
		void insert(int index, T value) {
			int i=0;
			ListEntry *current = m_first;
		
			while (i < index && current != 0) {
				current = current->next;
				++i;
			}
		
			if (current == 0)
				return;
			
			//TODO
		};

		//void remove(T index);
		iterator erase(iterator pos);

		void removeAll(T value) {
			ListEntry *current = m_first;
			ListEntry *temp;
		
			while (current != 0) {
				if (current->value == value) {
					if (current->next != 0) {
						current->next->prev = current->prev;
					} else {
						m_last = current->prev;
					}
					if (current->prev != 0) {
						current->prev->next = current->next;
					} else {
						m_first = current->next;
					}
					temp = current;
					current = current->next;
					delete temp;
				} else {
					current = current->next;
				}
			}
		};

		int find(T) const;
		bool contains(T) const;
		void replace(int index, T);

		iterator begin() const { return iterator(m_first); };
		iterator end() const { return iterator(0); };

		private:
		ListEntry *m_first;
		ListEntry *m_last;
		int m_size;
	};
};

#endif
