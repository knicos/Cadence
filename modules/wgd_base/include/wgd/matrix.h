#ifndef _MATRIX_
#define _MATRIX_

#include <iostream>
#include <math.h>
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <wgd/dll.h>

#ifndef __SSE__
#define __SSE__
#endif

#undef __SSE__

#ifdef __SSE__
#include <xmmintrin.h>
#endif

namespace wgd {
	
	class vector3d;

	#ifdef _MSC_VER
	__declspec(align(16)) //class matrix {
	#endif
	/**
	 * Representation of a matrix with operators. This class provides fast matrix operations
	 * that are needed for transforming animated models. It requires Intel SSE which means you
	 * need a Pentium 3 or better.
	 */
	class BASEIMPORT matrix {
	//#endif
		public:

		union {
		#ifdef __SSE__
		struct {
		 __m128 _L1, _L2, _L3, _L4;
		};
		#endif

		float m[4][4];
		};

		/** Identity constructor. */
		matrix(){ identity(); };
		
		/** Create matrix from three vectors (TBN) */
		matrix(const vector3d &tangent, const vector3d &binormal, const vector3d &normal);
		
		/**
		 * Constructor from OpenGL matrix.
		 */
		matrix(float *mat){
			#ifndef __SSE__
			//for(int i=0; i<4; i++)
			//	for(int j=0; j<4; j++)
			//		m[i][j] = mat[i*4 + j];

			m[0][0] = mat[0];
			m[0][1] = mat[1];
			m[0][2] = mat[2];
			m[0][3] = mat[3];

			m[1][0] = mat[4];
			m[1][1] = mat[5];
			m[1][2] = mat[6];
			m[1][3] = mat[7];

			m[2][0] = mat[8];
			m[2][1] = mat[9];
			m[2][2] = mat[10];
			m[2][3] = mat[11];

			m[3][0] = mat[12];
			m[3][1] = mat[13];
			m[3][2] = mat[14];
			m[3][3] = mat[15];

			#else
			_L1 = _mm_load_ps(mat);
			_L2 = _mm_load_ps(&mat[4]);
			_L3 = _mm_load_ps(&mat[8]);
			_L4 = _mm_load_ps(&mat[12]);
			#endif
		}

		void apply() const {
			#ifdef _MSC_VER
			__declspec(align(16)) float mat[16];
			#else
			float mat[16] __attribute__((aligned(16)));
			#endif
			toOpenGL(mat);
			glPushMatrix();
			glMultMatrixf(mat);
		}

		void unapply() const {
			glPopMatrix();
		}
		
