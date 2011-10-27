/*
 * includes/doste/dvm/definition.h
 * 
 * C++ Wrapper for DOSTE definitions.
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

#ifndef _doste_DVM_DEFINITION_H_
#define _doste_DVM_DEFINITION_H_

#include <cadence/doste/oid.h>
#include <cadence/dll.h>
#include <cadence/doste/buffer.h>

namespace cadence {
	namespace doste {

		class Context;

		/**
		* Manage and evaluate definitions. This is the heart of the virtual machine as it
		* is responsible to definition evaluation. It also works closely with OID to create
		* a simple way of describing definitions in C++. An example of a C++ definition is
		* shown below:<br><br>
		* <code>
		* root["test"] = root("devices")("ps2keyboard")("key");<br>
		* </code>
		*
		* @author Nicolas Pope
		* @see doste::OID
		* @see doste::Modifiers
		*/
		class DVMIMPORT Definition {
			public:
			/** Constructor to make a new definition. */
			Definition() { m_def = cadence::doste::OID::create(); m_size = 0; };
			/**
			* Constructor to use an existing definition.
			* @param o The object representing the definition.
			*/
			Definition(const cadence::doste::OID &o) : m_def(o) { m_size = (OID)m_def[Size]; };
			
			/**
			 * Constructor for compiled definitions.
			 */
			Definition(const cadence::doste::OID &o, void *c) : m_def(o), m_comp(c) { };
			
			/** Destructor. */
			~Definition() {};
	
			/**
			* Used to help construct definitions in C++. Allows you to apply together
			* OIDs to build up a definition. See example in main class description.
			* @param o The key or first object to add to the definition.
			* @return This.
			*/
			Definition operator()(const cadence::doste::OID &o);
	
			/**
			* Cast to OID to get definition object. This does not evaluate the
			* definition.
			*/
			operator cadence::doste::OID() const { return m_def; };
			
			/**
			* Evaluate the definition and return the result. It takes several parameters
			* to control how the definition should be evaluated or what it should do with
			* side-effects and dependencies.
			* @param obj Object to use as 'this' in the definition.
			* @param key Object to use as '$' in the definition.
			* @param fdef Is this a function definition. If it is then add dependencies to host definition.
			* @param agent Is this an agent. If yes then use the agent run queue for all side-effects.
			* @return The result object for this definition.
			*/
			cadence::doste::OID evaluate(const cadence::doste::OID &obj, const cadence::doste::OID &key, bool fdef=false);
			
			void toString(char *buf, int max) const;
	
			struct DContext {
				OID object;
				OID key;
			};
			
			private:
			cadence::doste::OID m_def;
			int m_size;
			void *m_comp;
			
			void buildif(char *buf, OID ifobj, int indent) const;
			void toString(char *buf, int max, int indent) const;

			cadence::doste::OID parseExpression(cadence::doste::Context *ctx, cadence::doste::Buffer *def, int &index, bool fdef);
		};
	};
};

#endif
