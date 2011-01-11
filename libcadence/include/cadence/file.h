/*
 * includes/doste/file.h
 * 
 * File wrapper class to provide file system independence.
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

#ifndef _doste_FILE_H_
#define _doste_FILE_H_

#include <cadence/object.h>
#include <cadence/dstring.h>
#include <cadence/stream.h>
#include <fstream>
#include <cadence/dll.h>

#ifdef _MSC_VER
#pragma warning ( disable : 4251 )
#endif

namespace cadence {
	class XARAIMPORT File : public cadence::Object, public cadence::Stream {
		public:
		VOBJECT(Object, File);

		enum Mode {READ, WRITE, READWRITE};

		File() {};
		File(const OID &obj) : Object(obj) {};
		virtual ~File() {};

		/** Get the filename. */
		PROPERTY_R(dstring, filename);
		/** Change the filename */
		PROPERTY_W(dstring, filename);

		/** Base is prepended to the filename */
		PROPERTY_R(dstring, base);
		/** Base is prepended to the filename */
		PROPERTY_W(dstring, base);

		/**
		 * Get a full path to a local cached copy of the file. This is
		 * needed for DLL modules which cannot load using this file
		 * class but instead directly from the local file system.
		 * @param buf A char array to fill with the filename.
		 * @param max The size of the char array.
		 */
		virtual void getLocalFilename(char *buf, int max)=0;

		void mode(Mode m) { m_mode = m; }
		Mode mode() const { return m_mode; }

		/**
		 * Open this file in the specified mode.
		 * @param m Mode is READ, WRITE or READWRITE.
		 * @return True if the file has been opened.
		 */	
		virtual bool open(Mode m)=0;
		/** Close this file if open. */
		virtual void close()=0;

		private:
		Mode m_mode;
	};

	/**
	 * Use this implementation of File to read files stored on the local
	 * file system.
	 */
	class XARAIMPORT LocalFile : public File {
		public:
		OBJECT(File, LocalFile);

                LocalFile();
		/** Constructor. Specify the filename */
		LocalFile(const char *filename);
		LocalFile(const OID &obj);
		~LocalFile();

		void getLocalFilename(char *buf, int max);

		bool open(Mode m);
		void close();
		int read(char *buf, int count);
		//int write(const char *buf, int count);
		void seek(int pos, Seek d);
		char peek(int n=0);
		bool eof();
		int size();
		int tell();
		
		static void initialise();
		static char *s_path[20];

		private:
		std::fstream m_stream;
		int m_size;
		char *m_cache;
		int m_pos;
		
	};
};

#endif
