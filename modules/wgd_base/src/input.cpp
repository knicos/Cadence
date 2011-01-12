#include <cadence/cadence.h>
#include <cadence/dstring.h>
#include <wgd/window.h>
#include <wgd/input.h>
#include <wgd/mouse.h>
#include <cadence/agent.h>

#ifdef LINUX
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

#include <iostream>

using namespace cadence;
using namespace cadence::doste;
using namespace wgd;

wgd::Key wgd::Keyboard::KEY_LEFT;
wgd::Key wgd::Keyboard::KEY_UP;
wgd::Key wgd::Keyboard::KEY_RIGHT;
wgd::Key wgd::Keyboard::KEY_DOWN;
wgd::Key wgd::Keyboard::KEY_RETURN;
wgd::Key wgd::Keyboard::KEY_ENTER;
wgd::Key wgd::Keyboard::KEY_SPACE;
wgd::Key wgd::Keyboard::KEY_CTRL;
wgd::Key wgd::Keyboard::KEY_SHIFT;
wgd::Key wgd::Keyboard::KEY_RCTRL;
wgd::Key wgd::Keyboard::KEY_RSHIFT;
wgd::Key wgd::Keyboard::KEY_ALT;
wgd::Key wgd::Keyboard::KEY_ALTGR;
wgd::Key wgd::Keyboard::KEY_DEL;
wgd::Key wgd::Keyboard::KEY_BKSPACE;
wgd::Key wgd::Keyboard::KEY_END;
wgd::Key wgd::Keyboard::KEY_PGDOWN;
wgd::Key wgd::Keyboard::KEY_PGUP;
wgd::Key wgd::Keyboard::KEY_HOME;
wgd::Key wgd::Keyboard::KEY_INSERT;
wgd::Key wgd::Keyboard::KEY_PAUSE;
wgd::Key wgd::Keyboard::KEY_TAB;
wgd::Key wgd::Keyboard::KEY_ESCAPE;
wgd::Key wgd::Keyboard::KEY_F1;
wgd::Key wgd::Keyboard::KEY_F2;
wgd::Key wgd::Keyboard::KEY_F3;
wgd::Key wgd::Keyboard::KEY_F4;
wgd::Key wgd::Keyboard::KEY_F5;
wgd::Key wgd::Keyboard::KEY_F6;
wgd::Key wgd::Keyboard::KEY_F7;
wgd::Key wgd::Keyboard::KEY_F8;
wgd::Key wgd::Keyboard::KEY_F9;
wgd::Key wgd::Keyboard::KEY_F10;
wgd::Key wgd::Keyboard::KEY_F11;
wgd::Key wgd::Keyboard::KEY_F12;

wgd::Keyboard::Keyboard(const OID &o) : cadence::Agent(o) {
	KEY_LEFT = "left";
	KEY_RIGHT = "right";
	KEY_UP = "up";
	KEY_DOWN = "down";
	KEY_ESCAPE = "escape";
	KEY_RETURN = "return";
	KEY_ENTER = "enter";
	KEY_SPACE = ' ';
	KEY_CTRL = "ctrl";
	KEY_RCTRL = "rctrl";
	KEY_SHIFT = "shift";
	KEY_RSHIFT = "rshift";
	KEY_ALT = "alt";
	KEY_ALTGR = "altgr";
	KEY_DEL = "delete";
	KEY_BKSPACE = "bkspace";
	KEY_END = "end";
	KEY_PGUP = "pgup";
	KEY_PGDOWN = "pgdown";
	KEY_HOME = "home";
	KEY_INSERT = "insert";
	KEY_PAUSE = "pause";
	KEY_TAB = "tab";
	KEY_F1 = "F1";
	KEY_F2 = "F2";
	KEY_F3 = "F3";
	KEY_F4 = "F4";
	KEY_F5 = "F5";
	KEY_F6 = "F6";
	KEY_F7 = "F7";
	KEY_F8 = "F8";
	KEY_F9 = "F9";
	KEY_F10 = "F10";
	KEY_F11 = "F11";
	KEY_F12 = "F12";
}

wgd::Keyboard::~Keyboard() {

}

