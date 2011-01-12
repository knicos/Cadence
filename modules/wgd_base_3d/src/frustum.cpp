#include <wgd/frustum.h>
#include <wgd/camera3d.h>
#include <wgd/math.h>
#include <GL/gl.h>

using namespace wgd;

void Frustum::create(Camera3D *c) {

	//	Get camera up, forward and left vectors
	vector3d zero;
	vector3d up			= zero.translate(c->quaternion().conjugate(), vector3d(0.0, 1.0, 0.0));
	vector3d forward	= zero.translate(c->quaternion().conjugate(), vector3d(0.0, 0.0, -1.0));
	vector3d left		= zero.translate(c->quaternion().conjugate(), vector3d(-1.0, 0.0, 0.0));
	vector3d position	= c->position();

	//	Get camera perspective
	float a = degToRad(c->fovY() / 2.0f);
	float b = degToRad(c->fov()  / 2.0f);
	
	//	Get the plane normals
	matrix rotate; 
	rotate.rotateAxis(left, -a);
	plane[0].normal = rotate * up * -1;			//	TOP
	
	rotate.rotateAxis(left, a);
	plane[1].normal = rotate * up;			//	BOTTOM
	
	rotate.rotateAxis(up, -b);
	plane[2].normal = rotate * left;		//	RIGHT
	
	rotate.rotateAxis(up, b);
	plane[3].normal = rotate * left * -1;		//	LEFT
	
	plane[4].normal = forward;				//	NEAR
	plane[5].normal = forward * -1;				//	FAR
	
	//	Next calculate the plane offsets from the normals and the eye position.
	plane[0].d = -position.dot( plane[0].normal );
	plane[1].d = -position.dot( plane[1].normal );
	plane[2].d = -position.dot( plane[2].normal );
	plane[3].d = -position.dot( plane[3].normal );
	plane[4].d = -position.dot( plane[4].normal );
	
	//	for far, need the point on the far plane
	vector3d farPt = position + (forward * c->farclip());
	plane[5].d = -farPt.dot( plane[5].normal );
}

//	Frustum Tests	//

int Frustum::inside(const vector3d &point, int flags) const {
	//	Test each clip plane
	for (int i=0; i<6; i++) {
		//ignore if already inside this plane
		if(!(flags & (2<<i))) {
			if(inside(plane[i], point)) flags |= (2 << i);
		}
	}
	if(flags == 0x7e) flags |= 0x1;
	return flags;
};

int Frustum::inside(const vector3d &min, const vector3d &max, int flags) const {
	//	ignore if already completely inside frustum
	if(flags & 0x1) return flags;
	
	//activate bit 1 (completely inside) until proven false
	flags |= 0x1;
	
	//	Test each clip plane
	int side;
	for (int i=0; i<6; i++) {
		//ignore if already inside this plane
		if(!(flags & (2<<i))) {
			side = inside(plane[i], min, max);
			//Reject if entirely outside plane
			if(side==0) { flags=0; break; };
			//Activate flag if intersects
			if(side==2) flags |= (2 << i);
			//deactivate completely inside flag if not completely inside
			if(side!=1) flags &= 0xfe;
		}
	}
	return flags;
};


//	Individual planes	//

bool Frustum::inside(const Plane &p, const vector3d &v) const {
	float planeEqVal = p.d + p.normal.dot(v);
	return (planeEqVal >= 0);
}

int Frustum::inside(const Plane &plane, const vector3d &min, const vector3d &max) const {
	// Assemble accept / reject points based on the plane slope
	vector3d accept, reject;
	
	accept.x = (plane.normal.x > 0)? min.x: max.x;
	accept.y = (plane.normal.y > 0)? min.y: max.y;
	accept.z = (plane.normal.z > 0)? min.z: max.z;
	
	reject.x = (plane.normal.x > 0)? max.x: min.x;
	reject.y = (plane.normal.y > 0)? max.y: min.y;
	reject.z = (plane.normal.z > 0)? max.z: min.z;

	// If reject point is outside the clipping plane
	if(!inside(plane, reject)) return 0;
	// If accept point is inside the clipping plane
	if(inside(plane, accept)) return 1;
	// Otherwise intersected
	return 2;
}
