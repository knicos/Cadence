#include "xnetcache.h"
#include "xnetconnection.h"
#include "xnetprotocol.h"

using namespace cadence;
using namespace cadence::doste;

XNetCache::XNetCache(XNetConnection *conn)
 : Handler(OID::local()+OID(0,0,16,0), OID::local()+OID(0,0,17,0xFFFFFFF)) {
	for (int i=0; i<CACHE_SIZE; i++)
		m_cache[i] = 0;
	
	m_conn = conn;
}

XNetCache::~XNetCache() {
	
}

CacheEntry *XNetCache::lookup(const OID &n, const OID &e) {
	int hash = hashOIDS(n,e);
	
	CacheEntry *res = m_cache[hash];
	while (res != 0 && ((res->n != n) || (res->e != e)))
		res = res->next;
	
	if (res == 0) {
		res = new CacheEntry;
		res->n = n;
		res->e = e;
		res->out_of_date = true;
		res->next = m_cache[hash];
		m_cache[hash] = res;
	}
	
	return res;
}

int XNetCache::lookupCount(CacheEntry *e, int hash) {
	CacheEntry *r = m_cache[hash];
	int count = 0;
	
	while ((r != 0) && (r != e)) {
		count++;
		r = r->next;
	}
	
	return count;
}

const OID &XNetCache::get(const OID &n, const OID &e) {
	CacheEntry *ent = lookup(n,e);
	
	if (ent->out_of_date) {

		Event *evt = new Event(Event::GET, n);
		evt->param<0>(e);
		
		//Also need to add dependency
		int hash = hashOIDS(n,e);
		int count = lookupCount(ent,hash);
		Event *evt2 = new Event(Event::ADDDEP, n);
		evt2->param<0>(e);
		evt2->param<1>(OID::local()+OID(0,0,16,(hash << 8) + count));
		
		m_conn->protocol()->begin();
		m_conn->protocol()->event(*evt2, 0);
		int id = m_conn->protocol()->event(*evt, XNetProtocol::EFLAG_WAITRESULT);
		m_conn->protocol()->end();
		
		delete evt;
		delete evt2;
		
		while (m_conn->protocol()->wait(id)) {
			XNetConnection::update();
		}

		ent->value = m_conn->protocol()->getResult(id);
		ent->out_of_date = false;
	} else {
		//std::cout << "USING CACHE\n";
	}
	
	return ent->value;
}

bool XNetCache::handle(Event &evt) {
	int count = evt.dest().d() & 0xFF;
	int hash = evt.dest().d() >> 8;

	CacheEntry *ent = m_cache[hash];
	while ((ent != 0) && (count > 0)) {
		ent = ent->next;
		count --;
	}
	
	ent->out_of_date = true;
	/*ent->value = evt.param<3>();
		//int hash = hashOIDS(n,e);
		//int count = lookupCount(ent,hash);
		Event *evt2 = new Event(Event::ADDDEP, ent->n);
		evt2->param<0>(ent->e);
		evt2->param<1>(OID::local()+OID(0,0,16,(hash << 8) + count));
		
		m_conn->protocol()->begin();
		m_conn->protocol()->event(*evt2, 0);
		m_conn->protocol()->end();
		
		delete evt2;*/
	
	return true;
}
