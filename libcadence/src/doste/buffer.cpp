/*
 * src/dvm/buffer.cpp
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

#include <cadence/doste/buffer.h>
#include <cadence/memory.h>

using namespace cadence;
using namespace cadence::doste;

Buffer *Buffer::s_buffers[MAX_BUFFERS] = {0};

OID Buffer::create(char *buf, int count) {
	OID id = allocate();
	Buffer *bobj = lookup(id);
	if (bobj->m_dofree) delete [] bobj->m_data;
	bobj->m_data = buf;
	bobj->m_count = count;
	bobj->m_dofree = false;
	bobj->m_type = B_CHAR;
	return id;
}

OID Buffer::create(int *buf, int count) {
	OID id = allocate();
	Buffer *bobj = lookup(id);
	if (bobj->m_dofree) delete [] bobj->m_data;
	bobj->m_data = (char*)buf;
	bobj->m_count = count;
	bobj->m_dofree = false;
	bobj->m_type = B_INT;
	return id;
}

OID Buffer::create(long long *buf, int count) {
	OID id = allocate();
	Buffer *bobj = lookup(id);
	if (bobj->m_dofree) delete [] bobj->m_data;
	bobj->m_data = (char*)buf;
	bobj->m_count = count;
	bobj->m_dofree = false;
	bobj->m_type = B_LONGLONG;
	return id;
}

OID Buffer::create(float *buf, int count) {
	OID id = allocate();
	Buffer *bobj = lookup(id);
	if (bobj->m_dofree) delete [] bobj->m_data;
	bobj->m_data = (char*)buf;
	bobj->m_count = count;
	bobj->m_dofree = false;
	bobj->m_type = B_FLOAT;
	return id;
}

OID Buffer::create(double *buf, int count) {
	OID id = allocate();
	Buffer *bobj = lookup(id);
	if (bobj->m_dofree) delete [] bobj->m_data;
	bobj->m_data = (char*)buf;
	bobj->m_count = count;
	bobj->m_dofree = false;
	bobj->m_type = B_DOUBLE;
	return id;
}

OID Buffer::create(OID *buf, int count) {
	OID id = allocate();
	Buffer *bobj = lookup(id);
	if (bobj->m_dofree) delete [] bobj->m_data;
	bobj->m_data = (char*)buf;
	bobj->m_count = count;
	bobj->m_dofree = false;
	bobj->m_type = B_OID;
	return id;
}

OID Buffer::create(BufferType t, int count) {
	OID id = allocate();
	Buffer *bobj = lookup(id);
	if (bobj == 0) return Null;
	if (bobj->m_dofree) delete [] bobj->m_data;
	
	bobj->m_count = count;
	bobj->m_dofree = true;
	bobj->m_type = t;

	//std::cout << "Count: " << count << "\n";
	
	switch (t) {
	case B_CHAR:		bobj->m_data = NEW char[count]; break;
	case B_INT:		bobj->m_data = (char*)new int[count]; break;
	case B_LONGLONG:	bobj->m_data = (char*)new long long[count]; break;
	case B_FLOAT:		bobj->m_data = (char*)new float[count]; break;
	case B_DOUBLE:		bobj->m_data = (char*)new double[count]; break;
	case B_OID:		//bobj->m_data = (char*)(new OID[count]); break;
				bobj->m_data = NEW char[count*sizeof(OID)]; break;
	default: 		bobj->m_dofree = false; bobj->m_data = 0; bobj->m_count = 0;
	}

	//std::cout << "Alloc ptr: " << (void*)bobj->m_data << "\n";
	
	return id;
}

OID Buffer::allocate() {
	for (unsigned int i=0; i<MAX_BUFFERS; i++) {
		if (s_buffers[i] == 0) {
			s_buffers[i] = NEW Buffer();
			return OID(0,14,0,i);
		}
	}
	
	return Null;
}

void Buffer::free(const OID &id) {
	if (!id.isBuffer()) return;
	unsigned int idi = (unsigned int)id.d();
	if (idi >= 0 && idi < MAX_BUFFERS) {
		if (s_buffers[idi] != 0)
			delete s_buffers[idi];
		s_buffers[idi] = 0;
	}
}

Buffer *Buffer::lookup(const OID &id) {
	if (!id.isBuffer()) return 0;
	unsigned int idi = (unsigned int)id.d();
	if (idi >= 0 && idi < MAX_BUFFERS) {
		return s_buffers[idi];
	} else {
		return 0;
	}
}

void Buffer::finalise() {
	for (int i=0; i < MAX_BUFFERS; i++) {
		if (s_buffers[i] != 0) delete s_buffers[i];
	}
}