void wgd::Keyboard::keyPress(char key) {
	wgd::Key rkey = convert(key);
	get("keys")[rkey].set(true);
	char c = ascii(rkey, (bool)((OID)keys()[KEY_SHIFT]) | (bool)((OID)keys()[KEY_RSHIFT]));

	if (c != 0) {
		set("ascii", c);
		set("key", rkey);
		set("press", Void);
		set("pressed", true);
		set("pressed", false);
	}
	
	m_pressed[rkey] = true;
}

void wgd::Keyboard::keyRelease(char key) {
	wgd::Key rkey = convert(key);
	(*this)["keys"][rkey].set(false);
}

char wgd::Keyboard::ascii(const wgd::Key &key, bool shift){
	//non-character OIDs convert to '@'
	//if((char)key == '@'){
		if(key == KEY_RETURN) return '\n';
		if(key == KEY_TAB) return '\t';
		if(key == KEY_BKSPACE) return 8;
	//} else {
		char ckey = (char)key;
		if(shift){
			switch(ckey){
			case '`':	return '?';
			case '1':	return '!';
			case '2':	return '"';
			case '3':	return '?';
			case '4':	return '$';
			case '5':	return '%';
			case '6':	return '^';
			case '7':	return '&';
			case '8':	return '*';
			case '9':	return '(';
			case '0':	return ')';
			case '-':	return '_';
			case '=':	return '+';
			case '[':	return '{';
			case ']':	return '}';
			case ';':	return ':';
			case '\'':	return '@';
			case '#':	return '~';
			case '\\':	return '|';
			case ',':	return '<';
			case '.':	return '>';
			case '/':	return '?';
			default:
				if(ckey >= 97 && ckey <= 122) return ckey-32;
			}
		} 
		if(ckey >= 32 && ckey <= 126) return ckey;
	//}
	return 0;
}

wgd::Key wgd::Keyboard::convert(char key) {
	wgd::Key rkey;

	#ifdef WIN32
	switch (key) {
	case 27:	rkey = KEY_ESCAPE; break;
	case -90:	rkey = KEY_UP; break;
	case -91:	rkey = KEY_LEFT; break;
	case -88:	rkey = KEY_DOWN; break;
	case -89:	rkey = KEY_RIGHT; break;
	case 13:	rkey = KEY_RETURN; break;
	case 8:		rkey = KEY_BKSPACE; break;
	case 32:	rkey = KEY_SPACE; break;
	case 17:	rkey = KEY_CTRL; break;
	case -111:	rkey = KEY_RCTRL; break;
	case 16:	rkey = KEY_SHIFT; break;
	//case ??:	rkey = KEY_ALT; break; //umm - alt does not print out?
	case -110:	rkey = KEY_ALTGR; break;
	case -83:	rkey = KEY_INSERT; break;
	case -82:	rkey = KEY_DEL; break;
	case -92:	rkey = KEY_HOME; break;
	case -93:	rkey = KEY_END; break;
	case -95:	rkey = KEY_PGUP; break;
	case -94:	rkey = KEY_PGDOWN; break;
	case 19:	rkey = KEY_PAUSE; break;
	case 9:		rkey = KEY_TAB; break;
	
	case 112:	rkey = KEY_F1; break;
	case 113:	rkey = KEY_F2; break;
	case 114:	rkey = KEY_F3; break;
	case 115:	rkey = KEY_F4; break;
	case 116:	rkey = KEY_F5; break;
	case 117:	rkey = KEY_F6; break;
	case 118:	rkey = KEY_F7; break;
	case 119:	rkey = KEY_F8; break;
	case 120:	rkey = KEY_F9; break;
	case 121:	rkey = KEY_F10; break;
	case 122:	rkey = KEY_F11; break;
	case 123:	rkey = KEY_F12; break;
	
	//more mappings
	case -33:	rkey = '`'; break;
	case -67:	rkey = '-'; break;
	case -69:	rkey = '='; break;
	case -37:	rkey = '['; break;
	case -35:	rkey = ']'; break;
	case -70:	rkey = ';'; break;
	case -64:	rkey = '\''; break;
	case -34:	rkey = '#'; break;
	case -36:	rkey = '\\'; break;
	case -68:	rkey = ','; break;
	case -66:	rkey = '.'; break;
	case -65:	rkey = '/'; break;
	
	
	default:	
		if ((int)key >= 65 && (int)key <= 90)
			rkey = (char)((int)key+32);
		else
			rkey = key;
		break;
	}
	
	return rkey;
	#endif
	
	#ifdef LINUX
	KeySym keysym = XKeycodeToKeysym(wgd::window->getXDisplay(), key, 0);
	switch (keysym) {
	case XK_Left:		return KEY_LEFT;
	case XK_Up:			return KEY_UP;
	case XK_Right:		return KEY_RIGHT;
	case XK_Down:		return KEY_DOWN;
	case XK_Return:		return KEY_RETURN;
	case XK_Escape:		return KEY_ESCAPE;
	case XK_BackSpace:	return KEY_BKSPACE;
	case XK_Tab:		return KEY_TAB;
	case XK_Delete:		return KEY_DEL;
	case XK_Pause:		return KEY_PAUSE;
	case XK_Home:		return KEY_HOME;
	case XK_Page_Up:	return KEY_PGUP;
	case XK_Page_Down:	return KEY_PGDOWN;
	case XK_End:		return KEY_END;
	case XK_Insert:		return KEY_INSERT;
	case XK_KP_Enter:	return KEY_ENTER;
	case XK_F1:			return KEY_F1;
	case XK_F2:			return KEY_F2;
	case XK_F3:			return KEY_F3;
	case XK_F4:			return KEY_F4;
	case XK_F5:			return KEY_F5;
	case XK_F6:			return KEY_F6;
	case XK_F7:			return KEY_F7;
	case XK_F8:			return KEY_F8;
	case XK_F9:			return KEY_F9;
	case XK_F10:		return KEY_F10;
	case XK_F11:		return KEY_F11;
	case XK_F12:		return KEY_F12;
	case XK_Shift_L:	return KEY_SHIFT;
	case XK_Shift_R:	return KEY_RSHIFT;
	case XK_Control_L:	return KEY_CTRL;
	case XK_Control_R:	return KEY_RCTRL;
	case XK_Alt_L:		return KEY_ALT;
	case XK_Alt_R:		return KEY_ALTGR; // Is this correct?
	
	default:	return (char)keysym;
	}
	#endif
}



