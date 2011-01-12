#ifndef _WGDVECTOR_
#define _WGDVECTOR_

#include <math.h>
#include <cadence/doste/oid.h>
#include <cadence/doste/type_traits.h>
#include <wgd/common.h>
#include <iostream>

#ifndef __SSE__
#define __SSE__
#endif

#ifdef __SSE__
#include <xmmintrin.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#endif

//Crazy macro to shorten events
#include <cadence/doste/modifiers.h>
#define EVENT_VECTOR(A,B)	EVENT(A,B(wgd::ix::x)(cadence::doste::modifiers::Seq)B(wgd::ix::y)(cadence::doste::modifiers::Seq)B(wgd::ix::z)(cadence::doste::modifiers::Seq)B)

namespace wgd {

	class matrix;
	class quaternion;
	class vector3d;

	/**
	 * A two dimensional floating point vector. All common vector
	 * operations are supported, mostly through operator
	 * overloading so you can to c = a+b to add vectors.
	 */
	class vector2d
	{
		public:
		float x;
		float y;

		vector2d(const cadence::doste::OID &o) {
			x = o[ix::x];
			y = o[ix::y];
		}

		void fillObject(const cadence::doste::OID &obj) const {
			//std::cout << "FILL VECTOR\n";
			obj.set(ix::x, x, true);
			obj.set(ix::y, y, false);
		}

		operator cadence::doste::OID() const {
			//TODO: Do something?
			//std::cout << "MAKE VECTOR2D\n";
			cadence::doste::OID v = cadence::doste::OID::create();
			v[ix::x] = x;
			v[ix::y] = y;
			return v;
		}
	
		vector2d() : x(0), y(0) {};
		
		vector2d(float px, float py)
			:	x(px),
				y(py) {};
	
		vector2d(float *pf)		//Assumes only of size 3... w is ignored.
			:	x(pf[0]),
				y(pf[1]) {};
		
		vector2d(const vector2d &v)
			: x(v.x), y(v.y) {};
	
		inline vector2d(const vector3d &v);
	
		float length() const {
			return sqrt(x*x + y*y);
		}
		
		float lengthSquared() const {
			return (x*x + y*y);
		}
		
		float distance (const vector2d &v) const {
			float dx = x - v.x;
			float dy = y - v.y;
			return sqrt(dx * dx + dy * dy);
		}
		
		float distanceSquared (const vector2d &v) const {
			float dx = x - v.x;
			float dy = y - v.y;
			return dx * dx + dy * dy;
		}
		
		/**
		 * Calculate the distance between two vectors.
		 * @return Distance between vectors.
		 */
		static float distance(const vector2d &a, const vector2d &b) {
			return a.distance(b);
		};

		/**
		 * Calculate the distance squared between two vectors.
		 * @return Distance squared between vectors.
		 */
		static float distanceSquared(const vector2d &a, const vector2d &b) {
			return a.distanceSquared(b);
		};
	
		vector2d normalise() {
			float l = length();
			if (l == 0)
				return *this;

			return vector2d(x / l, y / l);
		};
	
		const float *getArray() const { return &x; };
		void fillArray(float *a) {
			a[0] = x;
			a[1] = y;
		};
	
		float dot (const vector2d &v) const {
			return dotProduct(*this, v);
		}
	
		/** Unary negation */
		vector2d operator-() const {
			return vector2d(-x, -y);
		};
	
		vector2d operator+(const vector2d &v) const {
			return vector2d(x+v.x, y+v.y);
		};
	
		vector2d operator-(const vector2d &v) const {
			return vector2d(x-v.x, y-v.y);
		};
	
		vector2d operator*(float scale) const {
			return vector2d(x*scale, y*scale);
		};
	
		vector2d operator*(const vector2d &v) const {
			return vector2d(x*v.x, y*v.y);
		};
	
		vector2d operator/(float scale) const {
			return vector2d(x/scale, y/scale);
		};
		
		vector2d operator/(const vector2d &v) const {
			return vector2d(x/v.x, y/v.y);
		};
	
		vector2d& operator+=(const vector2d &v) {
			x += v.x;
			y += v.y;
			
			return *this;
		};
	
		vector2d& operator-=(const vector2d &v) {
			x -= v.x;
			y -= v.y;
			
			return *this;
		};
	
