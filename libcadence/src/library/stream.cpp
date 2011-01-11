/*
 * src/library/stream.cpp
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

#include <cadence/stream.h>
#include <string.h>
#include <iostream>

using namespace cadence;
//using namespace doste::dvm;

StringStream::StringStream(const char *s)
  : m_s(s), m_pos(0) {
	m_size = strlen(s);
}

int StringStream::read(char *buf, int count) {

	if (eof()) {
		buf[0] = 0;
		return 0;
	}

	if (m_pos+count >= m_size) count = m_size-m_pos;
	memcpy(buf, &m_s[m_pos], count);
	m_pos += count;
	return count;
}

void StringStream::seek(int pos, Seek d) {
	if (d == CUR) m_pos += pos;
	else if (d == BEG) m_pos = pos;
	else m_pos = m_size - pos;

	if (m_pos < 0) m_pos = 0;
	if (m_pos > m_size) m_pos = m_size;
}

char StringStream::peek(int n) {
	if (eof()) return 0;
	return m_s[m_pos+n];
}

int StringStream::tell() {
	return m_pos;
}

bool StringStream::eof() {
	return (m_pos >= m_size);
}

int StringStream::size() {
	return m_size;
} 