		/**
		 * matrix multiplication operator. This allows a*b matrix multiplication, however it is faster
		 * to use fastMultiply() instead of this.
		 */
		matrix operator*(const matrix &b) const {
			#ifndef __SSE__
			matrix result;
			result.m[0][0] = m[0][0]*b.m[0][0] + m[1][0]*b.m[0][1] + m[2][0]*b.m[0][2] + m[3][0]*b.m[0][3];
			result.m[0][1] = m[0][1]*b.m[0][0] + m[1][1]*b.m[0][1] + m[2][1]*b.m[0][2] + m[3][1]*b.m[0][3];
			result.m[0][2] = m[0][2]*b.m[0][0] + m[1][2]*b.m[0][1] + m[2][2]*b.m[0][2] + m[3][2]*b.m[0][3];
			result.m[0][3] = m[0][3]*b.m[0][0] + m[1][3]*b.m[0][1] + m[2][3]*b.m[0][2] + m[3][3]*b.m[0][3];

			result.m[1][0] = m[0][0]*b.m[1][0] + m[1][0]*b.m[1][1] + m[2][0]*b.m[1][2] + m[3][0]*b.m[1][3];
			result.m[1][1] = m[0][1]*b.m[1][0] + m[1][1]*b.m[1][1] + m[2][1]*b.m[1][2] + m[3][1]*b.m[1][3];
			result.m[1][2] = m[0][2]*b.m[1][0] + m[1][2]*b.m[1][1] + m[2][2]*b.m[1][2] + m[3][2]*b.m[1][3];
			result.m[1][3] = m[0][3]*b.m[1][0] + m[1][3]*b.m[1][1] + m[2][3]*b.m[1][2] + m[3][3]*b.m[1][3];

			result.m[2][0] = m[0][0]*b.m[2][0] + m[1][0]*b.m[2][1] + m[2][0]*b.m[2][2] + m[3][0]*b.m[2][3];
			result.m[2][1] = m[0][1]*b.m[2][0] + m[1][1]*b.m[2][1] + m[2][1]*b.m[2][2] + m[3][1]*b.m[2][3];
			result.m[2][2] = m[0][2]*b.m[2][0] + m[1][2]*b.m[2][1] + m[2][2]*b.m[2][2] + m[3][2]*b.m[2][3];
			result.m[2][3] = m[0][3]*b.m[2][0] + m[1][3]*b.m[2][1] + m[2][3]*b.m[2][2] + m[3][3]*b.m[2][3];

			result.m[3][0] = m[0][0]*b.m[3][0] + m[1][0]*b.m[3][1] + m[2][0]*b.m[3][2] + m[3][0]*b.m[3][3];
			result.m[3][1] = m[0][1]*b.m[3][0] + m[1][1]*b.m[3][1] + m[2][1]*b.m[3][2] + m[3][1]*b.m[3][3];
			result.m[3][2] = m[0][2]*b.m[3][0] + m[1][2]*b.m[3][1] + m[2][2]*b.m[3][2] + m[3][2]*b.m[3][3];
			result.m[3][3] = m[0][3]*b.m[3][0] + m[1][3]*b.m[3][1] + m[2][3]*b.m[3][2] + m[3][3]*b.m[3][3];

			return result;
			#else

			matrix c;
			__m128 _T1,_T2,_T3,_T4;

			_T1 = _mm_load1_ps(&b.m[0][0]);
			_T1 = _mm_mul_ps(_T1,_L1);
			_T2 = _mm_load1_ps(&b.m[0][1]);
			_T2 = _mm_mul_ps(_T2,_L2);
			_T3 = _mm_load1_ps(&b.m[0][2]);
			_T3 = _mm_mul_ps(_T3,_L3);
			_T4 = _mm_load1_ps(&b.m[0][3]);
			_T4 = _mm_mul_ps(_T4,_L4);
			c._L1 = _mm_add_ps(_T1,_T2);
			c._L1 = _mm_add_ps(c._L1,_T3);
			c._L1 = _mm_add_ps(c._L1,_T4);

			_T1 = _mm_load1_ps(&b.m[1][0]);
			_T1 = _mm_mul_ps(_T1,_L1);
			_T2 = _mm_load1_ps(&b.m[1][1]);
			_T2 = _mm_mul_ps(_T2,_L2);
			_T3 = _mm_load1_ps(&b.m[1][2]);
			_T3 = _mm_mul_ps(_T3,_L3);
			_T4 = _mm_load1_ps(&b.m[1][3]);
			_T4 = _mm_mul_ps(_T4,_L4);
			c._L2 = _mm_add_ps(_T1,_T2);
			c._L2 = _mm_add_ps(c._L2,_T3);
			c._L2 = _mm_add_ps(c._L2,_T4);

			_T1 = _mm_load1_ps(&b.m[2][0]);
			_T1 = _mm_mul_ps(_T1,_L1);
			_T2 = _mm_load1_ps(&b.m[2][1]);
			_T2 = _mm_mul_ps(_T2,_L2);
			_T3 = _mm_load1_ps(&b.m[2][2]);
			_T3 = _mm_mul_ps(_T3,_L3);
			_T4 = _mm_load1_ps(&b.m[2][3]);
			_T4 = _mm_mul_ps(_T4,_L4);
			c._L3 = _mm_add_ps(_T1,_T2);
			c._L3 = _mm_add_ps(c._L3,_T3);
			c._L3 = _mm_add_ps(c._L3,_T4);

			_T1 = _mm_load1_ps(&b.m[3][0]);
			_T1 = _mm_mul_ps(_T1,_L1);
			_T2 = _mm_load1_ps(&b.m[3][1]);
			_T2 = _mm_mul_ps(_T2,_L2);
			_T3 = _mm_load1_ps(&b.m[3][2]);
			_T3 = _mm_mul_ps(_T3,_L3);
			_T4 = _mm_load1_ps(&b.m[3][3]);
			_T4 = _mm_mul_ps(_T4,_L4);
			c._L4 = _mm_add_ps(_T1,_T2);
			c._L4 = _mm_add_ps(c._L4,_T3);
			c._L4 = _mm_add_ps(c._L4,_T4);

			return c;

			#endif
		};

