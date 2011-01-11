#ifndef _doste_VIRTUALOBJECT_H_
#define _doste_VIRTUALOBJECT_H_

#include <cadence/doste/oid.h>

namespace cadence {
	namespace doste {
		class Context;
	
		class VirtualObject {
			friend class Context;
	
			public:
			VirtualObject();
			~VirtualObject();
	
			void set(const OID &key, const OID &value);
			const OID &get(const OID &key);
			void define(const OID &key, const OID &def);
			void function(const OID &key, const OID &def);
	
			OID convert();
			OID getRealOID();
			OID doconvert() { m_convert = true; return getRealOID(); }
	
			private:
	
			struct Attribute {
				OID key;
				OID value;
				OID def;
				unsigned char flags;
				Attribute *next;
			};
	
			static const unsigned int VO_HASHSIZE = 20;
	
			Attribute *m_hash[VO_HASHSIZE];
			OID m_real;
			bool m_convert;
		};
	};
};

#endif
