#ifndef _WGD_WIIMOTE_
#define _WGD_WIIMOTE_

#include <wgd/vector.h>
#include <cadence/agent.h>
#include <list>

#include <wgd/dll.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifndef NO_BLUETOOTH
#ifdef LINUX
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#endif

#ifdef WIN32
#define near
#define far
#include <wgd/bluetooth/hidsdi.h>
#ifndef _MSC_VER
#include <wgd/bluetooth/ws2bth.h>
#else
#include <ws2bth.h>
#endif
#include <wgd/bluetooth/BluetoothAPIs.h>
#include <queue> //queue for the thread
#pragma comment ( lib, "irprops.lib")
#pragma comment ( lib, "setupapi.lib")
//#pragma comment ( lib, "hid.lib")
#endif
#endif

/* button codes */
#define WIIMOTE_BUTTON_TWO				0x0001
#define WIIMOTE_BUTTON_ONE				0x0002
#define WIIMOTE_BUTTON_B				0x0004
#define WIIMOTE_BUTTON_A				0x0008
#define WIIMOTE_BUTTON_MINUS			0x0010
#define WIIMOTE_BUTTON_ZACCEL_BIT6		0x0020
#define WIIMOTE_BUTTON_ZACCEL_BIT7		0x0040
#define WIIMOTE_BUTTON_HOME				0x0080
#define WIIMOTE_BUTTON_LEFT				0x0100
#define WIIMOTE_BUTTON_RIGHT			0x0200
#define WIIMOTE_BUTTON_DOWN				0x0400
#define WIIMOTE_BUTTON_UP				0x0800
#define WIIMOTE_BUTTON_PLUS				0x1000
#define WIIMOTE_BUTTON_ZACCEL_BIT4		0x2000
#define WIIMOTE_BUTTON_ZACCEL_BIT5		0x4000
#define WIIMOTE_BUTTON_UNKNOWN			0x8000

/* nunchul button codes */
#define NUNCHUK_BUTTON_Z				0x01
#define NUNCHUK_BUTTON_C				0x02

/* classic controller button codes */
#define CLASSIC_CTRL_BUTTON_UP			0x0001
#define CLASSIC_CTRL_BUTTON_LEFT		0x0002
#define CLASSIC_CTRL_BUTTON_ZR			0x0004
#define CLASSIC_CTRL_BUTTON_X			0x0008
#define CLASSIC_CTRL_BUTTON_A			0x0010
#define CLASSIC_CTRL_BUTTON_Y			0x0020
#define CLASSIC_CTRL_BUTTON_B			0x0040
#define CLASSIC_CTRL_BUTTON_ZL			0x0080
#define CLASSIC_CTRL_BUTTON_FULL_R		0x0200
#define CLASSIC_CTRL_BUTTON_PLUS		0x0400
#define CLASSIC_CTRL_BUTTON_HOME		0x0800
#define CLASSIC_CTRL_BUTTON_MINUS		0x1000
#define CLASSIC_CTRL_BUTTON_FULL_L		0x2000
#define CLASSIC_CTRL_BUTTON_DOWN		0x4000
#define CLASSIC_CTRL_BUTTON_RIGHT		0x8000

/* wiimote option flags */
#define WIIMOTE_USE_SMOOTHING			0x01
#define WIIMOTE_INIT_FLAGS			WIIMOTE_USE_SMOOTHING

struct WiiEvent;

struct WiiReadReq {
	unsigned int addr;
	unsigned short length;
	char *data;
};

#ifdef WIN32
	#ifndef NO_BLUETOOTH
	struct HIDDevice{
		HANDLE hDevice;
		HANDLE hWrite;
		HANDLE hRead;
		HANDLE event;
		HIDP_CAPS caps;
		OVERLAPPED overlapped;
	};
	#endif
#endif

namespace wgd {

	/**
	 * Wiimote input support. Currently this only works in Linux where you can have
	 * a maximum of 4 wiimotes connected at any time. It behaves exactly like a joystick
	 * and can be assigned to controls as if it where one. The axes are calibrated so that
	 * a value of 1 or -1 is the maxium g in a rest state and 0 is the g when flat on the table.
	 */
	class INPUTIMPORT Wiimote : public cadence::Agent {

		public:
		
		VOBJECT(Agent, Wiimote);
		
