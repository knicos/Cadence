#ifndef _WGD_JOYSTICK_
#define _WGD_JOYSTICK_

#ifdef LINUX
#include <linux/input.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#include <regstr.h>
#endif

#include <cadence/object.h>
#include <wgd/dll.h>
#include <wgd/index.h>

#define MAX_JOYSTICKS 6

namespace wgd {
	typedef cadence::doste::OID Hat;
	
	/**
	 * Provides joystick, gamepad and XBox 360 controller input. You can query button
	 * state and the current position of each axis. 
	 */
	class INPUTIMPORT Joystick : public cadence::Object {
		public:
		
		VOBJECT(cadence::Object, Joystick);
		
		/** Query a particular button. */
		bool btnPressed(int btn);
		/** Get the value of an axis. */
		float axis(int ax);
		
		/** set an axis to be inverted */
		void inverted(int ax, bool invert);
		/** get wherter an axis is inverted */
		bool inverted(int ax);
		
		Hat hat(int num=0);
		/** Return the joystick name. */
		const char *name();
		/** The number of axes available on this joystick */
		int numAxes() const { return m_numaxis; };
		/** The number of buttons. */
		int numButtons() const { return m_numbuts; };
		
		/** Number of joysticks. */
		static int number() { return s_numjs; };
		/** Get a specific joystick, 0 to number()-1. */
		static Joystick *joystick(int num);
		
		/* this seems to be correct for Microsoft joysticks */
		static const int BUTTON_PRIMARY = 0;
		static const int BUTTON_SECONDARY = 1;
		static const int AXIS_X = 0;
		static const int AXIS_Y = 1;
		static const int AXIS_Z = 2;
		static const int AXIS_R = 3;
		static const int AXIS_U = 4;
		static const int AXIS_V = 5;
		
		static void initialise();
		static void finalise();
		static void updateAll();
		
		
		private:
		int m_numbuts;
		int m_numaxis;
		char m_name[80];
		
		#ifdef WIN32
		int m_id;
		JOYCAPS m_joycaps;
		Joystick(const cadence::doste::OID obj, int id, const JOYCAPS &joycaps);	
		#else
		int m_fp;
		char m_devname[100];
		unsigned char m_keymap[KEY_MAX - BTN_MISC];
    		unsigned char m_absmap[ABS_MAX];
		Joystick(const cadence::doste::OID &obj, int fp, const char *devname);
		#endif
		
		Joystick();
		~Joystick();
		
		void update();
		
		static int s_numjs;
		static Joystick *s_jsticks[MAX_JOYSTICKS];
	};
};

#endif
