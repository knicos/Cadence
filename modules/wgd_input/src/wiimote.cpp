#include <wgd/wiimote.h>
#include <wgd/index.h>
#include <wgd/common.h>

#ifdef LINUX
#include <arpa/inet.h>
#include <unistd.h>
#endif

#ifndef NO_BLUETOOTH
#ifdef LINUX
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#endif

#ifdef WIN32
#include <wgd/bluetooth/bthsdpdef.h>
//#include <wgd/bluetooth/BluetoothAPIs.h>
//#include <wgd/bluetooth/hidsdi.h>
#include <setupapi.h>
#include <process.h>
#endif

extern cadence::doste::OID inputbase;

#ifdef _MSC_VER
#pragma warning( disable : 4996 )

BOOL (WINAPI *HidD_GetAttributes) (HANDLE, PHIDD_ATTRIBUTES);
VOID (WINAPI *HidD_GetHidGuid) (LPGUID);
BOOL (WINAPI *HidD_GetPreparsedData)(HANDLE, PHIDP_PREPARSED_DATA  *);
BOOL (WINAPI *HidD_FreePreparsedData)(PHIDP_PREPARSED_DATA);
NTSTATUS (WINAPI *HidP_GetCaps)(PHIDP_PREPARSED_DATA  PreparsedData, PHIDP_CAPS  Capabilities);

#endif
#endif

#define WIIMOTE_DEV_CLASS_0			0x04
#define WIIMOTE_DEV_CLASS_1			0x25
#define WIIMOTE_DEV_CLASS_2			0x00

#define WIIMOTE_VID	0x057e
#define WIIMOTE_PID	0x0306

#define WIIMOTE_OUTPUT_CHANNEL			0x11
#define WIIMOTE_INPUT_CHANNEL			0x13

#define SET_REPORT					0x50
#define BT_INPUT					0x01
#define BT_OUTPUT					0x02

#define PIEBY2 1.570796f

using namespace wgd;

int wgd::Wiimote::s_nummotes = 0;
Wiimote *wgd::Wiimote::s_motes[4];
cadence::doste::OID wgd::Wiimote::BUTTON_LEFT;
cadence::doste::OID wgd::Wiimote::BUTTON_RIGHT;
cadence::doste::OID wgd::Wiimote::BUTTON_UP;
cadence::doste::OID wgd::Wiimote::BUTTON_DOWN;
cadence::doste::OID wgd::Wiimote::BUTTON_ONE;
cadence::doste::OID wgd::Wiimote::BUTTON_TWO;
cadence::doste::OID wgd::Wiimote::BUTTON_MINUS;
cadence::doste::OID wgd::Wiimote::BUTTON_HOME;
cadence::doste::OID wgd::Wiimote::BUTTON_PLUS;
cadence::doste::OID wgd::Wiimote::BUTTON_A;
cadence::doste::OID wgd::Wiimote::BUTTON_B;
cadence::doste::OID wgd::Wiimote::BUTTON_C;
cadence::doste::OID wgd::Wiimote::BUTTON_Z;

struct WiiMsg {
	unsigned char proto;
	unsigned char type;
	unsigned char data[21];
} PACKED;

struct WiiEvent {
	unsigned char proto;
	unsigned char type;
	unsigned char data[22];
} PACKED;

struct WiiReadData {
	unsigned int addr;
	unsigned short size;
} PACKED;

struct WiiWriteData {
	unsigned int addr;
	unsigned char size;
	unsigned char data[10];
} PACKED;

namespace wgd {
	OnEvent(Wiimote, evt_leds) {
		int cleds = 0;
		OID obj = get("leds");
		for(int i=0; i<4; i++) {
			if((bool)obj.get(i)) cleds |= (1 << (4+i));
		}
		if(cleds != m_leds) leds(cleds);
		m_leds = cleds;
	}
	
	OnEvent(Wiimote, evt_rumble) {
		if ((bool)((*this)["rumble"]) && !m_rumble) {
			m_rumble = true;
			char buf = m_leds | 0x01;
			send_wii(CMD_RUMBLE, &buf, 1);
		} else if ((bool)((*this)["rumble"]) == false && m_rumble) {
			m_rumble = false;
			char buf = m_leds & 0xF0;
			send_wii(CMD_RUMBLE, &buf, 1);
		}
	}
	