		#ifdef LINUX
		#ifndef NO_BLUETOOTH
		Wiimote(bdaddr_t id, const cadence::doste::OID &obj);
		#else
		Wiimote(int id, const cadence::doste::OID &obj);
		#endif
		#endif
		#ifdef WIN32
		#ifdef NO_BLUETOOTH
		Wiimote(int id, const cadence::doste::OID &obj);
		#else
		Wiimote(HIDDevice id, const cadence::doste::OID &obj);
		#endif
		#endif
		~Wiimote();

		/**
		 * Is a specific button pressed. See BUTTON_ constants for
		 * possible button values.
		 * @param btn Check this button
		 * @return True if the button is being pressed.
		 */
		bool btnPressed(const cadence::doste::OID &btn) const {
			return get(ix::buttons)[btn];
		};

		/**
		 * Check if an LED is enabled.
		 * @param num LED number, 0 to 3.
		 * @return True if the LED is on.
		 */
		bool led(int num) const {
			return get("leds")[num];
		};
		/**
		 * Turn an LED on or off.
		 * @param num LED number, 0 to 3.
		 * @param on True to turn the LED on.
		 */
		void led(int num, bool on) {
			get("leds")[num] = on;
		};

		/**
		 * Enable or disable rumble.
		 * @param v True to enable rumble.
		 */
		 PROPERTY_WF(bool, rumble, "rumble");

		/**
		 * Is rumble enabled. */
		 PROPERTY_RF(bool, rumble, "rumble");

		/**
		 * Get the value of a specific axis.
		 * @param num Axis number, 0-2.
		 * @return Value of axis.
		 */
		float axis(int num) const {
			return get(ix::axes)[num][ix::value];
		}

		/**
		 * Enable or disable the infrared sensor.
		 * @param e True to enable.
		 */
		PROPERTY_WF(bool, irEnabled, "ir");

		/** Is the IR sensor enabled. */
		PROPERTY_RF(bool, irEnabled, "ir");

		/**
		 * The number of IR dots detected, 0 to 4.
		 * @return Number of dots.
		 */
		int irNumber() {

			if (m_dots[3].found) return 4;
			if (m_dots[2].found) return 3;
			if (m_dots[1].found) return 2;
			if (m_dots[0].found) return 1;
			return 0;
		};

		/**
		 * Get an IR dot X coordinate. These coordinates are normalised between
		 * 0.0 and 1.0.
		 * @param num Dot number.
		 * @return Normalised X-Coordinate of dot.
		 */
		float irX(int num) {
			if (num < 0 || num >= 4) return 0.0;
			return m_dots[num].x;
		};

		/**
		 * Get an IR dot X coordinate. These coordinates are normalised between
		 * 0.0 and 1.0.
		 * @param num Dot number.
		 * @return Normalised Y-Coordinate of dot.
		 */
		float irY(int num) {
			if (num < 0 || num >= 4) return 0.0;
			return m_dots[num].y;
		};

		static const unsigned char LED_NONE = 0x00;
		static const unsigned char LED_1 = 0x10;
		static const unsigned char LED_2 = 0x20;
		static const unsigned char LED_3 = 0x40;
		static const unsigned char LED_4 = 0x80;

		static cadence::doste::OID BUTTON_LEFT; 	/**< Left button. */
		static cadence::doste::OID BUTTON_RIGHT; /**< Right button. */
		static cadence::doste::OID BUTTON_UP; 	/**< Up button. */
		static cadence::doste::OID BUTTON_DOWN; 	/**< Down button. */
		static cadence::doste::OID BUTTON_ONE; 	/**< One button. */
		static cadence::doste::OID BUTTON_TWO; 	/**< Two button. */
		static cadence::doste::OID BUTTON_MINUS; /**< Minus button. */
		static cadence::doste::OID BUTTON_HOME; 	/**< Home button. */
		static cadence::doste::OID BUTTON_PLUS; 	/**< Plus button. */
		static cadence::doste::OID BUTTON_A; 	/**< A button. */
		static cadence::doste::OID BUTTON_B; 	/**< B button. */
		static cadence::doste::OID BUTTON_Z;	/**< Z button. Nunchuk */
		static cadence::doste::OID BUTTON_C;	/**< C button. Nunchuk */

		/**
		 * Call this first to detect wiimotes. This must be done before you can use
		 * a wiimote. Note that it is slow as it has to do a bluetooth inquiry.
		 */
		static void find();
		/**
		 * Number of Wiimotes detected.
		 */
		static int number() { return s_nummotes; };
		/**
		 * Get a specified Wiimote.
		 * @param num Wiimote number, 0 to number().
		 * @return The wiimote object.
		 */
		static Wiimote *wiimote(int num) {
			if (num < 0 || num > s_nummotes) return 0;
			else return s_motes[num];
		};
		
