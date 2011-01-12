/*
 * WGD Library
 *
 * Authors: 
 * Date: 16/9/2007
 *
 */

#ifndef _WGD_WINDOW_
#define _WGD_WINDOW_

#ifdef WIN32
#include <windows.h>
#endif

#include <wgd/dll.h>
#include <cadence/agent.h>
#include <cadence/dstring.h>
#include <cadence/doste/definition.h>
#include <cadence/doste/modifiers.h>
#include <wgd/index.h>

#ifdef LINUX
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>
#endif

namespace wgd {

	//class Colour;

	/**
	 * The OpenGL window for this game. This static class enables you to
	 * access and change properties of the window including its resolution
	 * and fullscreen status. It corresponds to the wgd(window) object in
	 * the configuration database. Note that you do not need to create a
	 * window as this is done for you by Game.
	 * @see Game
	 */
	class BASEIMPORT GameWindow : public cadence::Agent {

		public:

		OBJECT(Agent, GameWindow);

		GameWindow(const cadence::doste::OID &o);
		~GameWindow();

		/**
		 * Set the windows title.
		 * @param title A null terminated string.
		 */
		PROPERTY_WF(cadence::dstring, title, ix::title);
		/**
		 * Get the current title string for the window.
		 * @return The title string.
		 */
		PROPERTY_RF(cadence::dstring, title, ix::title);
		/**
		 * Change the windows or screens resolution. This must
		 * be a valid resolution or nothing will happen. Valid resolutions
		 * are specified in the configuration database but this will also
		 * check hardware for fullscreen mode.
		 * @param width Width in pixels.
		 * @param height Height in pixels.
		 */
		void resolution(int,int);
		/**
		 * Get the pixel width of the window.
		 * @return Width in pixels.
		 */
		PROPERTY_RF(int, width, ix::width);
		/**
		 * Get the pixel height of the window.
		 * @return Height in pixels.
		 */
		int height() { return m_height; }
		/**
		 * Get the number of colour bits.
		 * @return Colour bits.
		 */
		PROPERTY_RF(int, colourDepth, ix::colourdepth);
		/**
		 * Get the number of depth buffer bits.
		 * @return Depth buffer bits.
		 */
		PROPERTY_RF(int, depthBuffer, ix::depthbuffer);
		/**
		 * Change the depth buffer bits for the window.
		 * @param bits Number of depth bits.
		 */
		PROPERTY_WF(int, depthBuffer, ix::depthbuffer);
		/**
		 * Set the fullscreen status of this window. True will make
		 * it fullscreen and false will make it a window.
		 * @param fscreen Boolean to change fullscreen status.
		 */
		PROPERTY_WF(bool, fullScreen, ix::fullscreen);
		/**
		 * Get the current fullscreen status.
		 * @return True if currently in fullscreen mode.
		 */
		PROPERTY_RF(bool, fullScreen, ix::fullscreen);
		/**
		 * Change the default OpenGL clear colour to this colour.
		 * @param c RGB Colour.
		 */
		//static void clearColour(const Colour &);
		/**
		 * Get the current clear colour.
		 * @return A colour object containing the RGB values.
		 */
		//static Colour clearColour();
		/**
		 * Set the windows position on the screen.
		 * @param x X-Coordinate
		 * @param y Y-Coordinate
		 */
		void position(int,int);

		BEGIN_EVENTS(GameWindow);
		EVENT(evt_position, (*this)(ix::x)(cadence::doste::modifiers::Seq)(*this)(ix::y));
		EVENT(evt_size, (*this)(ix::width)(cadence::doste::modifiers::Seq)(*this)(ix::height));
		EVENT(evt_title, (*this)(ix::title));
		EVENT(evt_draw, (*this)("draw"));
		//EVENT(evt_end, (*this)("enddraw"));
		END_EVENTS;

		#ifdef WIN32
		HDC getHDC() { return m_hDC; };
		HWND getHWND() { return m_hWnd; };
		#endif

		#ifdef LINUX
		Display *getXDisplay() { return m_display; };
		Window getXWindow() { return m_window; };
		int getXScreen() { return m_screen; };
		#endif
		
		void draw();

		private:

		bool m_init;
		bool m_makecontext;
		
		//buffer height as it is used a lot
		int m_height;
		
		#ifdef LINUX
		Display *m_display;
		XVisualInfo *m_visual;
		Colormap m_colormap;
		XSetWindowAttributes m_swa;
		GLXContext m_context;
		Window m_window;
		XF86VidModeModeInfo m_deskMode;
		int m_screen;
		#endif

		#ifdef WIN32
		HWND m_hWnd;
		HDC m_hDC;
		HGLRC m_hRC;
		HINSTANCE m_hInst;
		bool m_hasmultisample;
		int m_pixformat;
		#endif

		void make();
		void finaliseInternal();
		void defaults();
		//bool validRes();
	};

	extern BASEIMPORT GameWindow *window;
};

#endif
