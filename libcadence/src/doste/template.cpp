using namespace doste;
using namespace doste::dvm;

Templates::Templates()
 : Handler(OID(0,16,0,0), OID(0,16,0,0xFFFFFFFF)) {
}

Templates::~Templates() {
}

bool Templates::handle(Event &evt) {

	switch(evt.type()) {
		case Event::GET:
		case Event::SET:
		case Event::DEFINE:
		case Event::DEFINE_FUNC:
		case Event::GET_KEYS:
		default: return false;
	}
}