	OnEvent(Wiimote, evt_ir) {
		if (irEnabled()) {
			//Enable the IR sensor.
			char buf = 0x04 | ((m_rumble) ? 0x01 : 0x00);
			send_wii(CMD_IR, &buf, 1);
			#ifdef WIN32
			Sleep(100);
			#else
			usleep(100000);
			#endif
			send_wii(CMD_IR2, &buf, 1);
			#ifdef WIN32
			Sleep(100);
			#else
			usleep(100000);
			#endif
	
			//Set the IR sensitivity;
			unsigned char ir_sens1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x00, 0xc0};
			unsigned char ir_sens2[] = {0x40, 0x00};
	
			buf = 0x08;
			write_data(MEM_IR, &buf, 1);
			#ifdef WIN32
			Sleep(100);
			#else
			usleep(100000);
			#endif
			write_data(MEM_IR_SENSITIVITY, (char*)ir_sens1, sizeof(ir_sens1));
			#ifdef WIN32
			Sleep(100);
			#else
			usleep(100000);
			#endif
			write_data(MEM_IR_SENSITIVITY2, (char*)ir_sens2, sizeof(ir_sens2));
			#ifdef WIN32
			Sleep(100);
			#else
			usleep(100000);
			#endif
			buf = IRMODE_EXTENDED;
			write_data(MEM_IR_MODE, &buf, 1);
	
			std::cout << "IR ENABLED\n";
			//wgd::cout << wgd::DEV << "IR Enabled...\n";
		} else {
			//wgd::cout << wgd::DEV << "IR Disabled...\n";
			//Disable the IR sensor.
			char buf = (m_leds & 0xF0) | ((m_rumble) ? 0x01 : 0x00);
			send_wii(CMD_IR, &buf, 1);
			send_wii(CMD_IR2, &buf, 1);
		}
	}
	
	IMPLEMENT_EVENTS(Wiimote, Agent);
};

#ifdef LINUX
#ifndef NO_BLUETOOTH
wgd::Wiimote::Wiimote(bdaddr_t id, const cadence::doste::OID &obj)
#else
wgd::Wiimote::Wiimote(int id, const cadence::doste::OID &obj)
#endif
#endif
#ifdef WIN32
#ifndef NO_BLUETOOTH
wgd::Wiimote::Wiimote(HIDDevice id, const cadence::doste::OID &obj)
#else
wgd::Wiimote::Wiimote(int id, const cadence::doste::OID &obj)
#endif
#endif
	: Agent(obj), m_addr(id), m_nchuk(false) {

	if (get("leds") == Null) 	set("leds",	OID::create());
	if (get("buttons") == Null)	set("buttons",	OID::create());
	if (get("axes") == Null)	set("axes",	OID::create());
	
	for(int i=0; i<3; i++) {
		if (get("axes")[i] == Null)			get("axes")[i] = OID::create();
		if (get("axes")[i][ix::scale] == Null)		get("axes")[i][ix::scale] = 1.0f;
		if (get("axes")[i][ix::deadzone] == Null)	get("axes")[i][ix::deadzone] =  0.001f;
	}

	if (get("dots") == Null)	set("dots", OID::create());
	if (get("dots")[0] == Null)	get("dots")[0] = OID::create();
	if (get("dots")[1] == Null)	get("dots")[1] = OID::create();
	if (get("dots")[2] == Null)	get("dots")[2] = OID::create();
	if (get("dots")[3] == Null)	get("dots")[3] = OID::create();


	irEnabled(true);
	
	//initialise rumble
	m_rumble = false;
	connect_wii();
	rumble(false);
	updateAll();

	registerEvents();
}

wgd::Wiimote::~Wiimote() {
	disconnect_wii();
}

void wgd::Wiimote::connect_wii() {

	#ifndef NO_BLUETOOTH
	#ifdef LINUX
	sockaddr_l2 addr;
	addr.l2_family = AF_BLUETOOTH;
	addr.l2_bdaddr = m_addr;
	m_outsock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (m_outsock == -1) {
		//wgd::cout << wgd::ERR << "Could not connect to wiimote.\n";
		return;
	}

	addr.l2_psm = htobs(WIIMOTE_OUTPUT_CHANNEL);

	if (connect(m_outsock, (sockaddr*)&addr, sizeof(addr)) < 0) {
		//wgd::cout << wgd::ERR << "Could not connect to wiimote.\n";
		return;
	}

	m_insock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (m_insock == -1) {
		//wgd::cout << wgd::ERR << "Could not connect to wiimote.\n";
		return;
	}

	addr.l2_psm = htobs(WIIMOTE_INPUT_CHANNEL);

	if (connect(m_insock, (sockaddr*)&addr, sizeof(addr)) < 0) {
		//wgd::cout << wgd::ERR << "Could not connect to wiimote.\n";
		return;
	}

	#endif
	
	#ifdef WIN32
	//start the listen thread
	listening=true;
	m_thread = (HANDLE)_beginthreadex(0, 0, listener, this, 0, &m_threadID); 
	InitializeCriticalSection(&m_lock);
	//wgd::cout << wgd::DEV << "Wiimote listen thread started (" << m_threadID << ")\n";
	#endif
	#endif

	//We want motion data but no expansion data.
	reportType(true,false);
	handshake_wii();
	leds(LED_NONE);
}