		/**
		 * High performance matrix multiplication. Instead of returning the matrix it takes a non-constant
		 * reference that will be filled with the result. result = a*b. Note that this is row major matrix
		 * multiplication unlike OpenGL, therefore you may need to multiply in the opposite order.
		 * @param result Location for the result matrix.
		 * @param a Left hand operand.
		 * @param b Right hand operand.
		 */
		static void fastMultiply(matrix &result, const matrix &a, const matrix &b) {
			#ifndef __SSE__

			result.m[0][0] = a.m[0][0]*b.m[0][0] + a.m[1][0]*b.m[0][1] + a.m[2][0]*b.m[0][2] + a.m[3][0]*b.m[0][3];
			result.m[0][1] = a.m[0][1]*b.m[0][0] + a.m[1][1]*b.m[0][1] + a.m[2][1]*b.m[0][2] + a.m[3][1]*b.m[0][3];
			result.m[0][2] = a.m[0][2]*b.m[0][0] + a.m[1][2]*b.m[0][1] + a.m[2][2]*b.m[0][2] + a.m[3][2]*b.m[0][3];
			result.m[0][3] = a.m[0][3]*b.m[0][0] + a.m[1][3]*b.m[0][1] + a.m[2][3]*b.m[0][2] + a.m[3][3]*b.m[0][3];

			result.m[1][0] = a.m[0][0]*b.m[1][0] + a.m[1][0]*b.m[1][1] + a.m[2][0]*b.m[1][2] + a.m[3][0]*b.m[1][3];
			result.m[1][1] = a.m[0][1]*b.m[1][0] + a.m[1][1]*b.m[1][1] + a.m[2][1]*b.m[1][2] + a.m[3][1]*b.m[1][3];
			result.m[1][2] = a.m[0][2]*b.m[1][0] + a.m[1][2]*b.m[1][1] + a.m[2][2]*b.m[1][2] + a.m[3][2]*b.m[1][3];
			result.m[1][3] = a.m[0][3]*b.m[1][0] + a.m[1][3]*b.m[1][1] + a.m[2][3]*b.m[1][2] + a.m[3][3]*b.m[1][3];

			result.m[2][0] = a.m[0][0]*b.m[2][0] + a.m[1][0]*b.m[2][1] + a.m[2][0]*b.m[2][2] + a.m[3][0]*b.m[2][3];
			result.m[2][1] = a.m[0][1]*b.m[2][0] + a.m[1][1]*b.m[2][1] + a.m[2][1]*b.m[2][2] + a.m[3][1]*b.m[2][3];
			result.m[2][2] = a.m[0][2]*b.m[2][0] + a.m[1][2]*b.m[2][1] + a.m[2][2]*b.m[2][2] + a.m[3][2]*b.m[2][3];
			result.m[2][3] = a.m[0][3]*b.m[2][0] + a.m[1][3]*b.m[2][1] + a.m[2][3]*b.m[2][2] + a.m[3][3]*b.m[2][3];

			result.m[3][0] = a.m[0][0]*b.m[3][0] + a.m[1][0]*b.m[3][1] + a.m[2][0]*b.m[3][2] + a.m[3][0]*b.m[3][3];
			result.m[3][1] = a.m[0][1]*b.m[3][0] + a.m[1][1]*b.m[3][1] + a.m[2][1]*b.m[3][2] + a.m[3][1]*b.m[3][3];
			result.m[3][2] = a.m[0][2]*b.m[3][0] + a.m[1][2]*b.m[3][1] + a.m[2][2]*b.m[3][2] + a.m[3][2]*b.m[3][3];
			result.m[3][3] = a.m[0][3]*b.m[3][0] + a.m[1][3]*b.m[3][1] + a.m[2][3]*b.m[3][2] + a.m[3][3]*b.m[3][3];
			#else

			__m128 _T1,_T2,_T3,_T4;

			_T1 = _mm_load1_ps(&b.m[0][0]);
			_T1 = _mm_mul_ps(_T1,a._L1);
			_T2 = _mm_load1_ps(&b.m[0][1]);
			_T2 = _mm_mul_ps(_T2,a._L2);
			_T3 = _mm_load1_ps(&b.m[0][2]);
			_T3 = _mm_mul_ps(_T3,a._L3);
			_T4 = _mm_load1_ps(&b.m[0][3]);
			_T4 = _mm_mul_ps(_T4,a._L4);
			result._L1 = _mm_add_ps(_T1,_T2);
			result._L1 = _mm_add_ps(result._L1,_T3);
			result._L1 = _mm_add_ps(result._L1,_T4);

			_T1 = _mm_load1_ps(&b.m[1][0]);
			_T1 = _mm_mul_ps(_T1,a._L1);
			_T2 = _mm_load1_ps(&b.m[1][1]);
			_T2 = _mm_mul_ps(_T2,a._L2);
			_T3 = _mm_load1_ps(&b.m[1][2]);
			_T3 = _mm_mul_ps(_T3,a._L3);
			_T4 = _mm_load1_ps(&b.m[1][3]);
			_T4 = _mm_mul_ps(_T4,a._L4);
			result._L2 = _mm_add_ps(_T1,_T2);
			result._L2 = _mm_add_ps(result._L2,_T3);
			result._L2 = _mm_add_ps(result._L2,_T4);

			_T1 = _mm_load1_ps(&b.m[2][0]);
			_T1 = _mm_mul_ps(_T1,a._L1);
			_T2 = _mm_load1_ps(&b.m[2][1]);
			_T2 = _mm_mul_ps(_T2,a._L2);
			_T3 = _mm_load1_ps(&b.m[2][2]);
			_T3 = _mm_mul_ps(_T3,a._L3);
			_T4 = _mm_load1_ps(&b.m[2][3]);
			_T4 = _mm_mul_ps(_T4,a._L4);
			result._L3 = _mm_add_ps(_T1,_T2);
			result._L3 = _mm_add_ps(result._L3,_T3);
			result._L3 = _mm_add_ps(result._L3,_T4);

			_T1 = _mm_load1_ps(&b.m[3][0]);
			_T1 = _mm_mul_ps(_T1,a._L1);
			_T2 = _mm_load1_ps(&b.m[3][1]);
			_T2 = _mm_mul_ps(_T2,a._L2);
			_T3 = _mm_load1_ps(&b.m[3][2]);
			_T3 = _mm_mul_ps(_T3,a._L3);
			_T4 = _mm_load1_ps(&b.m[3][3]);
			_T4 = _mm_mul_ps(_T4,a._L4);
			result._L4 = _mm_add_ps(_T1,_T2);
			result._L4 = _mm_add_ps(result._L4,_T3);
			result._L4 = _mm_add_ps(result._L4,_T4);

			#endif
		};
	
