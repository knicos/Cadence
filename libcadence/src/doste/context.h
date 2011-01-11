/*
 * src/dvm/context.h
 * 
 * Used within definitions to store contextual information.
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

#ifndef _doste_CONTEXT_H_
#define _doste_CONTEXT_H_

#include <cadence/vector.h>
#include "virtualobject.h"

namespace cadence {
	namespace doste {
		class Context {
			public:
			Context(Definition &def, const OID &obj, const OID &pkey)
			: m_key(pkey), m_object(obj), m_def(def), m_skey(pkey), m_sobject(obj) {};
			~Context();
	
			const OID &key() const { return m_key; }
			const OID &object() const { return m_object; }
			const Definition &definition() const { return m_def; }
			const OID &selfObject() const { return m_sobject; }
			const OID &selfKey() const { return m_skey; }
			void selfObject(const OID &sobj) { m_sobject = sobj; }
			void selfKey(const OID &skey) { m_skey = skey; }
	
			OID newVO() { m_vos.add(new VirtualObject()); return OID(0,100,0,m_vos.size()-1); }
			VirtualObject *getVO(const OID &vo) { return m_vos[vo.d()]; }
	
			private:
			void convertVOs();
			cadence::Vector<VirtualObject*> m_vos;
			OID m_key;
			OID m_object;
			Definition &m_def;
			OID m_skey;
			OID m_sobject;
		};
	};
};

#endif
