/*
 * src/library/file.cpp
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

#include <cadence/file.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> 

using namespace cadence;
using namespace cadence::doste;

char *LocalFile::s_path[20];

LocalFile::LocalFile() : File() {}

LocalFile::LocalFile(const char *pfilename) {
	filename(pfilename);
}

LocalFile::LocalFile(const OID &obj) : File(obj) {
	m_size = 0;
}

LocalFile::~LocalFile() {
}

void LocalFile::initialise() {
	char *paths = getenv("CADENCE_PATH");
	char *temp;
	
	//Now need to split paths into an array of individual paths.
	for (int i=0; i<19; i++) {
		s_path[i] = new char[500];
		temp = strchr(paths, ':');
		if (temp != 0) {
			strncpy(s_path[i], paths, temp-paths);
			paths = temp+1;
		} else {
			strcpy(s_path[i], paths);
			s_path[i+1] = 0;
			break;
		}
	}
}

void LocalFile::getLocalFilename(char *buf, int max) {
	struct stat fileinfo;
	char *buf2 = new char[1000];
	//dstring b = base();
	dstring f = filename();
	
	f.toString(buf2, 1000);

	//Should check for max...
	//b.toString(buf, b.size()+1);
	//f.toString(&buf[b.size()], f.size()+1);

	//Compare with all paths
	int i=0;
	while (s_path[i] != 0) {
		strcpy(buf, s_path[i]);
		strcat(buf, "/");
		strcat(buf, buf2);
		if (stat(buf, &fileinfo) == 0) {
			delete [] buf2;
			return;
		}
		i++;
	}
	strcpy(buf,buf2);
	delete [] buf2;
}

bool LocalFile::open(Mode m) {
	mode(m);
	m_cache = 0;
	m_pos = 0;

	/*(dstring b = base();
	dstring f = filename();
	char *buf = new char[b.size()+f.size()+2];
	b.toString(buf, b.size()+1);
	f.toString(&buf[b.size()], f.size()+1);*/
	char *buf = new char[3000]; //This is dangerous?
	getLocalFilename(buf, 3000);
	
	
	//std::cout << "LocalFile: " << buf << "\n";

	m_stream.open(buf, std::fstream::in | std::fstream::binary);
	delete [] buf;
	m_stream.seekg(0, std::fstream::end);
	m_size = m_stream.tellg();
	m_stream.seekg(0, std::fstream::beg);
	return m_stream.is_open();
}

void LocalFile::close() {
	if (m_cache != 0) delete [] m_cache;
	m_cache = 0;
	m_stream.close();
}

int LocalFile::read(char *buf, int count) {
	if (m_size == 0 || !m_stream.is_open()) return 0;

	if (m_cache == 0) {
		m_cache = new char[m_size+1];
		m_stream.read(m_cache,m_size);
	}

	if (eof()) {
		buf[0] = 0;
		return 0;
	}

	if (count > m_size-m_pos) {
		count = m_size-m_pos;
	}

	int start = m_pos; //m_stream.tellg();
	memcpy(buf, &m_cache[m_pos], count); //m_stream.read(buf,count);
	m_pos += count;
	//if (eof()) {
	//	buf[((int)m_stream.tellg()) - start] = 0;
	//}
	return m_pos - start;
}

void LocalFile::seek(int pos, Seek d=CUR) {
	if (d == CUR) {
		m_pos += pos;
	} else if (d == BEG) {
		m_pos = pos;
	} else {
		m_pos = m_size - pos;
	}
	/*if (d == CUR) {
		m_stream.seekg(pos, std::fstream::cur);
	} else if (d == BEG) {
		m_stream.seekg(pos, std::fstream::beg);
	} else {
		m_stream.seekg(pos, std::fstream::end);
	}*/
}

char LocalFile::peek(int n) {
	if (m_size == 0 || !m_stream.is_open()) return 0;

	if (m_cache == 0) {
		m_cache = new char[m_size];
		m_stream.read(m_cache,m_size);
	}

	if (m_pos+n >= m_size) return 0;
	return m_cache[m_pos+n];

	/*if (n == 0) {
		char p = m_stream.peek();
		if (eof()) return 0;
		return p;
	} else {
		int org = tell();
		seek(n, CUR);
		char p = m_stream.peek();
		if (eof()) return 0;
		seek(org, BEG);
		return p;
	}*/
}

bool LocalFile::eof() {
	//return m_stream.eof();
	//return (tell() >= size());
	return (m_pos >= m_size);
}

int LocalFile::tell() {
	//return m_stream.tellg();
	return m_pos;
}

int LocalFile::size() {
	return m_size;
}
