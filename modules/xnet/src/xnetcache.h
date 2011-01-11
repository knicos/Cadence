#ifndef _XNETCACHE_
#define _XNETCACHE_

#include <cadence/doste/handler.h>
#include <cadence/doste/oid.h>

class XNetConnection;

struct CacheEntry {
	cadence::doste::OID n;
	cadence::doste::OID e;
	cadence::doste::OID value;
	bool out_of_date;
	CacheEntry *next;
};

class XNetCache : public cadence::doste::Handler {
	public:
		
	XNetCache(XNetConnection *conn);
	~XNetCache();
	
	bool handle(cadence::doste::Event &evt);
	
	const cadence::doste::OID &get(const cadence::doste::OID &n, const cadence::doste::OID &e);
	//const cadence::doste::OID &getkeys(const cadence::doste::OID &n);
	
	private:

	static const unsigned int CACHE_SIZE = 10000;
	static const unsigned int KEY_CACHE_SIZE = 1000;
	CacheEntry *m_cache[CACHE_SIZE];
	//KeyCacheEntry *m_keycache[KEY_CACHE_SIZE];
	XNetConnection *m_conn;
	
	static int hashOIDS(const cadence::doste::OID &o, const cadence::doste::OID &k) {
		return (o.d()+k.d()) % CACHE_SIZE;
	};
	
	static int hashOID(const cadence::doste::OID &o) {
		return o.d() % KEY_CACHE_SIZE;
	};
	
	CacheEntry *lookup(const cadence::doste::OID &n, const cadence::doste::OID &e);
	//KeyCacheEntry *lookup(const cadence::doste::OID &n);
	int lookupCount(CacheEntry *e, int hash);
};

#endif
