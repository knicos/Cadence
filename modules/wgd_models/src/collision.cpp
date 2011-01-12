//collision functions from "Real Time Collision Detection" book

#include <wgd/vector.h>
#include "collision.h"

using namespace wgd;

// Closest point of a triangle to a point, page 141/2
vector3d wgd::ClosectPointTriangle(const vector3d &p, const vector3d &a, const vector3d &b, const vector3d &c) {
	// check if P in vertex region outside A
	vector3d ab = b - a;
	vector3d ac = c - a;
	
	vector3d ap = p - a;
	float d1 = ab.dot(ap);
	float d2 = ac.dot(ap);
	if(d1 <= 0.0f && d2 <= 0.0f) return a; //barycentric coordinates (1, 0, 0)
	
	// check if P in vertex region outside B
	vector3d bp = p - b;
	float d3 = ab.dot(bp);
	float d4 = ac.dot(bp);
	if(d3 >= 0.0f && d4 <= d3) return b; //barycentric coordinates (0, 1, 0)
	
	//Check if P in edge region of AB, if so, return projection of P onto AB
	float vc = d1*d4 - d3*d2;
	if(vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		return a + ab * v; //barycentric coordinates (1-v, v, 0)
	}
	
	// check if P in vertex region outside C
	vector3d cp = p - c;
	float d5 = ab.dot(cp);
	float d6 = ac.dot(cp);
	if(d6 >= 0.0f && d5 <= d6) return c; //barycentric coordinates (0, 0, 1)
	
	// check if P in edge region of AC, if so return projection of P onto AC
	float vb = d5*d2 - d1*d6;
	if(vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		return a + ac * w; //barycentric coordinates (1-w, 0, w)
	}
	
	// check if P in edge region of BC, if so return projection of P onto BC
	float va = d3*d6 - d5*d4;
	if(va <= 0.0f && (d4-d3) >= 0.0f && (d5-d6) >= 0.0f) {
		float w = (d4-d3) / ((d4-d3) + (d5-d6));
		return a + (c-b) * w; //barycentric coordinates (0, 1-w, w)
	}
	
	// P inside face region. compute Q through its barycentric coordinates (u, v, w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
	
}

// line segment intersects triangle page 191/2 (modified to return actual point)
int wgd::intersectLineTriangle(const vector3d &p, const vector3d &q, const vector3d &a, const vector3d &b, const vector3d &c, vector3d &result, vector3d &normal) {
	
	float t,u,v,w; // barycentric result
	
	vector3d ab = b - a;
	vector3d ac = c - a;
	vector3d qp = p - q;
	
	// Compute triangle normal
	vector3d n = ab.cross(ac);
	
	//compute denominator d. if d <= 0, line is parallel to or points away from triangle
	float d = qp.dot(n);
	if(d <= 0.0f) return 0;
	
	// compute intersection t value of pq with plane of triangle
	vector3d ap = p - a;
	t = ap.dot(n);
	if(t <= 0.0f) return 0; //intersection before line starts
	if(t > d) return 0; //intersection after line ends, leave out for ray tests
	
	// compute barycentric coordinate components and test if within bounds
	vector3d e = qp.cross(ap);
	v = ac.dot(e);
	if(v < 0.0f || v > d) return 0;
	w = -ab.dot(e);
	if(w < 0.0f || v + w > d) return 0;
	
	// segment/ray intersects triangle. perform delayed division and
	// compute the last barycentric coordinate component
	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f - v - w;
	
	// output stuff
	normal = n;
	result = a*u + b*v + c*w;
	
	return 1;
}

//Line segment - AABB test - page 183/4
#define EPSILON 0.00001f;
int wgd::TestSegmentAABB(const wgd::vector3d &p0, const wgd::vector3d &p1, const wgd::vector3d &min, const wgd::vector3d &max) {
	vector3d e = max - min;			//box halflength (*2 to save time)
	vector3d d = p1 - p0;			//segment halflength (*2 to save time)
	vector3d m = p0 + p1 - min - max;	//segment midpoint translated to origin
	
	//world coordinate as seperating axis
	float adx = abs(d.x);
	if(abs(m.x) > e.x + adx) return 0;
	float ady = abs(d.y);
	if(abs(m.y) > e.y + ady) return 0;
	float adz = abs(d.z);
	if(abs(m.z) > e.x + adz) return 0;
	
	//add epsilon to fix errors when near parallel
	adx+=EPSILON; ady+=EPSILON; adz+=EPSILON;
	
	//cross products of segment direction vector with coordinate axes
	if(abs(m.y*d.z - m.z * d.y) > e.y * adz + e.z * ady) return 0;
	if(abs(m.y*d.z - m.z * d.y) > e.y * adz + e.z * ady) return 0;
	if(abs(m.y*d.z - m.z * d.y) > e.y * adz + e.z * ady) return 0;
	//no seperating axis found
	return 1;
}