void wgd::Wiimote::handshake_wii() {
	read_data(MEM_CALIBRATION, (char*)m_calib, 8);
	//WiiEvent evt;

	/*while (true) {
		poll_wii(evt);

		if (evt.type == REPORT_READ) {
			int err = evt.data[0] & 0x0F;
			if (err == 0x08) {
				wgd::cout << wgd::ERR << "Wiimote read error: Address does not exist.\n";
				return;
			} else if (err == 0x07) {
				wgd::cout << wgd::ERR << "Wiimote read error: Address is write only.\n";
				return;
			} else if (err) {
				wgd::cout << wgd::ERR << "Wiimote read error: unknown\n";
				return;
			}

			int len = ((evt.data[0] & 0xF0) >> 4) + 1;
			//if (len < 8) { 
				//wgd::cout << wgd::ERR << "Invalid wiimote calibration data.\n";
				//continue;
			//}

			wgd::cout << wgd::DEV << "Reading wiimote calibration...\n";
			m_calzero.x = evt.data[1];
			m_calzero.y = evt.data[2];
			m_calzero.z = evt.data[3];
			m_calg.x = evt.data[5];
			m_calg.y = evt.data[6];
			m_calg.z = evt.data[7];
	
			wgd::cout << wgd::DEV << "Zero: " << m_calzero.x << "," << m_calzero.y << "," << m_calzero.z << "\n";
			wgd::cout << wgd::DEV << "G: " << m_calg.x << "," << m_calg.y << "," << m_calg.z << "\n";
			
			break;
		} else {
			continue;
		}
	}*/

	//Get calibration data...
}

void wgd::Wiimote::leds(unsigned char pleds) {
	m_leds = pleds & 0xF0;
	//Keep rumble on...
	if (m_rumble)
		pleds |= 0x01;
	send_wii(CMD_LEDS, (char*)&pleds, 1);
}

void wgd::Wiimote::postRequest(const WiiReadReq &req) {
	//Add to request queue
	m_requests.push_back(req);
	//If queue was empty then send immediately.
	if (m_requests.size() == 1) {
		WiiReadData msg;
		msg.addr = htonl(req.addr);
		msg.size = htons(req.length);
		send_wii(CMD_READ_DATA, (char*)&msg, sizeof(WiiReadData));
	}
}

void wgd::Wiimote::write_data(unsigned int addr, char *data, int length) {
	//WiiWriteData msg;
	//msg.addr =htonl(addr);
	//msg.size = length;
	//memcpy(&msg.data, data, length);

	char msg[21] = {0};
	msg[0] = ((addr & 0xff000000) >> 24) ;	//Or with rumble bit.
	msg[1] = (addr & 0xff0000) >> 16;
	msg[2] = (addr & 0xff00) >> 8;
	msg[3] = (addr & 0xff);
	msg[4] = (char)length;
	memcpy(msg+5, data, length);

	//wgd::cout << "write_data: " << (int)msg[0] << " " << (int)msg[1] << " " << (int)msg[2] << " " << (int)msg[3] << "\n";

	//return;

	send_wii(CMD_WRITE_DATA, (char*)&msg, 21);
}

void wgd::Wiimote::read_data(unsigned int addr, char *data, int length) {

	//Make a read request and post it.
	WiiReadReq req;
	req.addr = addr;
	req.data = data;
	req.length = length;
	postRequest(req);

	/*WiiEvent evt;
	int len;

	*/
}

void wgd::Wiimote::handleRead() {
	unsigned char *buf = (unsigned char*)m_requests.front().data;
	if (m_requests.front().addr == MEM_CALIBRATION) {
		//wgd::cout << wgd::DEV << "Reading wiimote calibration...\n";
		m_calzero.x = buf[0];
		m_calzero.y = buf[1];
		m_calzero.z = buf[2];
		m_calg.x = buf[4];
		m_calg.y = buf[5];
		m_calg.z = buf[6];

		//wgd::cout << wgd::DEV << "Zero: " << m_calzero.x << "," << m_calzero.y << "," << m_calzero.z << "\n";
		//wgd::cout << wgd::DEV << "G: " << m_calg.x << "," << m_calg.y << "," << m_calg.z << "\n";
	} else if (m_requests.front().addr == MEM_EXT_CALIB) {
		//wgd::cout << wgd::DEV << "Reading wiimote nunchuck calibration...\n";

		//DECRYPT
		for (int i=0; i<14; i++) {
			buf[i] = (buf[i] ^ 0x17) + 0x17;
		}

		m_nc_calzero.x = buf[0];
		m_nc_calzero.y = buf[1];
		m_nc_calzero.z = buf[2];
		m_nc_calg.x = buf[4];
		m_nc_calg.y = buf[5];
		m_nc_calg.z = buf[6];
		m_js_max.x = buf[8];
		m_js_min.x = buf[9];
		m_js_centre.x = buf[10];
		m_js_max.y = buf[11];
		m_js_min.y = buf[12];
		m_js_centre.y = buf[13];
	}
}

