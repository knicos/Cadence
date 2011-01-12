/*
 * WGD Library
 *
 * Authors: 
 * Date: 17/9/2007
 *
 */

#ifndef _WGD_COLOUR_
#define _WGD_COLOUR_

#include <wgd/index.h>
#include <cadence/doste/oid.h>
#include <cadence/doste/type_traits.h>
#include <wgd/common.h>

#include <iostream>

#define EVENT_COLOUR(A,B)	EVENT(A,B(wgd::ix::r)(cadence::doste::modifiers::Seq)B(wgd::ix::g)(cadence::doste::modifiers::Seq)B(wgd::ix::b)(cadence::doste::modifiers::Seq)B(wgd::ix::a)(cadence::doste::modifiers::Seq)B)

namespace wgd {

	enum ColourFormat { RGBA_CHAR, RGB_CHAR, RGB_FLOAT, RGBA_FLOAT, R_CHAR, R_FLOAT, RG_CHAR };

	#ifdef _MSC_VER
	#pragma pack(push)
	#pragma pack(1)
	#endif

	struct colour4i {
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;

		static const ColourFormat format = RGBA_CHAR;
	} PACKED;

	struct colour4f {
		float r;
		float g;
		float b;
		float a;

		static const ColourFormat format = RGBA_FLOAT;
	} PACKED;

	struct colour3i {
		unsigned char r;
		unsigned char g;
		unsigned char b;

		static const ColourFormat format = RGB_CHAR;
	} PACKED;

	struct colour3f {
		float r;
		float g;
		float b;

		static const ColourFormat format = RGB_FLOAT;
	} PACKED;

	struct colour2i {
		unsigned short r;

		static const ColourFormat format = RG_CHAR;
	} PACKED;

	struct colour1i {
		unsigned char l;

		static const ColourFormat format = R_CHAR;
	} PACKED;

	struct colour1f {
		float i;

		static const ColourFormat format = R_FLOAT;
	} PACKED;

	#ifdef _MSC_VER
	#pragma pack(pop)
	#endif

	/**
	 * Represents a 32 bit floating point colour. The values of the RGBA
	 * components can be between 0.0 and 1.0. Additional operations are
	 * provided to manipulate colours, such as linear interpolation from one
	 * colour to another.<br/>
	 * All colours in this library are represented with this class.
	 */
	struct colour {

		colour() {};
		/** RGB Constructor. Alpha is 1.0 by default. */
		colour(float _r, float _g, float _b)
			: r(_r),g(_g),b(_b),a(1.0f) {};
		/** RGBA Constructor */
		colour(float _r, float _g, float _b, float _a)
			: r(_r),g(_g),b(_b),a(_a) {};

		colour(const cadence::doste::OID &o) {
			r = o[ix::r];
			g = o[ix::g];
			b = o[ix::b];
			if(o[ix::a]==Null) a = 1.0f; //default alpha to 1.0f if not specified
			else a = o[ix::a];
		}

		void fillObject(const cadence::doste::OID &obj) const {
			//std::cout << "FILL VECTOR\n";
			obj.set(ix::r, r, true);
			obj.set(ix::g, g, true);
			obj.set(ix::b, b, true);
			obj.set(ix::a, a, false);
		}

		operator cadence::doste::OID() const {
			//TODO: Do something?
			cadence::doste::OID c = cadence::doste::OID::create();
			c[ix::r] = r;
			c[ix::g] = g;
			c[ix::b] = b;
			c[ix::a] = a;
			return c;
		}

		/**
		 * Convert to an array of floats. You pass a pointer to an array of 4 floats and this method
		 * will fill it with this colour. This is used for OpenGL which takes an array of floats.
		 * @param v Pointer for 4 float array.
		 */
		void toArray(float *ar) {
			ar[0] = r;
			ar[1] = g;
			ar[2] = b;
			ar[3] = a;
		};

		/**
		 * Interpolated between two colours. Given two colours it will linearly interpolate between then using the
		 * parameter t. If t is 0.0 then the colour is the first one, if it is 1.0 then it is the second one. Any
		 * value of t between 0 and 1 gives an interpolated colour.
		 * @param a First colour, returned if t = 0.0.
		 * @param b Second colour, returned if t = 1.0.
		 * @param t Position between colours, 0.0 is start, 1.0 is end.
		 * @return Interpolated colour.
		 */
		static colour lerp(const colour &a, const colour &b, float t) {
			return colour(a.r+((b.r-a.r)*t), a.g+((b.g-a.g)*t), a.b+((b.b-a.b)*t), a.a+((b.a-a.a)*t));
		}

		float r, g, b, a;
	};
};

namespace cadence {
	namespace doste {
		template <> class is_pod<wgd::colour> : public true_type {};
		template <> class is_reuseable<wgd::colour> : public true_type {};
	};
};

#endif