		static void initialise();
		static void finalise();
		static void updateAll();

		BEGIN_EVENTS(Agent);
		EVENT(evt_leds, (*this)("leds")(cadence::doste::All));
		EVENT(evt_rumble, (*this)("rumble"));
		EVENT(evt_ir, (*this)("ir"));
		END_EVENTS;
		
		private:

		void connect_wii();
		void disconnect_wii();
		void handshake_wii();

		void status();
		void reportType(bool motion, bool expansion);
		void read_data(unsigned int addr, char *data, int length);
		void write_data(unsigned int addr, char *data, int length);
		void send_wii(unsigned char cmd, char *msg, int length);
		bool poll_wii(WiiEvent &evt);
		void postRequest(const WiiReadReq &req);
		void handleRead();
		void update();
		void gforce();
		void orientation();
		void flashWii(const char *filename);
		void dumpFlash(const char *filename);

		void leds(unsigned char leds);

		#ifndef NO_BLUETOOTH
		#ifdef LINUX
		int m_insock;
		int m_outsock;
		bdaddr_t m_addr;
		#endif
		#ifdef WIN32
		//SOCKET m_insock;
		//SOCKET m_outsock;
		HIDDevice m_addr;
		
		//callback function for the thread
		static unsigned CALLBACK listener(void * param);
		unsigned int m_threadID;
		HANDLE m_thread;
		bool listening;
		std::queue<WiiEvent*> msgs;
		CRITICAL_SECTION m_lock;
		#endif
		#else
		int m_addr;
		#endif
		unsigned char m_leds;
		bool m_rumble;
		unsigned char m_calib[8];
		unsigned char m_nc_calib[14];
		std::list<WiiReadReq> m_requests;
		vector3d m_accel;
		vector3d m_calg;
		vector3d m_calzero;
		vector3d m_gforce;
		vector3d m_orient;
		vector3d m_nc_accel;
		vector3d m_nc_calg;
		vector3d m_nc_calzero;
		vector3d m_nc_gforce;
		vector3d m_nc_orient;
		vector2d m_js_max;
		vector2d m_js_min;
		vector2d m_js_centre;
		vector2d m_js;
		bool m_nchuk;

		struct Dots {
			bool found;
			int rawX;
			int rawY;
			float x;
			float y;
		} m_dots[4];

		static const unsigned char CMD_LEDS = 0x11;
		static const unsigned char CMD_REPORT_TYPE = 0x12;
		static const unsigned char CMD_RUMBLE = 0x13;
		static const unsigned char CMD_CTRL_STATUS = 0x15;
		static const unsigned char CMD_READ_DATA = 0x17;
		static const unsigned char CMD_WRITE_DATA = 0x16;
		static const unsigned char CMD_IR = 0x13;
		static const unsigned char CMD_IR2 = 0x1a;
		static const unsigned char CMD_SPEAKER_ENABLE = 0x14;
		static const unsigned char CMD_SPEAKER_DATA = 0x18;
		static const unsigned char CMD_SPEAKER_MUTE = 0x19;

		static const unsigned char REPORT_CTRL_STATUS = 0x20;
		static const unsigned char REPORT_READ = 0x21;
		static const unsigned char REPORT_WRITE = 0x22;
		static const unsigned char REPORT_BTN = 0x30;
		static const unsigned char REPORT_BTN_MOTION = 0x31;
		static const unsigned char REPORT_BTN_MOTION_IR = 0x33;
		static const unsigned char REPORT_BTN_EXP = 0x36;
		static const unsigned char REPORT_BTN_MOTION_EXP = 0x37;

		static const unsigned int MEM_CALIBRATION = 0x16;
		static const unsigned int MEM_EXT_CALIB = 0x04A40020;
		static const unsigned int MEM_EXT_INIT = 0x04A40040;
		static const unsigned int MEM_EXT_TYPE = 0x04A400FE;
		static const unsigned int MEM_IR_SENSITIVITY = 0x04b00000;
		static const unsigned int MEM_IR_SENSITIVITY2 = 0x04b0001a;
		static const unsigned int MEM_IR_MODE = 0x04b00033;
		static const unsigned int MEM_IR = 0x04b00030;

		static const unsigned char IRMODE_OFF = 0x00;
		static const unsigned char IRMODE_BASIC = 0x01;
		static const unsigned char IRMODE_EXTENDED = 0x03;
		static const unsigned char IRMODE_FULL = 0x05;

		static int s_nummotes;
		static Wiimote *s_motes[4];
		
	};
};

#endif
