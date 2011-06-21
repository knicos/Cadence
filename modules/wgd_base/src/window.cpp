/*
 * WGD Library
 *
 * Authors: 
 * Date: 16/9/2007
 *
 */

#include <wgd/window.h>
#include <cadence/dstring.h>
#include <cadence/cadence.h>
#include <wgd/common.h>
#include <wgd/keyboard.h>
#include <wgd/mouse.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef LINUX
#include <GL/glx.h>

int attriblistAA4[] = { GLX_DEPTH_SIZE, 32, GLX_DOUBLEBUFFER, GLX_RGBA, GLX_SAMPLE_BUFFERS_ARB, GL_TRUE, GLX_SAMPLES_ARB, 4 , None };
int attriblistAA2[] = { GLX_DEPTH_SIZE, 32, GLX_DOUBLEBUFFER, GLX_RGBA, GLX_SAMPLE_BUFFERS_ARB, GL_TRUE, GLX_SAMPLES_ARB, 2 , None };
int attriblist[] = { GLX_DEPTH_SIZE, 32, GLX_DOUBLEBUFFER, GLX_RGBA, None };
#endif

#ifdef WIN32
typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

#define WGL_DRAW_TO_WINDOW_ARB         0x2001
#define WGL_ACCELERATION_ARB           0x2003
#define WGL_SUPPORT_OPENGL_ARB         0x2010
#define WGL_FULL_ACCELERATION_ARB      0x2027
#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_ALPHA_BITS_ARB             0x201B
#define WGL_STENCIL_BITS_ARB           0x2023
#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define WGL_SAMPLE_BUFFERS_ARB         0x2041
#define WGL_SAMPLES_ARB                0x2042

#endif

#include <iostream>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

