#ifndef _QUATERNION_
#define _QUATERNION_

#include <math.h>

namespace wgd {
	
	class matrix;
	class vector3d;
	/**
	 * Alternative for representing rotations. Advantages are that it does not suffer
	 * from gimble lock and produces much better interpolation.
	 */
	class quaternion
	{
		public:
		/** Default to 0. */
		quaternion()
			: m_x(0.0), m_y(0.0), m_z(0.0), m_w(1.0) {};
		/** Specify components, not pitch, yaw, roll. */
		quaternion(float x, float y, float z, float w)
			: m_x(x), m_y(y), m_z(z), m_w(w) {};
		/** Take from a 4 float array. */
		quaternion(float *f)
			: m_x(f[0]), m_y(f[1]), m_z(f[2]), m_w(f[3]){};
		~quaternion() {};
	
		/**
		 * Same as glRotate, it generates the specified rotation int the
	 	 * specified axis. e.g. createAxisRotation(2.0,1.0,0.0,0.0) = 2 radians in X.
		 */
		void createAxisRotation(float radians, float x, float y, float z)
		{
			float res = sin(radians / 2.0f);
			m_x = x*res;
			m_y = y*res;
			m_z = z*res;
			m_w = cos(radians / 2.0f);
		};
	
		/**
		 * Simpler to use except it can suffer from problems, it is best to
		 * use createAxisRotation.
		 * @param pitch Pitch (X-axis).
		 * @param yaw Yaw (Y-axis).
		 * @param roll Roll (Z-axis).
		 */
		void createRotation(float pitch, float yaw, float roll)
		{
			/*
			float p = pitch / 2.0;
			float y = yaw / 2.0;
			float r = roll / 2.0;
		
			float sinp = sin(p);
			float siny = sin(y);
			float sinr = sin(r);
			float cosp = cos(p);
			float cosy = cos(y);
			float cosr = cos(r);
		
			m_x = sinr * cosp * cosy - cosr * sinp * siny;
			m_y = cosr * sinp * cosy + sinr * cosp * siny;
			m_z = cosr * cosp * siny - sinr * sinp * cosy;
			m_w = cosr * cosp * cosy + sinr * sinp * siny;
		
			normalise();
			*/
			
			quaternion tmp1, tmpr;
			tmpr.createAxisRotation(roll, 0.0, 0.0, 1.0);
			tmp1.createAxisRotation(pitch, 1.0, 0.0, 0.0);
			tmpr = tmp1*tmpr;
			tmp1.createAxisRotation(yaw, 0.0, 1.0, 0.0);
			tmpr = tmp1 * tmpr;
			
			m_x = tmpr.m_x;
			m_y = tmpr.m_y;
			m_z = tmpr.m_z;
			m_w = tmpr.m_w;
			
			
		};
		inline void createRotation(const vector3d &v);
	
		/** Scale the quaternion. */
		quaternion operator*(float s) const
		{
			return quaternion(m_x*s, m_y*s, m_z*s, m_w*s);
		};
	
		/** Add two quaternions together */
		quaternion operator+(const quaternion &q) const
		{
			return quaternion(m_x+q.m_x, m_y+q.m_y, m_z+q.m_z, m_w+q.m_w);
		}
	
		/**
		 * Interpolate between two quaternion rotations.
		 * @param a Start quaternion (t = 0).
		 * @param b End quaternion (t = 1).
		 * @param t Interpolation value between 0 and 1.
		 */
		static quaternion slerp(const quaternion &a, const quaternion &b, float t)
		{
			float w1;
			float w2;
			float dot = a.m_x*b.m_x+a.m_y*b.m_y+a.m_z*b.m_z+a.m_w*b.m_w;
			float theta = acos(dot);
			float sintheta = sin(theta);
	
			if (sintheta > 0.001)
			{
				w1 = sin((1.0f-t)*theta) / sintheta;
				w2 = sin(t*theta) / sintheta;
			} else {
				w1 = 1.0f - t;
				w2 = t;
			}
	
			return a*w1+b*w2;
		};
	