		vector2d& operator*=(float scale) {
			x *= scale;
			y *= scale;
			
			return *this;
		};
	
		vector2d& operator*=(const vector2d &v) {
			x *= v.x;
			y *= v.y;
			
			return *this;
		};
	
		vector2d& operator/=(float scale) {
			x /= scale;
			y /= scale;
			
			return *this;
		};
		
		bool operator==(const vector2d& v) {
			return (x == v.x && y == v.y);
		}
	
		bool operator!=(const vector2d& v) {
			return (x != v.x || y != v.y);
		}
	
		static float dotProduct(const vector2d &a, const vector2d &b) {
			return (a.x*b.x + a.y*b.y);
		};

		static vector2d lerp(const vector2d &a, const vector2d &b, float t) {
			return vector2d(a.x+((b.x-a.x)*t), a.y+((b.y-a.y)*t));
		}
		
		vector2d translate(float angle, const vector2d &v) {
			return vector2d(x + v.x * cos(angle), y + v.y * sin(angle));
		}

		friend std::ostream &operator<<(std::ostream &os, const vector2d &vec);
		inline friend vector2d operator*(float scale, const vector2d &v);
	};

	/**
	 * A three dimensional floating point vector. It actually contains a fourth
	 * dimension so that it can be used with the Matrix class. All common
	 * vector operators can be performed, mostly by using the overloaded
	 * operators.
	 */
	#ifdef _MSC_VER
	__declspec(align(16)) class vector3d
	#else
	class vector3d
	#endif
	{
		public:
		union {
		#ifdef __SSE__
		__m128 _L1;
		#endif
		struct {
		float x; /**< X-Coordinate */
		float y; /**< Y-Coordinate */
		float z; /**< Z-Coordinate */
		float w; /**< Dummy Coordinate */
		};
		};

		vector3d(const cadence::doste::OID &o) {
			x = o[ix::x];
			y = o[ix::y];
			z = o[ix::z];
			w = 1.0;
		}

		void fillObject(const cadence::doste::OID &obj) const {
			//std::cout << "FILL VECTOR\n";
			obj.set(ix::x, x, true);
			obj.set(ix::y, y, true);
			obj.set(ix::z, z, false);
		}

		operator cadence::doste::OID() const {
			//TODO: Do something?
			//std::cout << "MAKE VECTOR3D: x=" << x << " y=" << y << " z=" << z << "\n";
			cadence::doste::OID v = cadence::doste::OID::create();
			v[ix::x] = x;
			v[ix::y] = y;
			v[ix::z] = z;
			return v;
		}
	
		/** Zero vector. */
		vector3d() :x(0), y(0), z(0), w(1.0) {};

		/**
		 * 2d constructor, z defaults to 0.0. This is used for 2d games when specifying
		 * position of sprites etc. In a 2d game the values correspond to pixels.
		 * @param px X-Coordinate.
		 * @param py Y-Coordinate.
		 */
		vector3d(float px, float py)
			:	x(px),
				y(py),
				z(0.0),
				w(1.0) {};

		/**
		 * XYZ constructor. The W-Coordinate defaults to 1.0.
		 * @param px X-Coordinate.
		 * @param py Y-Coordinate.
		 * @param pz Z-Coordinate.
		 */
		vector3d(float px, float py, float pz)
			:	x(px),
				y(py),
				z(pz),
				w(1.0) {};
	
		/**
		 * XYZW constructor. You will not usually need this, use XYZ instead.
		 * @param px X-Coordinate.
		 * @param py Y-Coordinate.
		 * @param pz Z-Coordinate.
		 * @param pw W-Coordinate.
		 */
		vector3d(float px, float py, float pz, float pw)
			:	x(px),
				y(py),
				z(pz),
				w(pw) {};
	
		/**
		 * Construct from a 3 float array.
		 * @param pf Pointer to 3 floats.
		 */
		vector3d(float *pf)		//Assumes only of size 3... w is ignored.
			:	x(pf[0]),
				y(pf[1]),
				z(pf[2]),
				w(1.0) {};
	
		vector3d(const vector2d &v)
			: x(v.x), y(v.y), z(0), w(1.0) {};
	
		vector3d(const vector3d &v)
			: x(v.x), y(v.y), z(v.z), w(v.w) {};
	
		/**
		 * Calculate the vectors length. Note that this uses square root and may be slow.
		 * @return Length of vector.
		 */
		float length() const
		{
			return sqrt(x*x + y*y + z*z);
		}
		/**
		 * Calculate the squared length of this vector. This avoids the square root so is faster.
		 * @return Squared length of vector
		 */
		float lengthSquared() const
		{
			return (x*x + y*y + z*z);
		}

		/**
		 * Calculate the distance to a vector
		 * @return Distance to the other vector.
		 */
		float distance (const vector3d &v) const {
			float dx = x - v.x;
			float dy = y - v.y;
			float dz = z - v.z;
			return sqrt(dx * dx + dy * dy + dz * dz);
		}
		
		/**
		 * Calculate the distance to a vector
		 * @return Distance to the other vector.
		 */
		float distanceSquared (const vector3d &v) const {
			float dx = x - v.x;
			float dy = y - v.y;
			float dz = z - v.z;
			return dx * dx + dy * dy + dz * dz;
		}
		
		/**
		 * Calculate the distance between two vectors.
		 * @return Distance between vectors.
		 */
		static float distance(const vector3d &a, const vector3d &b) {
			return a.distance(b);
		};

		/**
		 * Calculate the distance squared between two vectors.
		 * @return Distance squared between vectors.
		 */
		static float distanceSquared(const vector3d &a, const vector3d &b) {
			return a.distanceSquared(b);
		};
	
		/**
		 * Normalise the vector. This will make it have a length of 1.
		 */
		vector3d normalise() const
		{
			float l = length();
			if (l == 0)
				return *this;

			return vector3d(x / l, y / l, z / l);
		};
	
		/**
		 * Get this vector as an array of floats. Note that this is read-only.
		 * @return Pointer to 3 (or 4) floats.
		 */
		const float *getArray() const { return &x; };

		/**
		 * Put this vector into a specified array. Similar to getArray. This will only
		 * fill the array with XYZ not W.
		 * @param a Pointer to 3 float array.
		 */
		void fillArray(float *a) {
			a[0] = x;
			a[1] = y;
			a[2] = z;
			//Don't assume w.
		};

		/**
		 * Make this vector from a float array. Useful if you want to reuse a vector
		 * instead of making a new one.
		 * @param a 3 float array.
		 */
		void fromArray(const float *a) {
			x = a[0];
			y = a[1];
			z = a[2];
		};
		
		float dot (const vector3d &v) const {
			return dotProduct(*this, v);
		}
	
		vector3d cross (const vector3d &v) const {
			return crossProduct(*this, v);
		}
	
		/** Unary negation */
		vector3d operator-() const {
			return vector3d(-x, -y, -z);
		};
	
		/** Add two vectors. */
		vector3d operator+(const vector3d &v) const {
			return vector3d(x+v.x, y+v.y, z+v.z);
		};
	
		/** Subtract a vector from another. */
		vector3d operator-(const vector3d &v) const {
			return vector3d(x-v.x, y-v.y, z-v.z);
		};
	
		/** Scale a vector */
		vector3d operator*(float scale) const {
			return vector3d(x*scale, y*scale, z*scale);
		};

		/** Scale a vector */
		vector3d operator/(float scale) const {
			return vector3d(x/scale, y/scale, z/scale);
		};
		
		vector3d operator/(const vector3d &v) const {
			return vector3d(x/v.x, y/v.y, z/v.z);
		};

		vector3d& operator+=(const vector3d &v) {
			x += v.x;
			y += v.y;
			z += v.z;
			
			return *this;
		};
	
		vector3d& operator-=(const vector3d &v) {
			x -= v.x;
			y -= v.y;
			z -= v.z;
			
			return *this;
		};
	
		vector3d& operator*=(float scale) {
			x *= scale;
			y *= scale;
			z *= scale;
			
			return *this;
		};
	
		vector3d& operator/=(float scale) {
			x /= scale;
			y /= scale;
			z /= scale;
			
			return *this;
		};
	
		vector3d& operator*=(const vector3d &v) {
			x *= v.x;
			y *= v.y;
			z *= v.z;
			
			return *this;
		};
	
		bool operator==(const vector3d& v) {
			return (x == v.x && y == v.y && z == v.z);
		}
	
		bool operator!=(const vector3d& v) {
			return (x != v.x || y != v.y || z != v.z);
		}
	
		/**
		 * Scale a vector. This is faster as fewer vector temporaries are created.
		 * @param result Scaled result goes here.
		 * @param a vector to scale.
		 * @param b Scale factor.
		 */
		static void fastScale(vector3d &result, const vector3d &a, float scale) {
			result.x = a.x*scale;
			result.y = a.y*scale;
			result.z = a.z*scale;
		};

		/**
		 * Translate this vector based on a vector with an arbitrary axis.
		 * It can be used for movement and velocities where you need to
		 * take the rotation into account.
		 * @param q Rotation quaternion for axis.
		 * @param v Translate by this vector in the given axis
		 * @return Translated vector.
		 */
		inline vector3d translate(const wgd::quaternion &q, const wgd::vector3d &v) const;
	
		/**
		 * Translate this vector based on a vector with an arbitrary axis.
		 * It can be used for movement and velocities where you need to
		 * take the rotation into account.
		 * @param q Rotation matrix for axis.
		 * @param v Translate by this vector in the given axis
		 * @return Translated vector.
		 */
		inline vector3d translate(const wgd::matrix &m, const wgd::vector3d &v) const;
	
		/** Multiply two vectors. */
		vector3d operator*(const vector3d &v) const {
			return vector3d(x*v.x, y*v.y, z*v.z);
		};
	
		/**
		 * Calculate the dot product of two vectors. This corresponds to the angle between them.
		 * @param a First vector.
		 * @param b Second vector.
		 * @return Dot product of a.b.
		 */
		static float dotProduct(const vector3d &a, const vector3d &b) {
			return (a.x*b.x + a.y*b.y + a.z*b.z);
		};
	
		/**
		 * Cross product of two vectors a and b.
		 * @param a First vector.
		 * @param b Second vector.
		 * @return Cross product of a and b.
		 */
		static vector3d crossProduct(const vector3d &a, const vector3d &b) {
			return vector3d(	(a.y*b.z) - (a.z*b.y),
							(a.z*b.x) - (a.x*b.z),
							(a.x*b.y) - (a.y*b.x));
		};

		/**
		 * Linear interpolation between two vectors.
		 * @param a Start vector.
		 * @param b End vector.
		 * @param t Interpolation coeficient.
		 * @return The interpolated vector.
		 */
		static vector3d lerp(const vector3d &a, const vector3d &b, float t) {
			return vector3d(a.x+((b.x-a.x)*t), a.y+((b.y-a.y)*t), a.z+((b.z-a.z)*t), a.w+((b.w-a.w)*t));
		}
	
		/**
		 * Get normal from 3 vectors. Useful if you have a triangle and need to find its normal.
		 * @param a vector A
		 * @param b vector B
		 * @param c vector C
		 * @return Normal to plane described by A B and C.
		 */
		static vector3d makeNormal(const vector3d &a, const vector3d &b, const vector3d &c) {
			return vector3d::crossProduct(b-a,c-a).normalise();
		};

		friend std::ostream &operator<<(std::ostream &os, const vector3d &vec);
		inline friend vector3d operator*(float scale, const vector3d &v);
	} ALIGNED;


};

