#include <cadence/agent.h>
#include <cadence/doste/doste.h>
#include "swindow.h"


using namespace cadence;
using namespace cadence::doste;

#define MAX_WINDOWS 10

Display *dis;
Shape_Window *windows[MAX_WINDOWS] = {0,};

class SAgent : public Agent {
		public:
		SAgent(const OID &obj) : Agent(obj) { registerEvents(); };
		~SAgent() {};
		
		BEGIN_EVENTS(Agent);
		EVENT(evt_windows, (*this)(cadence::doste::All));
		END_EVENTS;
	};

OnEvent(SAgent, evt_windows) {
	//Make all windows
	int j = 0;
	OID ag = (*this);
	for (OID::iterator i=ag.begin(); i!=ag.end(); i++) {
		if (*i != This)
		windows[j++] = (Shape_Window*)(ag.get(*i));
	}
}

IMPLEMENT_EVENTS(SAgent, Agent);

extern "C" void initialise(const cadence::doste::OID &base) {    
	dis = XOpenDisplay(NULL);
	Object::registerType<Shape_Window>();
	
	new SAgent(base);
}

extern "C" void cadence_update() {
	//#ifdef LINUX
	XEvent event;

	//if (Processor::numProcessors() > 1) XLockDisplay(wgd::window->getXDisplay());

	while (XPending(dis)) {
		XNextEvent(dis, &event);
		
		for (int i=0; i<MAX_WINDOWS; i++) {
			if ((windows[i] != 0) && (windows[i]->winid() == event.xany.window)) {
				windows[i]->x11event(event);
				break;
			}
		}
		
		/*switch (event.type) {
			case ClientMessage:
				if (*XGetAtomName(dis, event.xclient.message_type) ==
					*"WM_PROTOCOLS") {
					//cadence::doste::root.set("running", false);
				}
				break;
			case KeyRelease:
				keyevent = (XKeyEvent*)&event;
				wgd::keyboard->keyRelease((char)keyevent->keycode);
				break;
			case KeyPress:
				keyevent = (XKeyEvent*)&event;
				wgd::keyboard->keyPress((char)keyevent->keycode);
				break;

			case ButtonPress:
				buttonevent = (XButtonEvent*)&event;
				wgd::mouse->btnDown(buttonevent->button);
				break;
			case ButtonRelease:
				buttonevent = (XButtonEvent*)&event;
				wgd::mouse->btnUp(buttonevent->button);
				break;
			default:
				break;
		}*/
	}

}

extern "C" void finalise() {
	XCloseDisplay(dis);
}
