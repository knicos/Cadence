#ifndef _SWINDOW_
#define _SWINDOW_

#include <cadence/agent.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

using namespace cadence;

class Shape_Window : public Agent {
	public:
	Shape_Window(const OID &o);
	~Shape_Window();

	OBJECT(Agent, Shape_Window);
	
	PROPERTY_RF(int, x, "x");
	PROPERTY_RF(int, y, "y");
	PROPERTY_RF(int, width, "width");
	PROPERTY_RF(int, height, "height");
	PROPERTY_RF(dstring, title, "title");
	
	Window winid() { return m_win; }
	void x11event(XEvent &evt);

	BEGIN_EVENTS(Agent);
	EVENT(evt_visible, (*this)("visible"));
	EVENT(evt_x, (*this)("x"));
	EVENT(evt_y, (*this)("y"));
	EVENT(evt_width, (*this)("width"));
	EVENT(evt_height, (*this)("height"));
	EVENT(evt_title, (*this)("title"));
	EVENT(evt_canvas, (*this)("canvas")(cadence::doste::All));
	END_EVENTS;
	
	private:
	bool m_ignore;
	Window m_win;
};

#endif