void wgd::Wiimote::gforce() {
	float xg = m_calg.x - m_calzero.x;
	float yg = m_calg.y - m_calzero.y;
	float zg = m_calg.z - m_calzero.z;

	m_gforce.x = (m_accel.x - m_calzero.x) / xg;
	m_gforce.y = (m_accel.y - m_calzero.y) / yg;
	m_gforce.z = (m_accel.z - m_calzero.z) / zg;

	//NUNCHUK
	xg = m_nc_calg.x - m_nc_calzero.x;
	yg = m_nc_calg.y - m_nc_calzero.y;
	zg = m_nc_calg.z - m_nc_calzero.z;

	m_nc_gforce.x = (m_nc_accel.x - m_nc_calzero.x) / xg;
	m_nc_gforce.y = (m_nc_accel.y - m_nc_calzero.y) / yg;
	m_nc_gforce.z = (m_nc_accel.z - m_nc_calzero.z) / zg;
}

void wgd::Wiimote::orientation() {
	float x,y,z;

	x = m_gforce.x;
	y = m_gforce.y;
	z = m_gforce.z;

	x -= 1.0f;
	x *= 5.0f;
	//m_orient.x = x;

	x *= (float)get(ix::axes)[0][ix::scale];
	if((bool)get(ix::axes)[0][ix::invert])	x = -x;

	if(fabs(x) < (float)get(ix::axes)[0][ix::deadzone]) {
		get(ix::axes)[0][ix::value] = 0.0f;
	} else {
		get(ix::axes)[0][ix::value] = x;
	}

	y -= 1.0f;
	y *= 0.6666f;
	//m_orient.y = y;

	y *= (float)get(ix::axes)[1][ix::scale];
	if((bool)get(ix::axes)[1][ix::invert]) y = -y;

	if(fabs(y) < (float)get(ix::axes)[1][ix::deadzone]) {
		get(ix::axes)[1][ix::value] = 0.0f;
	} else {
		get(ix::axes)[1][ix::value] = y;
	}

	z -= 1.0f;
	z *= 5.0f;
	//m_orient.x = x;

	z *= (float)get(ix::axes)[2][ix::scale];
	if((bool)get(ix::axes)[2][ix::invert]) z = -z;

	if(fabs(z) < (float)get(ix::axes)[2][ix::deadzone]) {
		get(ix::axes)[2][ix::value] = 0.0f;
	} else {
		get(ix::axes)[2][ix::value] = z;
	}

	//NUNCHUK

	x = m_nc_gforce.x;
	y = m_nc_gforce.y;
	z = m_nc_gforce.z;

	x -= 1.0f;
	x *= 5.0f;
	//m_orient.x = x;

	x *= (float)get(ix::axes)[3][ix::scale];
	if ((bool)get(ix::axes)[3][ix::invert])
		x = -x;

	if (fabs(x) < (float)get(ix::axes)[3][ix::deadzone]) {
		get(ix::axes)[3][ix::value] = 0.0f;
	} else {
		get(ix::axes)[3][ix::value] = x;
	}

	y -= 1.0f;
	y *= 0.6666f;
	//m_orient.y = y;

	y *= (float)get(ix::axes)[4][ix::scale];
	if ((bool)get(ix::axes)[4][ix::invert])
		y = -y;

	if (fabs(y) < (float)get(ix::axes)[4][ix::deadzone]) {
		get(ix::axes)[4][ix::value] = 0.0f;
	} else {
		get(ix::axes)[4][ix::value] = y;
	}

	z -= 1.0f;
	z *= 5.0f;
	//m_orient.x = x;

	z *= (float)get(ix::axes)[5][ix::scale];
	if ((bool)get(ix::axes)[5][ix::invert])
		z = -z;

	if (fabs(z) < (float)get(ix::axes)[5][ix::deadzone]) {
		get(ix::axes)[5][ix::value] = 0.0f;
	} else {
		get(ix::axes)[5][ix::value] = z;
	}
}

