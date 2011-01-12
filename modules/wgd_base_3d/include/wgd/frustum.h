#ifndef _WGD_FRUSTUM_
#define _WGD_FRUSTUM_

#include <wgd/vector.h>

namespace wgd {
	class Camera3D;
	/**
	 *	View frustum used for culling
	 */
	class BASE3DIMPORT Frustum {
		public:
		
		void create(Camera3D* cam);
		
		/*	Clip flags
		 *	plane    index     clip flag
		 *	-----    -----     ---------
		 *	Completely outside = 0x00
		 *  inside	   -         0x01	- this is true if it is completely inside
		 *	top        0         0x02	- The rest are true if it is inside or intersects this plane
		 *	bottom     1         0x04
		 *	right      2         0x08
		 *	left       3         0x10
		 *	near       4         0x20
		 *	far        5         0x40
		 *
		 *	Outside = 0
		 *	Inside  > 0
		 */
		 
		 
		/**	Is a point inside the frustum 
		 * @param v The point to be tested
		 * @param flags Clip flags of any parent in a heirachy to avoid some tests
		 */
		int inside(const vector3d &v, int flags=0) const;
		/**	Is an AABB inside the frustum	*/
		int inside(const vector3d &min, const vector3d &max, int flags=0) const;
		
		private:
		
		//a frustum has 6 planes: { top, bottom, right, left, near, far }
		struct Plane {
			wgd::vector3d normal;
			float d;
		} plane[6];
		
		//point inside a plane
		bool inside(const Plane &plane, const wgd::vector3d &point) const;
		//AABB inside plane (0=outside, 1=inside, 2=intersect)
		int inside(const Plane &plane, const vector3d &min, const vector3d &max) const;
	};
};

#endif