		/**
		 * Convert to an OpenGL matrix float array.
		 * @param glmat Pointer to 16 float array.
		 */
		void toOpenGL(float *glmat) const
		{
			#ifdef __SSE__
			_mm_store_ps(glmat, _L1);
			_mm_store_ps(&glmat[4], _L2);
			_mm_store_ps(&glmat[8], _L3);
			_mm_store_ps(&glmat[12], _L4);
			#else

			glmat[0] = m[0][0];
			glmat[1] = m[0][1];
			glmat[2] = m[0][2];
			glmat[3] = m[0][3];
	
			glmat[4] = m[1][0];
			glmat[5] = m[1][1];
			glmat[6] = m[1][2];
			glmat[7] = m[1][3];
	
			glmat[8] = m[2][0];
			glmat[9] = m[2][1];
			glmat[10] = m[2][2];
			glmat[11] = m[2][3];
	
			glmat[12] = m[3][0];
			glmat[13] = m[3][1];
			glmat[14] = m[3][2];
			glmat[15] = m[3][3];
			#endif
		}

		friend std::ostream &operator<<(std::ostream &os, const matrix &mat);
	
		/**
		 * Add two matrices together. e.g. a+b. Use fastAdd if you need higher performance.
		 */
		matrix operator+(const matrix &b) {
			matrix c;

			#ifdef __SSE__
			c._L1 = _mm_add_ps(_L1, b._L1);
			c._L2 = _mm_add_ps(_L2, b._L2);
			c._L3 = _mm_add_ps(_L3, b._L3);
			c._L4 = _mm_add_ps(_L4, b._L4);
			#else
			/*for (int i=0; i<4; i++)
			{
				for (int j=0; j<4; j++)
				{
					result.m[i][j] = m[i][j]+b.m[i][j];
				}
			}*/

			c.m[0][0] = m[0][0]+b.m[0][0];
			c.m[0][1] = m[0][1]+b.m[0][1];
			c.m[0][2] = m[0][2]+b.m[0][2];
			c.m[0][3] = m[0][3]+b.m[0][3];

			c.m[1][0] = m[1][0]+b.m[1][0];
			c.m[1][1] = m[1][1]+b.m[1][1];
			c.m[1][2] = m[1][2]+b.m[1][2];
			c.m[1][3] = m[1][3]+b.m[1][3];

			c.m[2][0] = m[2][0]+b.m[1][0];
			c.m[2][1] = m[2][1]+b.m[1][1];
			c.m[2][2] = m[2][2]+b.m[1][2];
			c.m[2][3] = m[2][3]+b.m[1][3];

			c.m[3][0] = m[3][0]+b.m[1][0];
			c.m[3][1] = m[3][1]+b.m[1][1];
			c.m[3][2] = m[3][2]+b.m[1][2];
			c.m[3][3] = m[3][3]+b.m[1][3];
			#endif
			return c;
		};