void wgd::Wiimote::update() {
	WiiEvent evt;
	int length;
	char *data;
	//int len;
	short button;
	int err;
	char buf;
	int offs;
	int j;
	float irx,iry,minirx;
	int minirx_index;

	while (poll_wii(evt)) {

		switch(evt.type) {
		case REPORT_CTRL_STATUS:
			//Check for NunChuk
			if (evt.data[2] == 0x2) {
				//wgd::cout << wgd::DEV << "We have a nunchuk.\n";
				buf = 0;
				write_data(0x04A40040, &buf, 1);
				m_nchuk = true;
				reportType(true,m_nchuk);
				read_data(MEM_EXT_CALIB, (char*)m_nc_calib, 14);
			} else {
				m_nchuk = false;
				reportType(true,m_nchuk);
			}
			break;

		case REPORT_READ:
			length = m_requests.front().length;
			data = m_requests.front().data;

			err = evt.data[0] & 0x0F;
			if (err == 0x08) {
				//wgd::cout << wgd::ERR << "Wiimote read error: Address does not exist.\n";
				return;
			} else if (err == 0x07) {
				//wgd::cout << wgd::ERR << "Wiimote read error: Address is write only.\n";
				return;
			} else if (err) {
				//wgd::cout << wgd::ERR << "Wiimote read error: unknown\n";
				return;
			}
		
			//len = ((evt.data[0] & 0xF0) >> 4) + 1;
			//wgd::cout << "READ PACKET." << length << "\n";
			memcpy(data, &evt.data[1], length);
			//length -= len;
			//data += len;
			//m_requests.front().length = length;
			//m_requests.front().data = data;

			//if (length == 0) {
				handleRead();
				m_requests.pop_front();
				if (m_requests.size() > 0) {
					WiiReadData msg;
					msg.addr = htonl(m_requests.front().addr);
					msg.size = htons(m_requests.front().length);
					send_wii(CMD_READ_DATA, (char*)&msg, sizeof(WiiReadData));
				}
			//}
			break;

		//case REPORT_BTN_MOTION_EXP:	//Includes IR
		case REPORT_BTN_MOTION_IR:
			//Process IR information.

			//Assume extended mode, but this may not be the case.
			j = 0;
			irx = 0.0;
			iry = 0.0;
			minirx = 2000.0;
			minirx_index = 0;

			for (int i=0; i<4; i++) {
				m_dots[i].found = false;
				offs = 5+(3*i);
				m_dots[j].found = !(evt.data[offs]==0xff && evt.data[offs+1] == 0xff && evt.data[offs+2]==0xff);

				if (m_dots[j].found) {
					m_dots[j].rawX = evt.data[offs] | ((evt.data[offs+2] >> 4) & 0x03) << 8;
					m_dots[j].rawY = evt.data[offs+1] | ((evt.data[offs+2] >> 6) & 0x03) << 8;
					m_dots[j].x = 1.0f - ((float)m_dots[j].rawX / 1016.0f);
					m_dots[j].y = (float)m_dots[j].rawY / 760.0f;

					//wgd::cout << "IR Dot: " << i << " " << m_dots[i].x << "," << m_dots[i].y << "\n";

					get("dots")[j][ix::x] = m_dots[j].x;
					get("dots")[j][ix::y] = m_dots[j].y;

					if (minirx > m_dots[j].x) {
						minirx_index = j;
						minirx = m_dots[j].x;
					}

					j++;
				}

				irx += m_dots[j].x;
				iry += m_dots[j].y;
			}

			irx /= j;
			iry /= j;
			set("irx", m_dots[minirx_index].x);
			set("iry", m_dots[minirx_index].y);

			//wgd::cout << "EXPANSION\n";
			//DECRYPT
			//for (int i=15; i<22; i++) {
			//	evt.data[i] = (evt.data[i] ^ 0x17) + 0x17;
			//}

			//m_js.x = evt.data[15];
			//m_js.y = evt.data[16];
			//m_nc_accel.x = evt.data[17];
			//m_nc_accel.y = evt.data[18];
			//m_nc_accel.z = evt.data[19];

			//wgd::cout << wgd::DEV << "NC ACCEL X: " << m_nc_accel.x << "\n";

			//button = evt.data[20];
			//if (button & NUNCHUK_BUTTON_Z) get(ix::buttons)[BUTTON_Z] = true; else get(ix::buttons)[BUTTON_Z] = false;
			//if (button & NUNCHUK_BUTTON_C) get(ix::buttons)[BUTTON_C] = true; else get(ix::buttons)[BUTTON_C] = false;

		case REPORT_BTN_MOTION:
			m_accel.x = evt.data[2];
			m_accel.y = evt.data[3];
			m_accel.z = evt.data[4];

			gforce();
			orientation();

			//wgd::cout << wgd::DEV << "GFORCE: " << m_gforce.z << "\n";
			//break;

		case REPORT_BTN:
			button = htons(*(short*)&evt.data[0]);
			//wgd::cout << wgd::DEV << "Wiimote button pressed: " << button << "\n";

			if (button & WIIMOTE_BUTTON_TWO) get(ix::buttons)[BUTTON_TWO] = true; else get(ix::buttons)[BUTTON_TWO] = false;
			if (button & WIIMOTE_BUTTON_ONE) get(ix::buttons)[BUTTON_ONE] = true; else get(ix::buttons)[BUTTON_ONE] = false;
			if (button & WIIMOTE_BUTTON_LEFT) get(ix::buttons)[BUTTON_LEFT] = true; else get(ix::buttons)[BUTTON_LEFT] = false;
			if (button & WIIMOTE_BUTTON_RIGHT) get(ix::buttons)[BUTTON_RIGHT] = true; else get(ix::buttons)[BUTTON_RIGHT] = false;
			if (button & WIIMOTE_BUTTON_UP) get(ix::buttons)[BUTTON_UP] = true; else get(ix::buttons)[BUTTON_UP] = false;
			if (button & WIIMOTE_BUTTON_DOWN) get(ix::buttons)[BUTTON_DOWN] = true; else get(ix::buttons)[BUTTON_DOWN] = false;
			if (button & WIIMOTE_BUTTON_PLUS) get(ix::buttons)[BUTTON_PLUS] = true; else get(ix::buttons)[BUTTON_PLUS] = false;
			if (button & WIIMOTE_BUTTON_MINUS) get(ix::buttons)[BUTTON_MINUS] = true; else get(ix::buttons)[BUTTON_MINUS] = false;
			if (button & WIIMOTE_BUTTON_HOME) get(ix::buttons)[BUTTON_HOME] = true; else get(ix::buttons)[BUTTON_HOME] = false;
			if (button & WIIMOTE_BUTTON_A) get(ix::buttons)[BUTTON_A] = true; else get(ix::buttons)[BUTTON_A] = false;
			if (button & WIIMOTE_BUTTON_B) get(ix::buttons)[BUTTON_B] = true; else get(ix::buttons)[BUTTON_B] = false;

			break;

		default: break;
			//wgd::cout << wgd::WARN << "Unknown wiimote event (" << (int)evt.type << ")\n";
		}
	}	
	
}

