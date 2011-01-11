#include "xnethandler.h"
#include "xnetconnection.h"
#include "xnetprotocol.h"
#include "xnetcache.h"
#include <cadence/doste/processor.h>
#include <cadence/doste/buffer.h>
#include <string.h>

using namespace cadence;
using namespace cadence::doste;

//Supports 4 billion network connection objects!
XNetHandler::XNetHandler(XNetConnection *conn, const OID &base)
 : Handler(base+OID(0,0,1,0), base+OID(0,0,17,0xFFFFFFFF)) {
	m_conn = conn;
	
	m_cache = new XNetCache(m_conn);
}

XNetHandler::~XNetHandler() {
	//Delete mapping and attributes
	delete m_cache;
}

bool XNetHandler::handle(Event &evt) {

	if (evt.type() == Event::GET) {
		evt.result(m_cache->get(evt.dest(), evt.param<0>()));
	//} else if (evt.type() == Event::GET_KEYS) {
	//	evt.result(m_cache->getkeys(evt.dest()));
	} else {
		m_conn->protocol()->begin();
		int id = m_conn->protocol()->event(evt, ((evt.type() == Event::GET) || (evt.type() == Event::GET_KEYS) || (evt.type() == Event::GET_RANGE)) ? XNetProtocol::EFLAG_WAITRESULT : 0);
		m_conn->protocol()->end();
		
		//Block for result if needed.
		if (id != 0xFF) {
			//std::cout << "WAIT\n";
			while (m_conn->protocol()->wait(id)) {
				//Processor::processInstant();
				XNetConnection::update();
			}
			//std::cout << "DONE\n";
			evt.result(m_conn->protocol()->getResult(id));
		}
	
	}

	return true;
}