		/**
		 * Add two matrices together. Faster than operator as less temporaries are created.
		 * @param result Resulting matrix it put in here.
		 * @param a First matrix.
		 * @param b Second matrix.
		 */
		static void fastAdd(matrix &result, const matrix &a, const matrix &b) {
			#ifdef __SSE__
			result._L1 = _mm_add_ps(a._L1, b._L1);
			result._L2 = _mm_add_ps(a._L2, b._L2);
			result._L3 = _mm_add_ps(a._L3, b._L3);
			result._L4 = _mm_add_ps(a._L4, b._L4);
			#else
			/*for (int i=0; i<4; i++)
			{
				for (int j=0; j<4; j++)
				{
					result.m[i][j] = a.m[i][j]+b.m[i][j];
				}
			}*/

			result.m[0][0] = a.m[0][0]+b.m[0][0];
			result.m[0][1] = a.m[0][1]+b.m[0][1];
			result.m[0][2] = a.m[0][2]+b.m[0][2];
			result.m[0][3] = a.m[0][3]+b.m[0][3];

			result.m[1][0] = a.m[1][0]+b.m[1][0];
			result.m[1][1] = a.m[1][1]+b.m[1][1];
			result.m[1][2] = a.m[1][2]+b.m[1][2];
			result.m[1][3] = a.m[1][3]+b.m[1][3];

			result.m[2][0] = a.m[2][0]+b.m[1][0];
			result.m[2][1] = a.m[2][1]+b.m[1][1];
			result.m[2][2] = a.m[2][2]+b.m[1][2];
			result.m[2][3] = a.m[2][3]+b.m[1][3];

			result.m[3][0] = a.m[3][0]+b.m[1][0];
			result.m[3][1] = a.m[3][1]+b.m[1][1];
			result.m[3][2] = a.m[3][2]+b.m[1][2];
			result.m[3][3] = a.m[3][3]+b.m[1][3];
			#endif
		};
	
