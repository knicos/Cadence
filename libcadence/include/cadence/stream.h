/*
 * includes/doste/stream.h
 * 
 * Base class for any kind of stream. Used for parsing and file loading.
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


#ifndef _doste_STREAM_H_
#define _doste_STREAM_H_

#include <cadence/dll.h>

namespace cadence {
	/**
	 * Generic stream class. This is similar to iostream in the C++
	 * library and is used to remove the difference from a string,
	 * a local file and remote files etc. It also allows the token
	 * parsers to work with any data source.
	 * @author Nicolas Pope
	 * @see File
	 * @see LocalFile
	 * @see StringStream
	 * @see token
	 */
	class XARAIMPORT Stream {
		public:
		virtual ~Stream() {}

		enum Seek {BEG, CUR, END};

		/**
		 * Read a number of bytes from this stream into a buffer.
		 * @param buf An array to read the bytes into.
		 * @param count Number of bytes to read.
		 * @return Number of bytes actually read.
		 */
		virtual int read(char *buf, int count) { return 0; };
		/**
		 * Write a number of bytes into this stream from a buffer.
		 * @param buf The source array.
		 * @param count Number of bytes to write.
		 * @return Number of bytes actually written.
		 */
		virtual int write(const char *buf, int count) { return 0; };
		/**
		 * Move to a new position within the stream.
		 * @param pos New position.
		 * @param d If BEG then position if from beginning of stream. If CUR then position is added to current location. If END then position is from the end of the stream.
		 */
		virtual void seek(int pos, Seek d=CUR)=0;
		/**
		 * Look at the next character but do not change position in the stream.
		 * @param n Look at the nth character from the current position.
		 * @return The character or 0.
		 */
		virtual char peek(int n=0)=0;
		/** @return The current stream position. */
		virtual int tell()=0;
		/** @return True if the end of the stream has been reached. */
		virtual bool eof()=0;
		/** @return The total size of the stream if known, otherwise -1. */
		virtual int size()=0;

		//Put stream operators here.
		
		/**
		 * Read a single element of type T.<br>
		 * e.g. <code> char a = file->read(); </code>
		 * @return The value read.
		 */
		template <typename T>
		T read() {
			T buf;
			read((char*)&buf, sizeof(T));
			return buf;
		}

		/**
		 * Read a single element of type T into the specified buffer.<br>
		 * e.g. <code> char a;<br>file->read(a);</code>
		 * @param buf An object to read into.
		 */
		template <typename T>
		void read(T &buf) {
			read((char*)&buf, sizeof(T));
		}

		/**
		 * Parse a specified token from the stream. See the token namespace for
		 * available token types. This can be used for parsing text files and is
		 * used for the DASM notation.<br>
		 * If you need to key a resulting value (e.g. when parsing an integer) then
		 * use parse value instead. If the token fails to parse it does not modify the
		 * position of the stream.
		 * @param t The token object, passed by value.
		 * @return True if this token was parsed.
		 */
		template <typename T>
		bool parse(T t) { return t.parse(*this); }
		
		/**
		 * Similar to parse() except that the token is passed by reference. This
		 * enables you to get values back from the token if it was parsed.
		 * @param t The token to parse
		 * @return True if the token was parsed.
		 */
		template <typename T>
		bool parseValue(T &t) { return t.parse(*this); }
	};

	/**
	 * Implements the Stream class so that character arrays can be used
	 * as a data source instead of a file.
	 * @author Nicolas Pope
	 */
	class XARAIMPORT StringStream : public Stream {
		public:
		StringStream() : m_s(0), m_pos(0) {}
		StringStream(const char *s);
		~StringStream() {};

		int read(char *buf, int count);
		void seek(int pos, Seek d);
		char peek(int n);
		int tell();
		bool eof();
		int size();
		const char *data() const { return m_s; }

		private:
		const char *m_s;
		int m_pos;
		int m_size;
	};
};

#endif