#ifdef WIN32
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	//Use clicked window close button
	if (message == WM_CLOSE) {
		PostQuitMessage(0);
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
#endif

#undef True
#undef False

namespace wgd {
	OnEvent(GameWindow, evt_position) {
	
	}
	
	OnEvent(GameWindow, evt_size) {
		//cache window height
		m_height = get(ix::height);
	
		if (!m_init) {
			make();
			return;
		};

		//#ifdef LINUX
		//XResizeWindow(m_display, m_window, width(), height());
		//#endif

		//cadence::Object::destroyAll();
		//wgd::window = ((OID)WGD["window"]);
		//wgd::keyboard = ((OID)WGD["input"]["keyboard"]);
		//wgd::mouse = ((OID)WGD["input"]["mouse"]);
	}

	OnEvent(GameWindow, evt_title) {
		if (!m_init) return;

		dstring ti(get(ix::title));
		#ifdef LINUX
		XStoreName(m_display,m_window,ti);
		#endif
		
		#ifdef WIN32
		SetWindowTextA(m_hWnd, ti);
		#endif
	}

	OnEvent(GameWindow, evt_draw) {
		//if (value == True) {
			//std::cout << "draw\n";
		//draw();
			//set("begindraw", false);

		//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		//glLoadIdentity();
		//}
	}
	
	IMPLEMENT_EVENTS(GameWindow, Agent);
};

void wgd::GameWindow::make() {


	if (m_init == false) {

		#ifdef WIN32
		//Default is to centre the window on the screen.
		DEVMODE dmod;
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmod);
		if ((*this)["x"] == Null) {
			set("x", (int)((dmod.dmPelsWidth/2) - (width()/2)));
		}
		if ((*this)["y"] == Null) {
			set("y", (int)((dmod.dmPelsHeight/2) - (height()/2)));
		}
		#endif

	}

	#ifdef LINUX
	m_display = XOpenDisplay(0);
	if (m_display == 0) {
		//std::cerr << "Cannot open X11 display\n";
		Error(0, "Cannot open X11 display");
		return;
	}
	
	if (Processor::numProcessors() > 1) XLockDisplay(m_display);
	m_screen = DefaultScreen(m_display);

	//Get video modes
	XF86VidModeModeInfo **modes;
	int modeNum;
	int bestMode=0;

	if (fullScreen()) {
		XF86VidModeGetAllModeLines(m_display, m_screen, &modeNum, &modes);
		//Save desktop mode
		m_deskMode = *modes[0];
	
		//Find the mode we want.
		for (int i=0; i<modeNum; i++) {
			if ((modes[i]->hdisplay == width()) && (modes[i]->vdisplay == height())) {
				bestMode = i;
			}
		}
	}

	attriblistAA4[1] = depthBuffer();
	attriblistAA2[1] = depthBuffer();
	attriblist[1] = depthBuffer();

	m_visual = 0;
	if ((int)get("sampling") == 4)
		m_visual = glXChooseVisual(m_display, m_screen, attriblistAA4);
	if (m_visual == 0 || get("sampling") == Null || (int)get("sampling") == 2)
		m_visual = glXChooseVisual(m_display, m_screen, attriblistAA2);
	if (m_visual == 0)
		m_visual = glXChooseVisual(m_display, m_screen, attriblist);

	if (m_visual == 0) {
		Error(0, dstring("You seem to be making an invalid window:")
			+ "\n  width = " + width()
			+ "\n  height = " + height()
			+ "\n  colourdepth = " + colourDepth()
			+ "\n  depthbuffer = " + depthBuffer()
			+ "\n  fullscreen = " + fullScreen());
		return;
	}

	//Here we attempt to create a direct rendering opengl context (needed for shaders...).
	m_context = glXCreateContext(m_display, m_visual, 0, GL_TRUE);
	m_colormap = XCreateColormap(m_display, RootWindow(m_display, m_visual->screen), m_visual->visual, AllocNone);
	m_swa.colormap = m_colormap;
	m_swa.border_pixel = 0;
	m_swa.event_mask =	ExposureMask |
				StructureNotifyMask |
				KeyReleaseMask |
				KeyPressMask |
				ButtonPressMask |
				ButtonReleaseMask;

	if (fullScreen()) {
		XF86VidModeSwitchToMode(m_display, m_screen, modes[bestMode]);
        	XF86VidModeSetViewPort(m_display, m_screen, 0, 0);
		m_swa.override_redirect = true;
		XFree(modes);
	}

	m_window = XCreateWindow(	m_display,
					RootWindow(m_display,m_visual->screen),
					(fullScreen()) ? 0 : (int)get("x"),
					(fullScreen()) ? 0 : (int)get("y"),
					width(),
					height(),
					0,
					m_visual->depth,
					InputOutput,
					m_visual->visual,
					CWBorderPixel|CWColormap|CWEventMask | ((fullScreen()) ? CWOverrideRedirect : 0),
					&m_swa);

	//Set the windows title
	XStoreName(m_display, m_window, title());
	//Put window on the display.
	XMapWindow(m_display, m_window);

	if (fullScreen()) {
		XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, 0, 0);
		XGrabKeyboard(m_display, m_window, true, GrabModeAsync,
          		  GrabModeAsync, CurrentTime);
        	XGrabPointer(m_display, m_window, true, ButtonPressMask,
        		    GrabModeAsync, GrabModeAsync, m_window, None, CurrentTime);
	}

	m_makecontext = true;
	
	if (Processor::numProcessors() > 1) XUnlockDisplay(m_display);
	#endif

	#ifdef WIN32
	WNDCLASS wc;
	m_hInst = GetModuleHandle(NULL);
	
	//Convert name and title to unicode
	wchar_t *wClassName = new wchar_t[4];
	mbstowcs(wClassName, "wgd", 4);
	
	if(get(ix::title)==Null) set(ix::title, dstring("WGD-Lib v3"));
	int tLen = get(ix::title).get(ix::size);
	dstring title(get(ix::title));
	wchar_t *wTitle = new wchar_t[tLen+1];
	mbstowcs(wTitle, (const char*)title, tLen+1);


	if (m_init == false) {
		//register window class
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hInst;//GetCurrentProcess();
		wc.hIcon = LoadIcon(m_hInst, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = (LPCWSTR)wClassName;
		RegisterClass(&wc);
	}

	if (fullScreen()) {
		DEVMODE screensettings;
		memset(&screensettings, 0, sizeof(screensettings));
		screensettings.dmSize = sizeof(screensettings);
		screensettings.dmPelsWidth = width();
		screensettings.dmPelsHeight = height();
		screensettings.dmBitsPerPel = 24; //colourDepth();
		screensettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&screensettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			fullScreen(false);
			//wgd::cout << wgd::ERR << "Unable to change to fullscreen.\n";
		}
	}

	RECT crect;
	crect.top = (int)get("y");
	crect.left = (int)get("x");
	crect.right = crect.left + width();
	crect.bottom = crect.top + height();

	if (!fullScreen()) {
		AdjustWindowRect(&crect, WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE, false);
	} 

	//Mouse::titleSize((int)get("y") - crect.top);

	m_hWnd = CreateWindow(	(LPCWSTR)wClassName,
				(LPCWSTR)wTitle,
				((fullScreen()) ? 0 : WS_CAPTION) | WS_POPUPWINDOW | WS_VISIBLE,
				((fullScreen()) ? 0 : (int)get("x")),
				((fullScreen()) ? 0 : (int)get("y")),
				crect.right-crect.left,
				crect.bottom-crect.top,
				NULL, NULL, m_hInst, NULL);

	m_hDC = GetDC(m_hWnd);

	//Set pixel format for the DC
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = colourDepth();
	pfd.cDepthBits = depthBuffer();
	pfd.iLayerType = PFD_MAIN_PLANE;

	//if (m_hasmultisample)
	//	SetPixelFormat(m_hDC, m_pixformat, &pfd);
	//else
		SetPixelFormat(m_hDC, ChoosePixelFormat(m_hDC, &pfd), &pfd);

	m_makecontext = true;

	#endif

	draw();

	m_init = true;
}