		/**
		 * Vector matrix multiplication operator. For higher performance use fastMultiply().
		 */
		inline wgd::vector3d operator*(const wgd::vector3d &b) const;

		/**
		 * Fast vector matrix multiplication.
		 * @param result Result vector is put in here.
		 * @param a matrix operand.
		 * @param b Vector operand.
		 */
		inline static void fastMultiply(wgd::vector3d &result, const wgd::matrix &a, const wgd::vector3d &b);
	
		/**
		 * Make this an identity matrix.
		 */
		void identity() {
			m[0][0] = 1;
			m[0][1] = 0;
			m[0][2] = 0;
			m[0][3] = 0;
			
			m[1][0] = 0;
			m[1][1] = 1;
			m[1][2] = 0;
			m[1][3] = 0;

			m[2][0] = 0;
			m[2][1] = 0;
			m[2][2] = 1;
			m[2][3] = 0;

			m[3][0] = 0;
			m[3][1] = 0;
			m[3][2] = 0;
			m[3][3] = 1;
		};
		
		/** Is this matrix the identity */
		bool isIdentity() const {
			if(m[0][0]!=1.0 || m[0][1]!=0.0 || m[0][2]!=0.0 || m[0][3]!=0.0) return false;
			if(m[1][0]!=0.0 || m[1][1]!=1.0 || m[1][2]!=0.0 || m[1][3]!=0.0) return false;
			if(m[2][0]!=0.0 || m[2][1]!=0.0 || m[2][2]!=1.0 || m[2][3]!=0.0) return false;
			if(m[3][0]!=0.0 || m[3][1]!=0.0 || m[3][2]!=0.0 || m[3][3]!=1.0) return false;
			return true;
		}
		
		/**
		 * The transpose of this matrix.
		 * @return The transposed matrix.
		 */
		inline matrix transpose();

		/**
		 * Fast matrix transpose.
		 * @param res The transposed matrix is put in here.
		 * @param m The matrix to transpose.
		 */
		inline static void fastTranspose(wgd::matrix &res, const wgd::matrix &m);
		
		
		static bool fastInverse(matrix & dest, const matrix& src);
		/**
		 * Make this a translation matrix.
		 * @param pos XYZ position vector.
		 */
		inline void translate(const vector3d &v);
	
		/**
		 * Make this a scale matrix.
		 * @param scale XYZ scale vector.
		 */
		inline void scale(const wgd::vector3d &v);
	
		/**
		 * Make this a rotation matrix.
		 * @param v Pitch, Yaw, Roll vector.
		 */
		inline void rotate(const wgd::vector3d &v);
		
		/**
		 * Make this a rotation matrix around an arbituaty axis.
		 * @param axis The axis to rotate around.
		 * @param angle The angle to rotate in radians.
		 */
		inline void rotateAxis(const wgd::vector3d &axis, float angle);
		
		/**
		 * Make this a translation matrix.
		 */
		inline void translate(float x, float y, float z);
	
		/**
		 * Make this a scale matrix.
		 */
		inline void scale(float x, float y, float z);
	
		/**
		 * Make this a rotation matrix.
		 * @param x Pitch
		 * @param y Yaw
		 * @param z Roll
		 */
		inline void rotate(float x, float y, float z);

