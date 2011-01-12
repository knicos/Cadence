#include <wgd/joystick.h>
#include <wgd/math.h>

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#include <regstr.h>
#endif

#ifdef LINUX
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#define test_bit(nr, addr) (((1UL << ((nr) & 31)) & (((const unsigned int *) addr)[(nr) >> 5])) != 0)
#endif

#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

int wgd::Joystick::s_numjs = 0;
Joystick *wgd::Joystick::s_jsticks[MAX_JOYSTICKS];

extern cadence::doste::OID inputbase;

#ifdef LINUX
Joystick::Joystick(const OID &obj, int fp, const char *devname)
	: Object(obj), m_numbuts(0), m_numaxis(0) {
	m_fp = fp;

	if (get(ix::buttons) == Null) set(ix::buttons, OID::create());
	if (get(ix::axes) == Null) set(ix::axes, OID::create());

	fcntl(fp, F_SETFL, O_NONBLOCK);

	//Get the joystick name
	ioctl(m_fp, EVIOCGNAME(sizeof(m_name)), m_name);

	unsigned int keyBit[40];
	unsigned int absBit[40];
	unsigned int relBit[40];

	//Uses new unified API (HID) ? Assume yes
	if ((ioctl(fp, EVIOCGBIT(EV_KEY, sizeof(keyBit)), keyBit) >= 0) &&
        		(ioctl(fp, EVIOCGBIT(EV_ABS, sizeof(absBit)), absBit) >= 0) &&
        		(ioctl(fp, EVIOCGBIT(EV_REL, sizeof(relBit)), relBit) >= 0)) {
        	//joystick->hwdata->is_hid = SDL_TRUE;
	}

	for (int i=BTN_JOYSTICK; i<KEY_MAX; ++i) {
		if (test_bit(i, (void*)keyBit)) {
			//Need to key map (i-BTN_MISC) = number;
			m_keymap[i-BTN_MISC] = m_numbuts;
			++m_numbuts;
		}
	}

	for (int i=BTN_MISC; i<BTN_JOYSTICK; ++i) {
		if (test_bit(i, keyBit)) {
			//Need to key map (i-BTN_MISC) = number;
			m_keymap[i-BTN_MISC] = m_numbuts;
			++m_numbuts;
		}
	}

	for (int i=0; i< ABS_MAX; ++i) {
		//Skip hats
		if (i == ABS_HAT0X) {
			i = ABS_HAT3Y;
			continue;
		}

		if (test_bit(i, absBit)) {
			int values[5];
			if (ioctl(m_fp, EVIOCGABS(i), values) < 0)
				continue;

			//Complex axis shit goes here.
			m_absmap[i] = m_numaxis;
			if (get(ix::axes)[m_numaxis] == Null) {
				get(ix::axes)[m_numaxis] = OID::create();
				get(ix::axes)[m_numaxis]["max"] = 0;
				get(ix::axes)[m_numaxis]["min"] = 0;
				get(ix::axes)[m_numaxis][ix::deadzone] = 0.15f;
			}
			++m_numaxis;
		}
	}

	//Detect hats
	for (int i=ABS_HAT0X; i<=ABS_HAT3Y; i+=2) {
		if (test_bit(i, absBit) || test_bit(i+1, absBit)) {
			//one more hat.
		}
	}

	std::cout << "Found joystick: " << m_name << " with " << m_numbuts << " buttons and " << m_numaxis << " axis\n";
}
#endif

#ifdef WIN32
Joystick::Joystick(const OID obj, int id, const JOYCAPS &joycaps)
	: Object(obj), m_joycaps(joycaps), m_id(id) {
	//strcpy(m_name, (const char*)joycaps.szPname);
	for(int i=0;  i<80; i++) m_name[i] = (char)joycaps.szPname[i];

	m_numbuts = m_joycaps.wNumButtons;
	m_numaxis = m_joycaps.wNumAxes;
	//Num hats
	//if (m_joycaps.wCaps & JOYCAPS_HASPOV);

	if (get("buttons") == Null) set("buttons", OID::create());
	if (get("axes") == Null) set("axes", OID::create());

	int min = 0;
	int max = 0;
	for (int i=0; i<m_numaxis; i++) {
		if (get(ix::axes)[i] == Null) {
			get(ix::axes)[i] =  OID::create();
			get(ix::axes)[i][ix::deadzone] = 0.15f;
		}

		//In windows we ignore DST callibration information.
		switch(i) {
		case 0:		min = m_joycaps.wXmin; max = m_joycaps.wXmax; break;
		case 1:		min = m_joycaps.wYmin; max = m_joycaps.wYmax; break;
		case 2:		min = m_joycaps.wZmin; max = m_joycaps.wZmax; break;
		case 3:		min = m_joycaps.wRmin; max = m_joycaps.wRmax; break;
		case 4:		min = m_joycaps.wUmin; max = m_joycaps.wUmax; break;
		case 5:		min = m_joycaps.wVmin; max = m_joycaps.wVmax; break;
		}
		
		get(ix::axes).get(i).set("min", min);
		get(ix::axes).get(i).set("max", max);
	}

	std::cout << "Found joystick: " << m_name << " with " << m_numbuts << " buttons and " << m_numaxis << " axis\n";
}
#endif

