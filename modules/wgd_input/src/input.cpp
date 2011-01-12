#include <wgd/wiimote.h>
#include <wgd/joystick.h>
#include <cadence/agent.h>

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

wgd::OID inputbase;

class InputAgent : cadence::Agent {
	public:
	InputAgent(const OID &obj) : Agent(obj) {
		registerEvents();
	}
	
	~InputAgent() {}
	
	BEGIN_EVENTS(Agent);
	EVENT(evt_wiimote_find, (*this)("wiimotes")("find"));
	EVENT(evt_update, (*this)("update"));
	END_EVENTS;
};

OnEvent(InputAgent, evt_wiimote_find) {
	if(get("wiimotes")["find"] != wgd::Null) {
		Wiimote::find();
	}
}

OnEvent(InputAgent, evt_update) {
	Wiimote::updateAll();
	Joystick::updateAll();
}

IMPLEMENT_EVENTS(InputAgent, Agent);

extern "C" INPUTIMPORT void initialise(const cadence::doste::OID &base) {
	inputbase = base;
	cadence::Object::registerType<wgd::Wiimote>();
	cadence::Object::registerType<wgd::Joystick>();
	Wiimote::initialise();
	Joystick::initialise();
	new InputAgent(base);
}

extern "C" INPUTIMPORT void finalise() {
	Wiimote::finalise();
	Joystick::finalise();
}
