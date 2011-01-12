#include <wgd/matrix.h>

using namespace wgd;

bool matrix::fastInverse(matrix & dest, const matrix& src){
	//	6*8+2*6 = 60 multiplications
	//		2*1 =  2 divisions
	float r0[2][2], r1[2][2], r2[2][2], r3[2][2];
	
	float a, det, invDet;

	// r0 = m0.Inverse();
	det = src.m[0][0] * src.m[1][1] - src.m[0][1] * src.m[1][0];

	if ( fabs( det ) < 1e-14 ) {
		return false;
	}

	invDet = 1.0f / det;

	r0[0][0] =   src.m[1][1] * invDet;
	r0[0][1] = - src.m[0][1] * invDet;
	r0[1][0] = - src.m[1][0] * invDet;
	r0[1][1] =   src.m[0][0] * invDet;

	// r1 = r0 * m1;
	r1[0][0] = r0[0][0] * src.m[0][2] + r0[0][1] * src.m[1][2];
	r1[0][1] = r0[0][0] * src.m[0][3] + r0[0][1] * src.m[1][3];
	r1[1][0] = r0[1][0] * src.m[0][2] + r0[1][1] * src.m[1][2];
	r1[1][1] = r0[1][0] * src.m[0][3] + r0[1][1] * src.m[1][3];

	// r2 = m2 * r1;
	r2[0][0] = src.m[2][0] * r1[0][0] + src.m[2][1] * r1[1][0];
	r2[0][1] = src.m[2][0] * r1[0][1] + src.m[2][1] * r1[1][1];
	r2[1][0] = src.m[3][0] * r1[0][0] + src.m[3][1] * r1[1][0];
	r2[1][1] = src.m[3][0] * r1[0][1] + src.m[3][1] * r1[1][1];

	// r3 = r2 - m3;
	r3[0][0] = r2[0][0] - src.m[2][2];
	r3[0][1] = r2[0][1] - src.m[2][3];
	r3[1][0] = r2[1][0] - src.m[3][2];
	r3[1][1] = r2[1][1] - src.m[3][3];

	// r3.InverseSelf();
	det = r3[0][0] * r3[1][1] - r3[0][1] * r3[1][0];

	if ( fabs( det ) < 1e-14 ) {
		return false;
	}

	invDet = 1.0f / det;

	a = r3[0][0];
	r3[0][0] =   r3[1][1] * invDet;
	r3[0][1] = - r3[0][1] * invDet;
	r3[1][0] = - r3[1][0] * invDet;
	r3[1][1] =   a * invDet;

	// r2 = m2 * r0;
	r2[0][0] = src.m[2][0] * r0[0][0] + src.m[2][1] * r0[1][0];
	r2[0][1] = src.m[2][0] * r0[0][1] + src.m[2][1] * r0[1][1];
	r2[1][0] = src.m[3][0] * r0[0][0] + src.m[3][1] * r0[1][0];
	r2[1][1] = src.m[3][0] * r0[0][1] + src.m[3][1] * r0[1][1];

	//	FILL DESTINATION MATRIX //

	// m2 = r3 * r2;
	dest.m[2][0] = r3[0][0] * r2[0][0] + r3[0][1] * r2[1][0];
	dest.m[2][1] = r3[0][0] * r2[0][1] + r3[0][1] * r2[1][1];
	dest.m[3][0] = r3[1][0] * r2[0][0] + r3[1][1] * r2[1][0];
	dest.m[3][1] = r3[1][0] * r2[0][1] + r3[1][1] * r2[1][1];

	// m0 = r0 - r1 * m2;
	dest.m[0][0] = r0[0][0] - r1[0][0] * dest.m[2][0] - r1[0][1] * dest.m[3][0];
	dest.m[0][1] = r0[0][1] - r1[0][0] * dest.m[2][1] - r1[0][1] * dest.m[3][1];
	dest.m[1][0] = r0[1][0] - r1[1][0] * dest.m[2][0] - r1[1][1] * dest.m[3][0];
	dest.m[1][1] = r0[1][1] - r1[1][0] * dest.m[2][1] - r1[1][1] * dest.m[3][1];

	// m1 = r1 * r3;
	dest.m[0][2] = r1[0][0] * r3[0][0] + r1[0][1] * r3[1][0];
	dest.m[0][3] = r1[0][0] * r3[0][1] + r1[0][1] * r3[1][1];
	dest.m[1][2] = r1[1][0] * r3[0][0] + r1[1][1] * r3[1][0];
	dest.m[1][3] = r1[1][0] * r3[0][1] + r1[1][1] * r3[1][1];

	// m3 = -r3;
	dest.m[2][2] = -r3[0][0];
	dest.m[2][3] = -r3[0][1];
	dest.m[3][2] = -r3[1][0];
	dest.m[3][3] = -r3[1][1];

	return true;
}


matrix::matrix(const vector3d &tangent, const vector3d &binormal, const vector3d &normal) {
	m[0][0] = tangent.x;
	m[0][1] = tangent.y;
	m[0][2] = tangent.z;
	m[0][3] = 0;
	
	m[1][0] = binormal.x;
	m[1][1] = binormal.y;
	m[1][2] = binormal.z;
	m[1][3] = 0;
	
	m[2][0] = normal.x;
	m[2][1] = normal.y;
	m[2][2] = normal.z;
	m[2][3] = 0;
	
	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1.0f;
}