wgd::Input::Input(const OID &o) : Agent(o) {
	registerEvents();
}

wgd::Input::~Input() {
}

void wgd::Input::update() {
	
	mouse->update();
	keyboard->update();

	#ifdef LINUX
	XEvent event;
	XKeyEvent *keyevent;
	XButtonEvent *buttonevent;

	//if (Processor::numProcessors() > 1) XLockDisplay(wgd::window->getXDisplay());

	while (XPending(wgd::window->getXDisplay())) {
		XNextEvent(wgd::window->getXDisplay(), &event);
		switch (event.type) {
			case ClientMessage:
				if (*XGetAtomName(wgd::window->getXDisplay(), event.xclient.message_type) ==
					*"WM_PROTOCOLS") {
					cadence::doste::root.set("running", false);
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
		}
	}
	//if (Processor::numProcessors() > 1) XUnlockDisplay(wgd::window->getXDisplay());

	#else

	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		switch (msg.message) {
		case WM_QUIT:
			root["running"] = false;
			break;

		case WM_KEYDOWN:
			wgd::keyboard->keyPress((char)(msg.wParam + (((msg.lParam >> 24) * 0x1) << 7)));
			break;

		case WM_KEYUP:
			wgd::keyboard->keyRelease((char)(msg.wParam + (((msg.lParam >> 24) * 0x1) << 7)));
			break;

		case WM_LBUTTONDOWN:
			wgd::mouse->btnDown(1);
			break;

		case WM_LBUTTONUP:
			wgd::mouse->btnUp(1);
			break;
		case WM_MBUTTONDOWN:
			wgd::mouse->btnDown(2);
			break;
		case WM_MBUTTONUP:
			wgd::mouse->btnUp(2);
			break;
		case WM_RBUTTONDOWN:
			wgd::mouse->btnDown(3);
			break;
		case WM_RBUTTONUP:
			wgd::mouse->btnUp(3);
			break;
		case WM_MOUSEWHEEL:
			//wgd::mouse->set(ix::deltaz, GET_WHEEL_DELTA_WPARAM(msg.wParam));
			wgd::mouse->wheel(GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA);
			break;
			
		default:
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	#endif
}