		/**	Creates a transformation matrix that looks at a target point from a location
		 *	@param position The matrix origin
		 *	@param target The location the matrix points at
		 *	@param up The up vector - must be a unit vector, must not be parallel to the direction.
		 */
		inline void lookat(const vector3d& position, const vector3d &direction, const vector3d &up);

	#ifdef _MSC_VER	
	};
	#else
	} __attribute__((aligned(16),packed));
	#endif
};

#include <wgd/vector.h>

#undef __SSE__

wgd::vector3d wgd::matrix::operator*(const wgd::vector3d &b) const {
	#ifndef __SSE__
	return wgd::vector3d(
		m[0][0]*b.x + m[1][0]*b.y + m[2][0]*b.z + m[3][0],
		m[0][1]*b.x + m[1][1]*b.y + m[2][1]*b.z + m[3][1],
		m[0][2]*b.x + m[1][2]*b.y + m[2][2]*b.z + m[3][2],
		m[0][3]*b.x + m[1][3]*b.y + m[2][3]*b.z + m[3][3]);
	#else
	wgd::vector3d c;
	__m128 _T1, _T2, _X, _Y, _Z;
	_T1 = b._L1;
	_Z = _mm_shuffle_ps(_T1,_T1,_MM_SHUFFLE(2,2,2,2));
	_Y = _mm_shuffle_ps(_T1,_T1,_MM_SHUFFLE(1,1,1,1));
	_X = _mm_shuffle_ps(_T1,_T1,_MM_SHUFFLE(0,0,0,0));
	//c._L1 = _L1*_X + _L2*_Y + _L3*_Z + _L4;
	_T1 = _mm_mul_ps(_L1,_X);
	_T2 = _mm_mul_ps(_L2,_Y);
	_T1 = _mm_add_ps(_T1,_T2);
	_T2 = _mm_mul_ps(_L3,_Z);
	_T1 = _mm_add_ps(_T1,_T2);
	c._L1 = _mm_add_ps(_T1,_L4);
	return c;
	#endif
};

void wgd::matrix::fastMultiply(wgd::vector3d &result, const wgd::matrix &a, const wgd::vector3d &b) {
	#ifndef __SSE__
	result.x = a.m[0][0]*b.x + a.m[1][0]*b.y + a.m[2][0]*b.z + a.m[3][0];
	result.y = a.m[0][1]*b.x + a.m[1][1]*b.y + a.m[2][1]*b.z + a.m[3][1];
	result.z = a.m[0][2]*b.x + a.m[1][2]*b.y + a.m[2][2]*b.z + a.m[3][2];
	result.w = a.m[0][3]*b.x + a.m[1][3]*b.y + a.m[2][3]*b.z + a.m[3][3];

	#else
	__m128 _T1, _T2, _X, _Y, _Z;
	_T1 = b._L1;
	//_Z = _mm_shuffle_ps(_T1,_T1,_MM_SHUFFLE(2,3,0,1));
	_Z = _mm_shuffle_ps(_T1,_T1,_MM_SHUFFLE(2,2,2,2));
	_Y = _mm_shuffle_ps(_T1,_T1,_MM_SHUFFLE(1,1,1,1));
	_X = _mm_shuffle_ps(_T1,_T1,_MM_SHUFFLE(0,0,0,0));
	//result._L1 = a._L1*_X + a._L2*_Y + a._L3*_Z + a._L4;
	_T1 = _mm_mul_ps(a._L1,_X);
	_T2 = _mm_mul_ps(a._L2,_Y);
	_T1 = _mm_add_ps(_T1,_T2);
	_T2 = _mm_mul_ps(a._L3,_Z);
	_T1 = _mm_add_ps(_T1,_T2);
	result._L1 = _mm_add_ps(_T1,a._L4);
	#endif
};