void wgd::Wiimote::reportType(bool motion, bool expansion) {
	char buf[2];
	buf[0] = 0x00;
	buf[1] = 0x00;

	if (rumble())
		buf[0] |= 0x01;

	if (motion && expansion)
		buf[1] = 0x37;
	else if (expansion)
		buf[1] = 0x36;
	else if (motion)
		buf[1] = 0x33;
	else
		buf[1] = 0x30;

	send_wii(CMD_REPORT_TYPE, buf, 2);
}

bool wgd::Wiimote::poll_wii(WiiEvent &evt) {
	
	#ifndef NO_BLUETOOTH
	#ifdef LINUX
	fd_set fds;
	FD_ZERO(&fds);
	int highest = 0;

	//Timeout of 500 micro seconds.
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_SET(m_insock, &fds);
	highest = m_insock;

	select(highest+1, &fds, 0, 0, &tv);
	
	if (FD_ISSET(m_insock, &fds)) {
		int res = read(m_insock, (char*)&evt, sizeof(WiiEvent));

		if (res == -1) {
			//wgd::cout << wgd::ERR << "Wiimote read error.\n";
			return false;
		}

		if (res < 2) {
			//wgd::cout << wgd::ERR << "Wiimote disconnected.\n";
			return false;
		}

		//wgd::cout << "EVT Size: " << res << "\n";
	} else {
		return false;
	}
	#endif
	
	#ifdef WIN32
	
	//windows gets its stuff from the thread!
	//DWORD wait = WaitForSingleObject(m_addr.event, 300);
	
	
	if(msgs.empty()) return false;
	
	EnterCriticalSection(&m_lock);
	//get message from queue
	evt.proto = msgs.front()->proto;
	evt.type = msgs.front()->type;
	memcpy(evt.data, msgs.front()->data, 22);
	
	//delete message from queue
	delete msgs.front();
	msgs.pop();
	
	LeaveCriticalSection(&m_lock);
	
	#endif
	#endif

	return true;
}

//windows listener thread
#if defined(WIN32) && !defined(NO_BLUETOOTH)
unsigned CALLBACK wgd::Wiimote::listener(void * param){
	unsigned char report[22];
	memset(report, 0, 22);
	DWORD result, read;
	
	Wiimote *wii = (Wiimote*) param;
	
	while(wii->listening){
		//check read handle
		if(wii->m_addr.hRead != INVALID_HANDLE_VALUE){
			result = ReadFile(wii->m_addr.hRead, report, wii->m_addr.caps.InputReportByteLength, &read, (LPOVERLAPPED)&(wii->m_addr.overlapped));		
		}
		
		// Wait for read to finish
		result = WaitForSingleObject(wii->m_addr.event, 300);
		ResetEvent(wii->m_addr.event);
		
		//retry if the wait was unsuccessful
		if (result != WAIT_OBJECT_0) continue;
		
		//convert message
		WiiEvent *evt = new WiiEvent;;
		evt->proto=0;
		evt->type = report[0];
		memcpy(evt->data, (char*)report+1, 21);
		
		//add message to event queue
		EnterCriticalSection(&wii->m_lock);
		wii->msgs.push(evt);
		LeaveCriticalSection(&wii->m_lock);
		
	}
	return 0;
}
#endif



