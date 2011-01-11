/*
 * includes/doste/messages.h
 * 
 * Wrappers to generate error,warning,debug and information message objects.
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

#ifndef _doste_MESSAGES_H_
#define _doste_MESSAGES_H_

#include <cadence/dstring.h>
#include <cadence/dll.h>

namespace cadence {
	/**
	 * Represents any error that occurs within DOSTE or in any loaded module. It will
	 * create a database object filled with the error information and assign that to
	 * root["error"]. By default DOSTE will trigger on this and print, in red, a message
	 * to standard error. An error should be generated if the system has failed in some way
	 * that would cause it to badly break if the error had not been detected. A fatal error
	 * is one that causes the system to halt, typically these have to use standard error
	 * directly.
	 */
	class XARAIMPORT Error {
		public:
		/**
		 * Create a new error object. Currently only a number and string message
		 * can be specified with the werror.
		 * @param id A unique error number or 0 if unknown.
		 * @param msg A dstring containing the error message.
		 */
		Error(int id, cadence::dstring msg);
		Error(int id, cadence::dstring msg, cadence::doste::OID obj, cadence::doste::OID key);
		~Error() {}

		static const int UNKNOWN = 0;		/**< An unknown error, yet to be given a number. */
		static const int SYNTAX = 1;		/**< A syntax error occured in a notation parser. */
		static const int MODULE_NOT_FOUND = 2;	/**< Unable to load a dynamic module, does not exist. */
		static const int INVALID_MODULE = 3;	/**< The specified module is not a valid DOSTE module. */
		static const int POOL = 4;		/**< Cannot use error objects for this!!!!! */
		static const int CYCLIC = 5;		/**< Cyclic Definition Detected. */
		static const int CUSTOM = 1000;		/**< Custom modules events should be >= this number. */
	};

	/**
	 * Represents any warning that occurs within DOSTE or in any loaded module. It will
	 * create a database object filled with the warning information and assign that to
	 * root["warning"]. By default DOSTE will trigger on this and print, in orange or yellow, a message
	 * to standard error. A warning should be generated if the system will not break but might not
	 * do as the user expects.
	 */
	class XARAIMPORT Warning {
		public:
		Warning(int id, cadence::dstring msg);
		~Warning() {}

		static const int UNKNOWN = 0;			/**< An unknown warning, yet to be given a number. */
		static const int SCRIPT_NOT_FOUND = 1;		/**< A script file could not be found. */
		static const int INVALID_DESTINATION = 2;	/**< Trying to send event to unknown OID. */
		static const int GLOBAL_HANDLERS = 3;		/**< Attempting to set multiple global handlers. */
		static const int HANDLER_EXCEED = 4;		/**< Maximum number of event handlers exceeded. */
		static const int CURRENT_QUEUE = 5;		/**< Sending event to current queue, a bad idea. */
		static const int EVENT_OVERFLOW = 6;		/**< Trying to delete more events than were allocated. */
		static const int UNPROCESSED_EVENT = 7;		/**< An event matched a handler but was not processed. */
		static const int SYNTAX = 8;
		static const int CUSTOM = 1000;			/**< Custom module warnings should be >= this number. */
	};

	class XARAIMPORT Debug {
		public:
		Debug(int id, cadence::dstring msg);
		~Debug() {}

		static const int UNKNOWN = 0;
		static const int CUSTOM = 1000;	//All custom user message number are above this.
	};

	class XARAIMPORT Info {
		public:
		Info(int id, cadence::dstring msg);
		~Info() {}

		static const int UNKNOWN = 0;
		static const int LOADING = 1;
		static const int CUSTOM = 1000;	//All custom user message number are above this.
	};
};

#endif