		/**
		 * Converts to and OpenGL matrix. This is a 16 float array.
		 */
		void createMatrix(float *matrix) const
		{
			// First row
			matrix[ 0] = 1.0f - 2.0f * ( m_y * m_y + m_z * m_z ); 
			matrix[ 1] = 2.0f * (m_x * m_y + m_z * m_w);
			matrix[ 2] = 2.0f * (m_x * m_z - m_y * m_w);
			matrix[ 3] = 0.0f;  
		
			// Second row
			matrix[ 4] = 2.0f * ( m_x * m_y - m_z * m_w );  
			matrix[ 5] = 1.0f - 2.0f * ( m_x * m_x + m_z * m_z ); 
			matrix[ 6] = 2.0f * (m_z * m_y + m_x * m_w );  
			matrix[ 7] = 0.0f;  
		
			// Third row
			matrix[ 8] = 2.0f * ( m_x * m_z + m_y * m_w );
			matrix[ 9] = 2.0f * ( m_y * m_z - m_x * m_w );
			matrix[10] = 1.0f - 2.0f * ( m_x * m_x + m_y * m_y );  
			matrix[11] = 0.0f;  
		
			// Fourth row
			matrix[12] = 0;  
			matrix[13] = 0;  
			matrix[14] = 0;  
			matrix[15] = 1.0f;
		};
	
		/**
		 * Converts to a Matrix object.
		 * @see Matrix
		 */
		inline void createMatrix(wgd::matrix &m) const;
	
		/**
		 * Extract a pitch yaw roll vector.
		 */
		inline wgd::vector3d pitchYawRoll();
	
		/** Normalise. */
		void normalise()
		{
			float mag2 = m_w*m_w+m_x*m_x+m_y*m_y+m_z*m_z;
			if (fabs(mag2-1.0) > 0.00001)
			{
				float mag = sqrt(mag2);
				m_w /= mag;
				m_x /= mag;
				m_y /= mag;
				m_z /= mag;
			}
		};
	
		/** Conjugate, a kind of inverse. */
		quaternion conjugate() const {
			return quaternion(-m_x,-m_y,-m_z,m_w);
		};
	
		/**
		 * Multiply two quaternions together. This combines the rotation as would
		 * combining two rotation matrices.
		 */
		quaternion operator*(const quaternion &q) const
		{
			return quaternion(	m_w*q.m_x + m_x*q.m_w + m_y*q.m_z - m_z*q.m_y,
								m_w*q.m_y + m_y*q.m_w + m_z*q.m_x - m_x*q.m_z,
								m_w*q.m_z + m_z*q.m_w + m_x*q.m_y - m_y*q.m_x,
								m_w*q.m_w - m_x*q.m_x - m_y*q.m_y - m_z*q.m_z);
		};
	
		float m_x;
		float m_y;
		float m_z;
		float m_w;
	};

	typedef quaternion Quaternion;
};

#include <wgd/matrix.h>
#include <wgd/vector.h>

void wgd::quaternion::createMatrix(wgd::matrix &m) const
{
	m.m[0][0] = 1.0f - 2.0f * ( m_y * m_y + m_z * m_z );
	m.m[1][0] = 2.0f * ( m_x * m_y - m_z * m_w );
	m.m[2][0] = 2.0f * ( m_x * m_z + m_y * m_w );
	m.m[3][0] = 0.0;

	m.m[0][1] = 2.0f * (m_x * m_y + m_z * m_w);
	m.m[1][1] = 1.0f - 2.0f * ( m_x * m_x + m_z * m_z );
	m.m[2][1] = 2.0f * ( m_y * m_z - m_x * m_w );
	m.m[3][1] = 0.0;

	m.m[0][2] = 2.0f * (m_x * m_z - m_y * m_w);
	m.m[1][2] = 2.0f * (m_z * m_y + m_x * m_w );
	m.m[2][2] = 1.0f - 2.0f * ( m_x * m_x + m_y * m_y );
	m.m[3][2] = 0.0;

	m.m[0][3] = 0.0;
	m.m[1][3] = 0.0;
	m.m[2][3] = 0.0;
	m.m[3][3] = 1.0;
};

void wgd::quaternion::createRotation(const vector3d &v){
	createRotation(v.x, v.y, v.z);
}

wgd::vector3d wgd::quaternion::pitchYawRoll(){
	
	//tan(yaw)   =  2(q1q2+q4q3)/ (q42 + q12 - q22- q32)
	//sin(pitch) = -2(q1q3-q4q2)
	//tan(roll)  =  2(q4q1+q2q3)/ (q42 - q12 - q22+ q32)
	
	float yaw = atan(2*(m_x*m_y+m_w*m_z)/ (m_w*m_y + m_x*m_y - m_y*m_y- m_z*m_y));
	float pitch = asin(-2*(m_x*m_z-m_w*m_y));
	float roll = atan(2*(m_w*m_x+m_y*m_z)/ (m_w*m_y - m_x*m_y - m_y*m_y+ m_z*m_y));
	
	return wgd::vector3d(pitch, yaw, roll);
	
};

#endif