namespace cadence {
	namespace doste {
		template <> class is_pod<wgd::vector2d> : public true_type {};
		template <> class is_pod<wgd::vector3d> : public true_type {};
		template <> class is_reuseable<wgd::vector3d> : public true_type {};
		template <> class is_reuseable<wgd::vector2d> : public true_type {};
	};
};

#include <wgd/matrix.h>
#include <wgd/quaternion.h>

wgd::vector3d wgd::vector3d::translate(const wgd::quaternion &q, const wgd::vector3d &v) const {
	wgd::matrix mat;
	wgd::quaternion quat = q.conjugate();
	quat.createMatrix(mat);
	
	vector3d w = v;
	w = mat * w;
	w += *this;
	return w;
}

wgd::vector3d wgd::vector3d::translate(const wgd::matrix &mat, const wgd::vector3d &v) const {
	return *this + (mat * v);
}

wgd::vector2d::vector2d(const wgd::vector3d &v)
	: x(v.x), y(v.y) {};

wgd::vector2d wgd::operator*(float scale, const wgd::vector2d &v) {
	return vector2d(v.x * scale, v.y * scale);
}

wgd::vector3d wgd::operator*(float scale, const wgd::vector3d &v) {
	return vector3d(v.x * scale, v.y * scale, v.z * scale);
}

#endif


