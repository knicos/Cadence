/*
 * includes/doste/dvm/buffer.h
 * 
 * Efficient temporary storage for events that accept or return bulk data.
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

#ifndef _DOSTE_DVM_BUFFER_H_
#define _DOSTE_DVM_BUFFER_H_

#include <cadence/doste/oid.h>
#include <cadence/dll.h>

namespace cadence {
	namespace doste {

		enum BufferType {B_INT=1, B_CHAR=2, B_OID=3, B_FLOAT=4, B_LONGLONG=5, B_DOUBLE=6};

		/**
		* A temporary object designed for storing integer indexed data. This is generated
		* by various bulk events and is the most efficient way of communicating bulk
		* data to the database and vice versa. Typically the sender will create and fill
		* a buffer object and the receiver will use and then delete it.
		*/
		class DVMIMPORT Buffer {
			Buffer() : m_count(0), m_data(0), m_dofree(false) {
				//std::cout << "MAKE BUFFER\n";
			};
	
			~Buffer() {
				//std::cout << "DESTROY BUF\n";
				if (m_dofree && m_data != 0) {
					delete [] m_data;
					m_data = 0;
					m_dofree = false;
				}
			};
	
			public:
			/**
			* Construct a buffer using an array as the data source. It will use the
			* given pointer without copying the data and so the pointer must remain
			* valid until the buffer has been used (the event has been processed).
			* @param buf The array to use as the source.
			* @param count Number of elements in the char array.
			* @return Buffer OID.
			*/
			static OID create(char *buf, int count);
			static OID create(int *buf, int count);
			static OID create(long long *buf, int count);
			static OID create(float *buf, int count);
			static OID create(double *buf, int count);
			static OID create(OID *buf, int count);
	
			/**
			* Create a buffer of the specified type. This allocates the required
			* memory which will be freed when the buffer is destroyed.
			* @param t The buffer data type.
			* @param count Number of elements in the array.
			* @return Buffer OID.
			*/
			static OID create(BufferType t, int count);
	
			/**
			* Allocate a buffer but do not set its data source or allocated any
			* memory for it yet.
			* @return Buffer OID.
			*/
			static OID create() { return allocate(); };		
	
			/**
			* Get the raw data pointer.
			*/
			void *raw() const { return m_data; };
	
			/**
			* Get the size in bytes of the data array. This is not the number of
			* elements in the array as they may be more than one byte each.
			* @see count()
			* @return Size of data array in bytes.
			*/
			int size() const {
				switch(m_type) {
				case B_INT:	return m_count*sizeof(int);
				case B_CHAR:	return m_count*sizeof(char);
				case B_OID:	return m_count*sizeof(OID);
				case B_FLOAT:	return m_count*sizeof(float);
				case B_LONGLONG:	return m_count*sizeof(long long);
				case B_DOUBLE:	return m_count*sizeof(double);
				default:	return 0;
				}
			};
	
			/**
			* Number of elements in the data array.
			* @return Number of elements.
			*/
			int count() const { return m_count; };
	
			/**
			* Query the type of the data stored in this buffer object.
			*/
			BufferType type() const { return m_type; };
			
			//Conversion methods.
			void get(int *buf, int count);
			void get(char *buf, int count);
			void get(OID *buf, int count);
			void get(float *buf, int count);
			OID get(int index) const { return ((OID*)m_data)[index]; };
			
			void set(int *buf, int count);
			void set(char *buf, int count);
			void set(OID *buf, int count);
			void set(float *buf, int count);
			void seti(int index, const OID &value) { ((OID*)m_data)[index] = value; };
			
			static OID allocate();
			static void destroy(const OID &id) { free(id); };
			static void free(const OID &id);
			static Buffer *lookup(const OID &id);
			
			static void finalise();
			
			private:
			int m_count;
			BufferType m_type;
			char *m_data;
			bool m_dofree;
			
			static const unsigned int MAX_BUFFERS = 50;
			static Buffer *s_buffers[MAX_BUFFERS];
		};
	};
};

#endif