void wgd::GameWindow::finaliseInternal() {
	#ifdef LINUX

	XLockDisplay(m_display);
	if (fullScreen()) {
		XF86VidModeSwitchToMode(m_display, m_screen, &m_deskMode);
        	XF86VidModeSetViewPort(m_display, m_screen, 0, 0);
	}

	glXDestroyContext(m_display, m_context);
	XDestroyWindow(m_display,m_window);
	XCloseDisplay(m_display);
	#endif
	
	#ifdef WIN32
	if (fullScreen()) {
		ChangeDisplaySettings(NULL, 0);
	}
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hRC);
	ReleaseDC(m_hWnd, m_hDC);
	DestroyWindow(m_hWnd);
	//UnregisterClass("wgd", s_hInst);
	//s_hInst=NULL;
	#endif
}

/*void wgd::GameWindow::title(const char * ti) {
	dstring ti2(ti);
	WGD[ix::window].set(ix::title, ti2);
}

const char *wgd::GameWindow::title() {
	dstring ti(WGD[ix::window][ix::title]);
	return (char*)ti;
}*/

void wgd::GameWindow::resolution(int width,int height) {
	set(ix::width, width);
	set(ix::height, height);
}

//void wgd::GameWindow::icon(const char*) {
//}

/*void wgd::GameWindow::clearColour(const Colour &c) {
	if (WGD[ix::window][ix::clearcolour] == Null)
		WGD[ix::window].set(ix::clearcolour, wgd::DB::createObject());

	WGD[ix::window][ix::clearcolour].set(ix::r, c.r());
	WGD[ix::window][ix::clearcolour].set(ix::g, c.g());
	WGD[ix::window][ix::clearcolour].set(ix::b, c.b());
}*/

//Colour wgd::GameWindow::clearColour() {
//	return wgd::Colour(WGD[ix::window][ix::clearcolour][ix::r], WGD[ix::window][ix::clearcolour][ix::g], WGD[ix::window][ix::clearcolour][ix::b]);
//}

void wgd::GameWindow::position(int x,int y) {
	set(ix::x, x);
	set(ix::y, y);
}

