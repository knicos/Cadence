#ifndef _SLINE_
#define _SLINE_

#include "swindow.h"

class Shape_Line : public Agent {
	public:
	Shape_Line(const OID &o) : Agent(o) { m_win = 0; registerEvents(); }
	~Shape_Line() {};

	OBJECT(Agent, Shape_Line);
	
	void redraw();
	void setWindow(Shape_Window *w) { m_win = w; }

	BEGIN_EVENTS(Agent);
	EVENT(evt_x1, (*this)("p1")("x"));
	EVENT(evt_y1, (*this)("p1")("y"));
	EVENT(evt_x2, (*this)("p2")("x"));
	EVENT(evt_y2, (*this)("p2")("y"));
	END_EVENTS;
	
	private:
	Shape_Window *m_win;
};

#endif
