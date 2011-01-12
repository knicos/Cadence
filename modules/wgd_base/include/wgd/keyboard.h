/*
 * WGD Library
 *
 * Authors: 
 * Date: 17/9/2007
 *
 */

#ifndef _WGD_KEYBOARD_
#define _WGD_KEYBOARD_

#include <cadence/agent.h>
#include <wgd/index.h>
#include <wgd/dll.h>
#include <map>

namespace wgd {

	/** Keyboard key type. */
	typedef cadence::doste::OID Key;

	/**
	 * Provides access to keyboard input.
	 */
	class BASEIMPORT Keyboard : public cadence::Agent {
		friend class Input;
		public:
		OBJECT(Agent,Keyboard);

		Keyboard(const cadence::doste::OID &o);
		~Keyboard();
		
		cadence::doste::OID keys() const { return (*this)[ix::keys]; }
		
		/// get whether a key is pressed
		bool keyDown(const Key &key) const { return keys()[key]; }
		/// get whether a key was pressed in the last frame
		bool keyPressed(const Key &key) { return m_pressed[key]; }


		static Key KEY_LEFT;	/**< Left Arrow */
		static Key KEY_UP;	/**< Up Arrow */
		static Key KEY_RIGHT;	/**< Right Arrow */
		static Key KEY_DOWN;	/**< Down Arrow */
		static Key KEY_RETURN;	/**< Main return */
		static Key KEY_ENTER;	/**< Numpad Enter */
		static Key KEY_SPACE;	/**< Space, same as ' ' */
		static Key KEY_CTRL;	/**< Left Ctrl */
		static Key KEY_SHIFT;	/**< Left Shift */
		static Key KEY_RCTRL;	/**< Right Ctrl */
		static Key KEY_RSHIFT;	/**< Right Shift */
		static Key KEY_ALT;	/**< Alt */
		static Key KEY_ALTGR;	/**< Alt Gr */
		static Key KEY_DEL;	/**< Delete */
		static Key KEY_BKSPACE;	/**< Backspace */
		static Key KEY_END;	/**< End */
		static Key KEY_PGDOWN;	/**< Page Down */
		static Key KEY_PGUP;	/**< Page Up */
		static Key KEY_HOME;	/**< Home */
		static Key KEY_INSERT;	/**< Insert */
		static Key KEY_PAUSE;	/**< Pause */
		static Key KEY_TAB;	/**< Tab */
		static Key KEY_ESCAPE;	/**< Escape */
		static Key KEY_F1;	/**< F1 */
		static Key KEY_F2;	/**< F2 */
		static Key KEY_F3;	/**< F3 */
		static Key KEY_F4;	/**< F4 */
		static Key KEY_F5;	/**< F5 */
		static Key KEY_F6;	/**< F6 */
		static Key KEY_F7;	/**< F7 */
		static Key KEY_F8;	/**< F8 */
		static Key KEY_F9;	/**< F9 */
		static Key KEY_F10;	/**< F10 */
		static Key KEY_F11;	/**< F11 */
		static Key KEY_F12;	/**< F12 */

		private:
		void update() { m_pressed.clear(); }
		void keyPress(char key);
		void keyRelease(char key);
		
		
		std::map<Key, bool> m_pressed;
		char ascii(const Key &key, bool shift);
		Key convert(char key);
	};

	extern BASEIMPORT Keyboard *keyboard;
};

#endif
