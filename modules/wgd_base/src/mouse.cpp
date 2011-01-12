#include <wgd/mouse.h>
#include <wgd/window.h>
#include <wgd/index.h>
#include <cadence/agent.h>

//If uses DMX
//#include <X11/extensions/dmxext.h>

#define PIXELS_PER_SEC 3000.0f

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

Button Mouse::BTN_LEFT;
Button Mouse::BTN_RIGHT;
Button Mouse::BTN_MIDDLE;
int Mouse::s_wheel = 0;
int Mouse::lastX;
int Mouse::lastY;
bool Mouse::s_vis = true;

Mouse::Mouse(const OID &o) : Agent(o) {
	registerEvents();
	
	if((*this)[ix::visible] ==  Null) set(ix::visible, true);
	
	if ((*this)[ix::buttons] == Null) {
		set(ix::buttons, OID::create());
	}

	if ((*this)[ix::axes] == Null) {
		set(ix::axes, OID::create());
	}

	if ((*this)[ix::axes][0] == Null) {
		(*this)[ix::axes][0] = OID::create();
	}

	if ((*this)[ix::axes][1] == Null) {
		(*this)[ix::axes][1] = OID::create();
	}

	BTN_LEFT = "left";
	BTN_RIGHT = "right";
	BTN_MIDDLE = "middle";

	reset();
}

Mouse::~Mouse() {
}

namespace wgd {
	OnEvent(Mouse, evt_cursor) {
		reset();
	}

	OnEvent(Mouse, evt_visible) {
		reset();
	}

	OnEvent(Mouse, evt_position) {
		int newX = x();
		int newY = y();
		if(lastX!=newX || lastY!=newY) {
			position(newX, newY);
		}
	}

	IMPLEMENT_EVENTS(Mouse, Agent);
};

wgd::vector2d Mouse::position() {
	return vector2d(get(ix::x), get(ix::y));
}
void Mouse::position(const wgd::vector2d &p) {
	position((int)p.x, (int)p.y);
}

void Mouse::position(int cx, int cy) {
	lastX = cx;
	lastY = cy;
	
	#ifdef WIN32
	RECT rect; rect.top = 0; rect.left = 0; rect.right = 100; rect.bottom=100;
	if (!wgd::window->fullScreen()) {
		AdjustWindowRect(&rect, WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE, false);
	}
	cx = (int)wgd::window->get(ix::x) + cx - rect.left;
	cy = (int)wgd::window->get(ix::y) + cy - rect.top;
	SetCursorPos(cx, cy);
	#elif LINUX
	XWarpPointer(wgd::window->getXDisplay(), 0, wgd::window->getXWindow(), 0, 0, 0, 0, cx, cy);
	#endif
}

void Mouse::reset() {
	#ifdef WIN32
	if ((get(ix::cursor) == Null) && (visible())) {
		if(!s_vis) ShowCursor(true);
		s_vis = true;
	} else {
		if(s_vis) ShowCursor(false);
		s_vis=false;
	}
	#endif

	#ifdef LINUX
	Pixmap bm_no;
	Colormap cmap;
	Cursor no_ptr;
	XColor black, dummy;
	static char bm_no_data[] = {0, 0, 0, 0, 0, 0, 0, 0};

	cmap = DefaultColormap(wgd::window->getXDisplay(), wgd::window->getXScreen());
	XAllocNamedColor(wgd::window->getXDisplay(), cmap, "black", &black, &dummy);
	bm_no = XCreateBitmapFromData(wgd::window->getXDisplay(), wgd::window->getXWindow(), bm_no_data, 8, 8);
	no_ptr = XCreatePixmapCursor(wgd::window->getXDisplay(), bm_no, bm_no, &black, &black, 0, 0);

	if ((get(ix::cursor) == Null) && (visible())) 
		XUndefineCursor(wgd::window->getXDisplay(), wgd::window->getXWindow());
	else
		XDefineCursor(wgd::window->getXDisplay(), wgd::window->getXWindow(), no_ptr);
	XFreeCursor(wgd::window->getXDisplay(), no_ptr);
	if (bm_no != None)
		XFreePixmap(wgd::window->getXDisplay(), bm_no);
	XFreeColors(wgd::window->getXDisplay(), cmap, &black.pixel, 1, 0);
	#endif
}

void Mouse::btnDown(int btn) {
	switch(btn) {
	case 1:	(*this)[ix::buttons][BTN_LEFT] = true; break;
	case 2:	(*this)[ix::buttons][BTN_MIDDLE] = true; break;
	case 3:	(*this)[ix::buttons][BTN_RIGHT] = true; break;
	// Linux treats button 4 as scroll wheel up and button 5 as scroll wheel down
	#ifdef LINUX
	case 4:	wheel(-1); break;
	case 5:	wheel(1); break;
	#else
	case 4: break;
	case 5: break;
	#endif
	case 6:	break;
	default: break;
	}
}

void Mouse::btnUp(int btn) {
	switch(btn) {
	case 1:	(*this)[ix::buttons][BTN_LEFT] = false; break;
	case 2:	(*this)[ix::buttons][BTN_MIDDLE] = false; break;
	case 3:	(*this)[ix::buttons][BTN_RIGHT] = false; break;
	case 4:	break;
	case 5:	break;
	case 6:	break;
	default: break;
	}
}

int Mouse::bNo(const Button&b) const {
	if(b.isInt()) return b;
	if(b == BTN_LEFT) return 1;
	if(b == BTN_MIDDLE) return 2;
	if(b == BTN_RIGHT) return 3;
	return 0;
}

void Mouse::update() {
	//mouse wheel
	set(ix::deltaz, s_wheel); s_wheel=0;
	
	int newX, newY;
	
	#ifdef WIN32
	POINT pnt;
	GetCursorPos(&pnt);

	RECT rect;
	rect.top = 0; rect.left = 0; rect.right = 100; rect.bottom=100;
	if (!wgd::window->fullScreen()) {
		AdjustWindowRect(&rect, WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE, false);
	}
	
	//update window position
	int wx = window->get(ix::x);
	int wy = window->get(ix::y);
	RECT wpos;
	GetWindowRect(window->getHWND(), &wpos);
	if(wx != wpos.left) window->set(ix::x, (int)wpos.left);
	if(wy != wpos.top)  window->set(ix::y, (int)wpos.top);
	
	//calculate mouse position relative to the window
	newX = pnt.x + rect.left - wpos.left;
	newY = pnt.y + rect.top  - wpos.top;
	#endif

	#ifdef LINUX
	Window wroot, wchild;
	int rx,ry;
	unsigned int mask;

	XQueryPointer(	wgd::window->getXDisplay(),
			wgd::window->getXWindow(),
			&wroot,	//Root Window
			&wchild,//Child Window
			&rx,	//Root X
			&ry,	//Root Y
			&newX,	//Window X -- newX set here
			&newY,	//Window Y -- newY set here
			&mask);	//Mask

	#endif
	
	set(ix::deltax, newX - lastX, true);
	set(ix::deltay, newY - lastY);
	
	if (grab()) {
		position(lastX, lastY);
	} else {
		lastX = newX;
		lastY = newY;
		
		set(ix::x, lastX, true);
		set(ix::y, lastY);
	}
	
	//record last button states
	m_last[1] = pressed(BTN_LEFT);
	m_last[2] = pressed(BTN_MIDDLE);
	m_last[3] = pressed(BTN_RIGHT);
	bool moved = newX - lastX != 0 || newY - lastY != 0;
	for(int i=1; i<=3; i++) m_moved[i] = (m_last[i] && (moved || m_moved[i]));
}

