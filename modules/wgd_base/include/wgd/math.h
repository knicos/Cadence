#ifndef _WGD_MATH_
#define _WGD_MATH_

#include <math.h>
#include <stdlib.h>

namespace wgd {
	/** Constant Pi */
	static const double PI = 3.14159265;
	/** Constant 2 * pi */
	static const double TWO_PI = 6.28318531;
	/** Constant pi / 2 */
	static const double PI_HALF = 1.57079633;
	
	/** Convert degrees to radians */
	inline float degToRad(float deg) {
		return (float)(deg * (PI/180.0));
	};

	/** Convert radians to degrees */
	inline float radToDeg(float rad) {
		return (float)(rad * (180.0/PI));
	};

	/** Generate a random number from 0.0 to 1.0 */
	inline float random() {
		return (float)(rand() % 10000) / 10000.0f;
	};

	/** Generate a random number between 0 and n-1. */
	inline int random(int n) {
		return rand() % n;
	};

	/** Generate a random number between min and max-1 */
	inline int random(int min, int max) {
		return min + (rand() % (max-min));
	};

	#ifndef _MSC_VER
	template <typename A> A max(A a, A b) {
		return (a > b) ? a : b;
	};

	template <typename A> A min(A a, A b) {
		return (a < b) ? a : b;
	};
	#endif
};

#endif
