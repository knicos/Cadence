/*
 * WGD Library
 *
 * Authors: 
 * Date: 23/9/2007
 *
 */

#ifndef _WGD_MOUSE_
#define _WGD_MOUSE_

#include <wgd/index.h>
#include <cadence/doste/definition.h>
#include <cadence/agent.h>
#include <cadence/doste/modifiers.h>
#include <wgd/dll.h>
#include <wgd/vector.h>

namespace wgd {
	/** Represents mouse buttons */
	typedef cadence::doste::OID Button;

	/**
	 * Provides access to mouse events and position. 
	 */
	class BASEIMPORT Mouse : public cadence::Agent {
		friend class Input;
		public:

		OBJECT(Agent, Mouse);

		Mouse(const cadence::doste::OID &o);
		~Mouse();

		/**
		 * Make the mouse visible or invisible. You cannot move the mouse out of the
		 * window if the mouse is invisible as it is moved back to the center each frame.
		 */
		PROPERTY_WF(bool, visible, ix::visible);
		/** Is the mouse visible. */
		PROPERTY_RF(bool, visible, ix::visible);

		/** Grab the mouse. Brings it back to the centre of the screen each frame. */
		PROPERTY_WF(bool, grab, ix::grab);
		PROPERTY_RF(bool, grab, ix::grab);

		/**
		 * Get mouse X position. Note that this is not useful when the mouse is not visible.
		 */
		PROPERTY_RF(int, x, ix::x);
		/**
		 * Get mouse Y position. Note that this is not useful when the mouse is not visible.
		 */
		PROPERTY_RF(int, y, ix::y);
		
		/** Get the cursor position on screen */
		wgd::vector2d position();
		/** Set the cursor position */
		void position(int x, int y);
		/** Set the cursor position */
		void position(const wgd::vector2d &);
		
		
		/**
		 * Change in X since last frame.
		 */
		PROPERTY_RF(int, deltaX, ix::deltax);
		/**
		 * Change in Y since last frame.
		 */
		PROPERTY_RF(int, deltaY, ix::deltay);
		/** Get change in mouse wheel since last update */
		PROPERTY_RF(int, deltaZ, ix::deltaz);

		/** Used by Controls */
		cadence::doste::OID buttons() { return get(ix::buttons); };
		/** Used by Controls */
		cadence::doste::OID axes() { return get(ix::axes); };

		/**
		 * Query if a mouse button is currently pressed.
		 */
		bool btnPressed(const Button &b) { return (*this)[ix::buttons][b]; };
		/// Same as btnPressed
		bool pressed(const Button &b) const { return (*this)[ix::buttons][b]; };
		
		// Extra functions for non-event based usage
		/// has a button been clicked in the last frame (pressed and released without moving the mouse)
		bool click(const Button &button) const		{ return buttonUp(button) && !m_moved[bNo(button)]; }
		/// are you dragging with a mouse button pressed
		bool drag(const Button &button)	const		{ return pressed(button) && m_moved[bNo(button)]; }
		/// Was the button released in tha last frame
		bool buttonUp(const Button &button) const	{ return !pressed(button) && m_last[bNo(button)]; }
		/// Was the button pressed in the last frame
		bool buttonDown(const Button &button) const	{ return pressed(button) && !m_last[bNo(button)]; }
		
		
		BEGIN_EVENTS(Agent);
		EVENT(evt_cursor, (*this)(ix::cursor));
		EVENT(evt_visible, (*this)(ix::visible));
		EVENT(evt_position, (*this)(ix::x)(cadence::doste::modifiers::Seq)(*this)(ix::y));
		END_EVENTS;

		static Button BTN_LEFT;		/**< Left mouse button */
		static Button BTN_MIDDLE;	/**< Middle mouse button. Aka Scroll wheel button */
		static Button BTN_RIGHT;	/**< Right mouse button. */
		//static Button BTN_A;
		//static Button BTN_B;
		//static Button BTN_C;
		//static Button BTN_D;

		static const int AXIS_X = 0;
		static const int AXIS_Y = 1;
		//static cadence::OID AXIS_Z;
		
		void reset();
		
		private:
		int bNo(const Button&) const;
		bool m_last[6];		//state of buttons last frame
		bool m_moved[6];	//has the mouse been moved since the button was pressed
		
		void btnDown(int);
		void btnUp(int);
		void wheel(int d) { s_wheel += d; }
		void update();

		
		static int lastX;
		static int lastY;
		static bool s_vis;
		static int s_wheel;
	};

	extern BASEIMPORT Mouse *mouse;
};

#endif