Joystick::~Joystick() {
	#ifdef LINUX
	close(m_fp);
	#endif
}


Joystick *wgd::Joystick::joystick(int num) {
	if (num < 0 || num >= MAX_JOYSTICKS)
		return 0;
	return s_jsticks[num];
}

float wgd::Joystick::axis(int ax) {
	return get(ix::axes)[ax][ix::value];
}

bool wgd::Joystick::btnPressed(int btn) {
	return get(ix::buttons)[btn];
}
void wgd::Joystick::inverted(int ax, bool invert) {
	get(ix::axes)[ax][ix::invert] = invert;
}

bool wgd::Joystick::inverted(int ax) {
	return get(ix::axes)[ax][ix::invert];
}

void wgd::Joystick::update() {
	int min, max;
	float deadzone;
	float value;

	#ifdef LINUX
	int len, code;
	input_event events[32];

	while ((len = read(m_fp, events, sizeof(events))) > 0) {
		len /= sizeof(events[0]);
		
		for (int i=0; i<len; i++) {
			code = events[i].code;
			//std::cout << "Joystick event " << events[i].type << "\n";
			
			switch (events[i].type) {
			case EV_KEY:
				if (code >= BTN_MISC) {
					code -= BTN_MISC;
					//Button pressed / release... events[i].value
					get(ix::buttons)[(int)m_keymap[code]] = (events[i].value) ? true : false;
				}
				break;

			case EV_ABS:
				//Axis change, code = HAT or AXIS number
				//Value is new state.
				switch(code) {
				case ABS_HAT0X:
				case ABS_HAT0Y:
				case ABS_HAT1X:
				case ABS_HAT1Y:
				case ABS_HAT2X:
				case ABS_HAT2Y:
				case ABS_HAT3X:
				case ABS_HAT3Y:
					//std::cout << "Hat was changed: " << code-ABS_HAT0X << "\n";
					break;
				default:
					min = get(ix::axes)[m_absmap[code]]["min"];
					max = get(ix::axes)[m_absmap[code]]["max"];
					deadzone = get(ix::axes)[m_absmap[code]][ix::deadzone];

					//Automatic calibration:
					if (events[i].value < min) {
						min = events[i].value;
						get(ix::axes)[m_absmap[code]]["min"] =  min;
					}
					if (events[i].value > max) {
						max = events[i].value;
						get(ix::axes)[m_absmap[code]]["max"] =  max;
					}

					value = (((float)events[i].value - ((min+max) / 2.0f)) / (float)(abs(min)+abs(max))) * 2.0f;
					if (inverted(i)) value = -value;

					if (fabs(value) < deadzone) {
						get(ix::axes)[m_absmap[code]][ix::value] =  0.0f;
					} else {
						get(ix::axes)[m_absmap[code]][ix::value] = value;
						//std::cout << "Axis changed: " << (int)m_absmap[code] << "=" << events[i].value << "\n";
					}
					break;
				}
				break;

			case EV_REL:
				//Relative Axis (Ball)
				break;

			default:
				break;
			}
		}
	}

	#else
	
	//First get axis positions
	JOYINFOEX joyinfo;
	joyinfo.dwSize = sizeof(joyinfo);
	joyinfo.dwFlags = JOY_RETURNALL | JOY_RETURNPOVCTS;
	joyGetPosEx(m_id, &joyinfo);
	
	int pos[6];
	pos[0] = joyinfo.dwXpos;
	pos[1] = joyinfo.dwYpos;
	pos[2] = joyinfo.dwZpos;
	pos[3] = joyinfo.dwRpos;
	pos[4] = joyinfo.dwUpos;
	pos[5] = joyinfo.dwVpos;

	DWORD flags[6] = {	JOY_RETURNX, JOY_RETURNY, JOY_RETURNZ,
					JOY_RETURNR, JOY_RETURNU, JOY_RETURNV
	};

	//Get axis positions...
	for (int i=0; i<m_numaxis; i++) {
		if (joyinfo.dwFlags & flags[i]) {
			min = get(ix::axes)[i]["min"];
			max = get(ix::axes)[i]["max"];
			
			deadzone = get(ix::axes)[i][ix::deadzone];
			value = (((float)pos[i] - ((min+max) / 2.0f)) / (float)(abs(min)+abs(max))) * 2.0f;
			if (inverted(i)) value = -value;
			if (fabs(value) < deadzone) {
				get(ix::axes)[i][ix::value] = 0.0f;
			} else {
				get(ix::axes)[i][ix::value] = value;
				//wgd::cout << wgd::DEV << "Axis changed: " << i << "=" << value << "\n";
			}
		}
	}

	//Now do the buttons.
	if (joyinfo.dwFlags & JOY_RETURNBUTTONS) {
		for (int i=0; i<m_numbuts; i++) {
			if (joyinfo.dwButtons & (1<<i)) {
				get(ix::buttons).set(i, true);
				//wgd::cout << "Button " << i << "\n";
			} else {
				get(ix::buttons).set(i, false);
			}
		}
	}
	
	//hat - pov being the angle of the hat in degrees
	if(joyinfo.dwPOV > 36000) set("pov", false);
	else set("pov", joyinfo.dwPOV / 100.0f);
	
	#endif
}