wgd::matrix wgd::matrix::transpose() {
	matrix r;
	r.m[0][0] = m[0][0];
	r.m[1][0] = m[0][1];
	r.m[2][0] = m[0][2];
	r.m[3][0] = m[0][3];

	r.m[0][1] = m[1][0];
	r.m[1][1] = m[1][1];
	r.m[2][1] = m[1][2];
	r.m[3][1] = m[1][3];

	r.m[0][2] = m[2][0];
	r.m[1][2] = m[2][1];
	r.m[2][2] = m[2][2];
	r.m[3][2] = m[2][3];

	r.m[0][3] = m[3][0];
	r.m[1][3] = m[3][1];
	r.m[2][3] = m[3][2];
	r.m[3][3] = m[3][3];

	return r;
}

void wgd::matrix::fastTranspose(wgd::matrix &res, const wgd::matrix &m) {
	res.m[0][0] = m.m[0][0];
	res.m[1][0] = m.m[0][1];
	res.m[2][0] = m.m[0][2];
	res.m[3][0] = m.m[0][3];

	res.m[0][1] = m.m[1][0];
	res.m[1][1] = m.m[1][1];
	res.m[2][1] = m.m[1][2];
	res.m[3][1] = m.m[1][3];

	res.m[0][2] = m.m[2][0];
	res.m[1][2] = m.m[2][1];
	res.m[2][2] = m.m[2][2];
	res.m[3][2] = m.m[2][3];

	res.m[0][3] = m.m[3][0];
	res.m[1][3] = m.m[3][1];
	res.m[2][3] = m.m[3][2];
	res.m[3][3] = m.m[3][3];
}

void wgd::matrix::translate(const vector3d &v) { translate(v.x, v.y, v.z); }

void wgd::matrix::scale(const wgd::vector3d &v) { scale(v.x, v.y, v.z); }

void wgd::matrix::rotate(const wgd::vector3d &v) { rotate(v.x, v.y, v.z); }

void wgd::matrix::translate(float x, float y, float z) {
	identity();
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
};

void wgd::matrix::scale(float x, float y, float z) {
	identity();
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
};

void wgd::matrix::rotate(float x, float y, float z) {
	wgd::matrix c;
	c.identity();
	c.m[0][0] = cos(z);
	c.m[1][0] = -sin(z);
	c.m[0][1] = sin(z);
	c.m[1][1] = cos(z);

	wgd::matrix d;
	d.identity();
	d.m[1][1] = cos(x);
	d.m[2][1] = -sin(x);
	d.m[1][2] = sin(x);
	d.m[2][2] = cos(x);

	c = d*c;
	d.identity();
	d.m[0][0] = cos(y);
	d.m[2][0] = sin(y);
	d.m[0][2] = -sin(y);
	d.m[2][2] = cos(y);

	*this = d*c;
};

void wgd::matrix::rotateAxis(const wgd::vector3d &axis, float angle){
	vector3d v = axis.normalise();
	identity();
	
	float s = sin(angle);
	float c = cos(angle);

	float xx = v.x * v.x;
	float yy = v.y * v.y;
	float zz = v.z * v.z;
	float xy = v.x * v.y;
	float yz = v.y * v.z;
	float zx = v.z * v.x;
	float xs = v.x * s;
	float ys = v.y * s;
	float zs = v.z * s;
	float one_c = 1.0f - c;
	
	m[0][0] = (one_c * xx) + c;
	m[1][0] = (one_c * xy) - zs;
	m[2][0] = (one_c * zx) + ys;

	m[0][1] = (one_c * xy) + zs;
	m[1][1] = (one_c * yy) + c;
	m[2][1] = (one_c * yz) - xs;

	m[0][2] = (one_c * zx) - ys;
	m[1][2] = (one_c * yz) + xs;
	m[2][2] = (one_c * zz) + c;
};


void wgd::matrix::lookat(const vector3d& position, const wgd::vector3d &target, const wgd::vector3d &up) {
	wgd::vector3d dir = (target - position).normalise();
	wgd::vector3d t = wgd::vector3d::crossProduct(up, dir);
	wgd::vector3d b = wgd::vector3d::crossProduct(dir, t);
	matrix p; p.translate(position);
	matrix r(t, b, dir);
	
	fastMultiply(*this, r, p);
}

#endif
