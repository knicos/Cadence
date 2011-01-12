#include <wgd/window.h>
#include <wgd/extensions.h>
#include <cadence/cadence.h>
#include <cadence/object.h>
#include <wgd/input.h>
#include <wgd/mouse.h>
#include <wgd/common.h>

#include <wgd/vertex.h>

using namespace cadence;
using namespace cadence::doste;

void createIndices();

namespace wgd {
	BASEIMPORT GameWindow*  window;
	BASEIMPORT Keyboard*  keyboard;
	BASEIMPORT Mouse*  mouse;
	BASEIMPORT wgd::OID WGD;
};

//somewhere to define this...
char* wgd::vertex_tangent::name = "tangent";

extern "C" void BASEIMPORT initialise(const OID &base) {
	createIndices();
	
	wgd::WGD = base;
	
	Object::registerType<wgd::GameWindow>();
	Object::registerType<wgd::Keyboard>();
	Object::registerType<wgd::Mouse>();
	Object::registerType<wgd::Input>();
	
	// Suggestion: set these in the GameWindow/Keyboard/Mouse constructor.
	// Rationale: so that their location in the database isn't hardcoded
	
	//if (base["window"] == cadence::Null) base["window"] = cadence::OID::create();
	wgd::window = ((OID)base["window"]);
	wgd::keyboard = ((OID)base["input"]["keyboard"]);
	wgd::mouse = ((OID)base["input"]["mouse"]);

	wgd::Extensions::initialise();
}

extern "C" void BASEIMPORT cadence_update() {
	wgd::Input::update();
	wgd::window->draw();
}

extern "C" void BASEIMPORT finalise() {
	wgd::Extensions::finalise();
}
