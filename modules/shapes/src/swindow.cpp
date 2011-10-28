#include "swindow.h"

extern Display *dis;

Shape_Window::Shape_Window(const OID &o) : Agent(o) {

	//std::cout << "Making Window\n";

	m_win = XCreateSimpleWindow(dis, RootWindow(dis, 0), x(), y(), width(), height(), 0, BlackPixel (dis, 0), BlackPixel(dis, 0));
	XStoreName(dis, m_win, title());
	XSelectInput(dis, m_win, StructureNotifyMask );
	XMapWindow(dis, m_win);
	XFlush(dis);

	registerEvents();
}

Shape_Window::~Shape_Window() {
	
	
}

void Shape_Window::x11event(XEvent &evt) {
	switch(evt.type){
		case ConfigureNotify:
			if (width() != evt.xconfigure.width) { m_ignore = true; set("width", evt.xconfigure.width); }
			if (height() != evt.xconfigure.height) { m_ignore = true; set("height", evt.xconfigure.height); }
			if (x() != evt.xconfigure.x) { m_ignore = true; set("x", evt.xconfigure.x); }
			if (y() != evt.xconfigure.y) { m_ignore = true; set("y", evt.xconfigure.y); }
			break;
		
	}
	
}

OnEvent(Shape_Window , evt_visible) {
	
}

OnEvent(Shape_Window , evt_canvas) {
	std::cout << "Canvas Change\n";
}

OnEvent(Shape_Window , evt_x) {
	if (!m_ignore)
	XMoveWindow(dis,m_win,x(),y());
	m_ignore = false;
}

OnEvent(Shape_Window , evt_y) {
	if (!m_ignore)
	XMoveWindow(dis,m_win,x(),y());
	m_ignore = false;
}

OnEvent(Shape_Window , evt_width) {
	if (!m_ignore)
	XResizeWindow(dis,m_win,width(),height());
	m_ignore = false;
}

OnEvent(Shape_Window , evt_height) {
	if (!m_ignore)
	XResizeWindow(dis,m_win,width(),height());
	m_ignore = false;
}

OnEvent(Shape_Window , evt_title) {
	//std::cout << "Title changed\n";
		//#ifdef LINUX
		XStoreName(dis,m_win,title());
		XFlush(dis);
		//#endif
		
		//#ifdef WIN32
		//SetWindowTextA(m_hWnd, ti);
		//#endif
}

IMPLEMENT_EVENTS(Shape_Window, Agent);