void wgd::GameWindow::draw() {

	if (m_makecontext) {
		m_makecontext = false;
	
		#ifdef LINUX
		//We only ever have one context, so make it current.
		glXMakeCurrent(m_display, m_window, m_context);
		#endif
	
		#ifdef WIN32
		//create render context (RC)
		m_hRC = wglCreateContext(m_hDC);
		if(!wglMakeCurrent(m_hDC, m_hRC)){
			Error(0, "wgdMakeCurrent failed to work and gave the error " + (int)GetLastError());
			return;
		}
	
		if (m_init == false) {
			//Check for multisample support.
			PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			
			int pixelFormat = 0;
			bool valid = false;
			UINT numFormats = 0;
			float fAttributes[] = {0,0};
		
			if (wglChoosePixelFormatARB!=0) {
				// These Attributes Are The Bits We Want To Test For In Our Sample
				// Everything Is Pretty Standard, The Only One We Want To 
				// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
				// These Two Are Going To Do The Main Testing For Whether Or Not
				// We Support Multisampling On This Hardware
				int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
					WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
					WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
					WGL_COLOR_BITS_ARB,24,
					WGL_ALPHA_BITS_ARB,8,
					WGL_DEPTH_BITS_ARB,16,
					WGL_STENCIL_BITS_ARB,0,
					WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
					WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
					WGL_SAMPLES_ARB, 4 ,						// Check For 4x Multisampling
					0,0};
	
				//iAttributes[11] = depthBuffer();
			
				//if ((int)WGD["fseffects"]["sample"] == 4)
				//	// First We Check To See If We Can Get A Pixel Format For 4 Samples
				//	valid = (wglChoosePixelFormatARB(s_hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats)) ? true : false;
			
				// Our Pixel Format With 4 Samples Failed, Test For 2 Samples
				//if (!valid || WGD["fseffects"]["sample"] == Null || (int)WGD["fseffects"]["sample"] == 2) {
				//	iAttributes[19] = 2;
				//	valid = (wglChoosePixelFormatARB(s_hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats)) ? true : false;
				//}
			}
		
			if (valid && numFormats >= 1) {
				finaliseInternal();
				m_hasmultisample = true;
				m_init = true;
				m_pixformat = pixelFormat;
				//GameWindow::initialise();
				return;
			}
		}
	
		#endif
		
		glClearColor(	0.0,
				0.0,
				0.0,
				0.5f);
	
	
	
	
		glClear(GL_COLOR_BUFFER_BIT);
		glClearDepth(1.0f);                     // Depth buffer setup
		//glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);                // Enables depth testing
		glDepthFunc(GL_LEQUAL);                 // The type of depth testing to do
		glCullFace(GL_BACK);					//back face culling
		glEnable(GL_CULL_FACE);					
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // Really nice perspective calculations
		glEnable(GL_BLEND);							// Enable alpha blending of textures
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
		int glerr;
		if ((glerr = glGetError()) != 0) {
			Error(0, "An OpenGL error occured: " + glerr);
		}
	}

	//if (FSEffects::motionBlur() > 0.000001) {
	//	wgd::cout << "MOTION\n";
	//	glAccum(GL_ACCUM, 1.f/5);
	//	glAccum(GL_RETURN, 1.f);
	//}

	#ifdef LINUX
	glXSwapBuffers(m_display, m_window);
	#endif
	#ifdef WIN32
	SwapBuffers(m_hDC);
	#endif
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
}

void wgd::GameWindow::defaults() {

	if ((*this)["width"] == Null)
		set("width", 800);
	if ((*this)["height"] == Null)
		set("height", 600);
	if ((*this)["colourdepth"] == Null)
		set("colourdepth", 32);
	if ((*this)["depthbuffer"] == Null)
		set("depthbuffer", 16);
	if ((*this)["title"] == Null)
		set("title", DString("WGD Default Title"));
}

wgd::GameWindow::GameWindow(const OID &o) : Agent(o), m_init(false), m_makecontext(false) {
	#ifdef LINUX
	if (Processor::numProcessors() > 1) XInitThreads();
	#endif

	//Set defaults.
	defaults();
	registerEvents();
	//make();
}

wgd::GameWindow::~GameWindow() {
	finaliseInternal();
}

/*void wgd::GameWindow::evtClearCol(DOSTE_HANDLER) {
	glClearColor(	(float)WGD[ix::window][ix::clearcolour][ix::r],
			(float)WGD[ix::window][ix::clearcolour][ix::g],
			(float)WGD[ix::window][ix::clearcolour][ix::b],
			0.5f);
}*/

/*void wgd::GameWindow::evtResolution(DOSTE_HANDLER) {
	if (s_hasinit) {
		if (validRes()) {

			if ((int)id == 3 && (bool)value == false) {
				#ifdef LINUX
				XF86VidModeSwitchToMode(s_display, s_screen, &s_deskMode);
        			XF86VidModeSetViewPort(s_display, s_screen, 0, 0);
				#else
				ChangeDisplaySettings(NULL, 0);
				#endif
			}

			Resource::destroyAll();
			finaliseInternal();
			initialise();
			//Lighting::initialise();
			Shader::current(0);
			Lighting::reset();
			Mouse::reset();
			FSEffects::reset();
		} else {
			wgd::cout << wgd::WARN << "Invalid resolution " << width() << "x" << height() << "\n";
		}
	}
}*/

/*bool wgd::GameWindow::validRes() {
	int w = width();
	int h = height();
	int c = colourDepth();

	if (c != 16 && c != 32)
		return false;

	for (int i=0; i<100; i++) {
		if (WGD[ix::window][ix::resolutions][i] == Null)
			break;

		if ((int)WGD[ix::window][ix::resolutions][i][ix::width] == w && (int)WGD[ix::window][ix::resolutions][i][ix::height] == h)
			return true;
	}

	return false;
}*/

/*void wgd::GameWindow::evtTitle(DOSTE_HANDLER) {
	if (s_hasinit) {
		dstring ti(value);
		#ifdef LINUX
		XStoreName(s_display,s_window,(char*)ti);
		#endif
		
		#ifdef WIN32
		SetWindowTextA(s_hWnd, (char*)ti);
		#endif
	}
}*/