void wgd::Joystick::updateAll() {
	for (int i=0; i<s_numjs; i++)
		s_jsticks[i]->update();
}

void wgd::Joystick::initialise() {

	if (inputbase["joysticks"] == Null) inputbase["joysticks"] = OID::create();
	OID njs;

	#ifdef LINUX

	int js;
	unsigned int evBit[40];
	unsigned int keyBit[40];
	unsigned int absBit[40];
	char buf[100];

	for (int i=0; i<MAX_JOYSTICKS; i++) {
		s_jsticks[i] = 0;
	}

	//First we must find the joysticks
	for (int i=0; i<10; i++) {
		sprintf(buf, "/dev/input/event%d", i);

		js = open(buf, O_RDONLY);
		if (js == -1) {
			continue;
		}

		//Is this a joystick?
		if ((ioctl(js, EVIOCGBIT(0,sizeof(evBit)),evBit) < 0) ||
				(ioctl(js, EVIOCGBIT(EV_KEY,sizeof(keyBit)),keyBit) < 0) ||
				(ioctl(js, EVIOCGBIT(EV_ABS,sizeof(absBit)),absBit) < 0)) {
			close(js);
			continue;
		}

		if (!(test_bit(EV_KEY, evBit) && test_bit(EV_ABS, evBit) &&
				test_bit(ABS_X, absBit) && test_bit(ABS_Y, absBit) &&
				(test_bit(BTN_TRIGGER, keyBit) || test_bit(BTN_A, keyBit)
				|| test_bit(BTN_1, keyBit)))) {
			close(js);
			continue;
		}
		//add joystick to database
		inputbase["joysticks"][s_numjs] = OID::create();
		s_jsticks[s_numjs++] = new Joystick(inputbase["joysticks"][s_numjs], js, buf);
	}

	#else

	JOYINFOEX joyinfo;
	JOYCAPS joycaps;
	MMRESULT result;

	int maxdevs = joyGetNumDevs();

	for (int i = JOYSTICKID1; i < maxdevs && s_numjs < MAX_JOYSTICKS; ++i) {
		joyinfo.dwSize = sizeof(joyinfo);
		joyinfo.dwFlags = JOY_RETURNALL;
		result = joyGetPosEx(i, &joyinfo);
		if (result == JOYERR_NOERROR) {
			result = joyGetDevCaps(i, &joycaps, sizeof(joycaps));
			if (result == JOYERR_NOERROR) {
				//add joystick to database
				inputbase["joysticks"][s_numjs] = OID::create();
				s_jsticks[s_numjs++] = new Joystick(inputbase["joysticks"][s_numjs], i, joycaps);
				
			}
		}
	}

	#endif
}

void wgd::Joystick::finalise() {
}