void wgd::Wiimote::send_wii(unsigned char cmd, char *msg, int length) {
	WiiMsg m;
	memset((char*)&m, 0, sizeof(WiiMsg));
	m.proto = SET_REPORT | BT_OUTPUT;
	m.type = cmd;
	memcpy(m.data, msg, length);

	#ifndef NO_BLUETOOTH
	#ifdef LINUX
	write(m_outsock, (char*)&m, length+2);
	//write(m_outsock, (char*)&m, 23);
	#endif
	#ifdef WIN32
	
	//WriteFile(m_addr, (char*)&m, length+2, &written, 0);
	DWORD written=0;
	ULONG result;
	char report[22];
	//clear report memory
	memset(report, 0, 22);
	//skip m.proto - as it results in ERROR_INVALID_PARAMETER
	memcpy(report, (char*)&m + 1, length+2);

	//check the write handle
	if (m_addr.hWrite != INVALID_HANDLE_VALUE){
		SetLastError(0);
		result = WriteFile(m_addr.hWrite, report, m_addr.caps.OutputReportByteLength, &written, NULL);
		if (!result){
			std::cout << "Write Failed [" << GetLastError() << "]\n";
		}
	}
	
	#endif
	#endif
}

void wgd::Wiimote::disconnect_wii() {
	#ifndef NO_BLUETOOTH
	#ifdef LINUX
	close(m_insock);
	close(m_outsock);
	#endif
	#ifdef WIN32
	
	//stop listen thread
	listening=false;
	WaitForSingleObject(m_addr.event, INFINITE);
	m_thread = NULL;
	DeleteCriticalSection(&m_lock);
	
	//close device handles
	CloseHandle(m_addr.hRead);
	CloseHandle(m_addr.hWrite);
	//CloseHandle(m_addr.hDevice);
	#endif
	#endif
}

