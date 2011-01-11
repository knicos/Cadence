#include "xnethandler.h"
#include "xnetconnection.h"
#include "xnet.h"
#include <cadence/doste/doste.h>

using namespace cadence;
using namespace cadence::doste;

OID xnetbase;

class XNetAgent : public Agent {
	public:
	XNetAgent(const OID &obj) : Agent(obj) {
		registerEvents();
	}

	~XNetAgent() {}

	BEGIN_EVENTS(Agent);
	EVENT(evt_listen, (*this)("listen"));
	EVENT(evt_connection, (*this)("connections")(All));
	//EVENT(evt_update, root("time"));
	END_EVENTS;
};

OnEvent(XNetAgent, evt_listen) {
	if ((bool)get("listen") == true) {
		XNetConnection::listen(*this);
	} else {
		//If already listening.
		//XNetConnection::stop();
	}
}

OnEvent(XNetAgent, evt_connection) {
	OID conns = get("connections");
	for (OID::iterator i=conns.begin(); i!=conns.end(); i++) {
		(XNetConnection*)conns.get(*i);
	}
}

//OnEvent(XNetAgent, evt_update) {
//	XNetConnection::update();
//}

IMPLEMENT_EVENTS(XNetAgent, Agent);

XNetAgent *myagent;

extern "C" void XNETIMPORT initialise(const OID &base) {
	xnetbase = base;
	Object::registerType<XNetConnection>();
	myagent = new XNetAgent(base);
}

extern "C" void XNETIMPORT update() {
	XNetConnection::update();
}

extern "C" void XNETIMPORT finalise() {
	//delete myagent;
}
