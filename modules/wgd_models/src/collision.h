//collision functions from book 'Real Time Collision Detection'

#ifndef _WGD_COLLISION_
#define _WGD_COLLISION_

namespace wgd {
	wgd::vector3d ClosectPointTriangle(const wgd::vector3d &p, const wgd::vector3d &a, const wgd::vector3d &b, const wgd::vector3d &c);
	int intersectLineTriangle(const wgd::vector3d &p, const wgd::vector3d &q, const wgd::vector3d &a, const wgd::vector3d &b, const wgd::vector3d &c, wgd::vector3d &result, wgd::vector3d &normal);
	int TestSegmentAABB(const wgd::vector3d &p0, const wgd::vector3d &p1, const wgd::vector3d &minAABB, const wgd::vector3d &maxAABB);
};

#endif