void wgd::Wiimote::find() {
	//Search all bluetooth devices for WIIMOTES
	std::cout << "Scanning for Wiimotes...\n";

	#ifndef NO_BLUETOOTH
	#ifdef LINUX
	int id = hci_get_route(0);
	if (id < 0) {
		return;
	}

	int sock = hci_open_dev(id);
	if (sock < 0) {
		return;
	}

	inquiry_info *scan_info = new inquiry_info[128];
	memset(scan_info, 0, sizeof(inquiry_info)*128);

	int num_dev = hci_inquiry(id, 5, 128, 0, &scan_info, IREQ_CACHE_FLUSH);
	if (num_dev < 0) {
		return;
	}

	//wgd::cout << wgd::DEV << "Found " << num_dev << " bluetooth devices.\n";

	s_nummotes = 0;
	for (int i=0; i<num_dev; i++) {
		if (	(scan_info[i].dev_class[0] == WIIMOTE_DEV_CLASS_0) &&
			(scan_info[i].dev_class[1] == WIIMOTE_DEV_CLASS_1) &&
			(scan_info[i].dev_class[2] == WIIMOTE_DEV_CLASS_2)) {
			
			std::cout << "We found a Wiimote\n";
			
			if(inputbase["wiimotes"][s_nummotes]==Null) inputbase["wiimotes"][s_nummotes] = OID::create();
			s_motes[s_nummotes] = new Wiimote(scan_info[i].bdaddr, inputbase["wiimotes"][s_nummotes]);
			s_nummotes += 1;
		}
	}

	close(sock);
	#endif

	#ifdef WIN32
	
	//next version...
	HIDDevice device;
	HANDLE deviceHandle = 0;
	HANDLE hDevInfo;

	#ifdef _MSC_VER
	//Load the HID dll manually
	HMODULE hiddll = LoadLibrary(L"hid.dll");
	HidD_GetAttributes = (BOOL (WINAPI *) (HANDLE, PHIDD_ATTRIBUTES))GetProcAddress(hiddll, "HidD_GetAttributes");
	HidD_GetHidGuid = (VOID (WINAPI *) (LPGUID))GetProcAddress(hiddll, "HidD_GetHidGuid");
	HidD_GetPreparsedData = (BOOL (WINAPI *)(HANDLE, PHIDP_PREPARSED_DATA  *))GetProcAddress(hiddll, "HidD_GetPreparsedData");
	HidD_FreePreparsedData = (BOOL (WINAPI *)(PHIDP_PREPARSED_DATA))GetProcAddress(hiddll, "HidD_FreePreparsedData");
	HidP_GetCaps = (NTSTATUS DDKAPI (WINAPI *)(PHIDP_PREPARSED_DATA  PreparsedData, PHIDP_CAPS  Capabilities))GetProcAddress(hiddll, "HidP_GetCaps");
	#endif

	HIDD_ATTRIBUTES				attributes;
	SP_DEVICE_INTERFACE_DATA		devInfoData;
	int					memberIndex = 0;
	LONG					result;	
	GUID					HidGuid;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	detailData = NULL;
	ULONG required = 0;
	ULONG length = 0;
	
	HidD_GetHidGuid(&HidGuid);
	hDevInfo = SetupDiGetClassDevs(&HidGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
	
	devInfoData.cbSize = sizeof(devInfoData);
	while(true){
		
		// Got any more devices?
		result = SetupDiEnumDeviceInterfaces (hDevInfo, 0, &HidGuid, memberIndex, &devInfoData);
		if(result==0) break;
		
		// Call once to get the needed buffer length
		result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &length, NULL);
		detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(length);
		detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		
		// After allocating, call again to get data
		result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, length, &required, NULL);

		//open devce
		deviceHandle = CreateFile(detailData->DevicePath, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 0, NULL);

		//Is this a wiimote?
		attributes.Size = sizeof(attributes);
		result = HidD_GetAttributes(deviceHandle, &attributes);
		if (attributes.VendorID == WIIMOTE_VID && attributes.ProductID == WIIMOTE_PID){
			
			//wgd::cout << wgd::DEV << "Found a Wiimote!\n";			
			std::cout << "Found a wiimote\n";
			
			device.hDevice = deviceHandle;

			//Does not work in visual studio.
			#ifdef _MSC_VER
			//break;
			#endif
				
			//Get the Capabilities structure for the device.
			PHIDP_PREPARSED_DATA preparsedData;
			HidD_GetPreparsedData(deviceHandle, &preparsedData);
			HidP_GetCaps(preparsedData, &device.caps);
			HidD_FreePreparsedData(preparsedData);
			
		CloseHandle(deviceHandle);
			
			//get read and write handles seperately
			device.hWrite = CreateFile(detailData->DevicePath, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);			
		
			device.hRead  = CreateFile(detailData->DevicePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

			// Get an event object for the overlapped structure.
			device.event = CreateEvent (NULL, TRUE, TRUE, L"");
			device.overlapped.hEvent = device.event;
			device.overlapped.Offset = 0;
			device.overlapped.OffsetHigh = 0;
						
			
			//make sure the handles are valid
			bool fail=false;
			if(device.hRead == INVALID_HANDLE_VALUE) {
				cadence::Error(0, "Invalid Wiimote Read Handle");
				fail=true;
			}
			if(device.hWrite == INVALID_HANDLE_VALUE) {
				cadence::Error(0, "Invalid Wiimote Write Handle");
				fail=true;
			}			
			
			if(!fail) {
				if(inputbase["wiimotes"][s_nummotes] == Null) {
					std::cout << "Null - creating\n";
					inputbase["wiimotes"][s_nummotes] = OID::create();
				}
				s_motes[s_nummotes++] = new Wiimote(device, inputbase["wiimotes"][s_nummotes]);	
			}
			
		} else {
			
			CloseHandle(deviceHandle);
		}
		free(detailData);
		//next device
		memberIndex++;
	} 

	
	SetupDiDestroyDeviceInfoList(hDevInfo);

	#ifdef _MSC_VER
	FreeLibrary(hiddll);
	#endif
	
	#endif
	#endif
}

void wgd::Wiimote::initialise() {

	#ifndef NO_BLUETOOTH
	#ifdef WIN32
	WORD version = MAKEWORD(2,0);
	WSADATA wsadata;
	WSAStartup(version, &wsadata);
	#endif
	#endif

	BUTTON_LEFT = "left";
	BUTTON_RIGHT = "right";
	BUTTON_UP = "up";
	BUTTON_DOWN = "down";
	BUTTON_PLUS = "plus";
	BUTTON_MINUS = "minus";
	BUTTON_HOME = "home";
	BUTTON_ONE = "one";
	BUTTON_TWO = "two";
	BUTTON_A = "a";
	BUTTON_B = "b";
	BUTTON_C = "c";
	BUTTON_Z = "z";
}

void wgd::Wiimote::finalise() {
	#ifndef NO_BLUETOOTH
	#ifdef WIN32
	WSACleanup();
	#endif
	#endif
}

void wgd::Wiimote::updateAll() {
	for (int i=0; i<s_nummotes; i++) {
		//if(s_motes[i]) 
		s_motes[i]->update();
	}
}

