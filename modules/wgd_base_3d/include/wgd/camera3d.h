#ifndef _WGD_CAMERA3D_
#define _WGD_CAMERA3D_

#include <wgd/camera.h>
#include <wgd/frustum.h>
#include <wgd/vector.h>
#include <cadence/doste/definition.h>



namespace wgd {
	class BASE3DIMPORT Camera3D : public wgd::Camera {
		public:
		Camera3D(const cadence::doste::OID&);
		~Camera3D();
		
		void bind();
		void unbind();
		
		/** Position of camera in the 3D world*/
		PROPERTY_WF(vector3d, position, ix::position);
		PROPERTY_RF(vector3d, position, ix::position);
		
		/** Move the camera relative to its position */
		void move(const vector3d&);
		/** Move the camera relative to its position and orientation */
		void translate(const vector3d&);
		
		/** Set the pitch yaw and roll of the camera */
		PROPERTY_WF(vector3d, orientation, ix::orientation);
		PROPERTY_RF(vector3d, orientation, ix::orientation);
		
		/** Rotate the camera using pitch, yaw and roll */
		void rotate(const vector3d&);
		
		/** Get the quaternion of the camera's orientation */
		const Quaternion quaternion() const;
		/** Set the camera orientation using a quaternion */
		void quaternion(const Quaternion&);
		
		/** Make the camera look at a point, modify roll to change the 'up' vector */
		void lookat(const vector3d&);
		/** get a vector describing the direction the camera is pointing */
		vector3d direction() { return vector3d().translate(m_quat, vector3d(0.0, 0.0, -1.0)); }
		
		/** clipping */
		#ifdef WIN32
		#undef near
		#undef far
		#endif
		PROPERTY_WF(float, nearclip, ix::near);
		PROPERTY_RF(float, nearclip, ix::near);
		
		PROPERTY_WF(float, farclip, ix::far);
		PROPERTY_RF(float, farclip, ix::far);
		
		/** Field of view in X */
		PROPERTY_WF(float, fov, ix::fov);
		PROPERTY_RF(float, fov, ix::fov);
		
		/** Field of view in Y */
		float fovY() { return fov() / ((float)m_width / m_height); };
				
		BEGIN_EVENTS(wgd::Camera);
		EVENT_VECTOR(evt_orientation, (*this)(ix::orientation));
		END_EVENTS;
		
		OBJECT(Camera, Camera3D);
		
		//Frustum clipping
		int insideFrustum(const wgd::vector3d &point) const { return m_frustum.inside(point); }
		int insideFrustum(const wgd::vector3d &min, const wgd::vector3d &max) const { return m_frustum.inside(min, max); }
		
		/** Project a point from world coordinates to screen coordinates
		 * @param world the world position of the point
		 * @return screen coordinates relative to viewport widget
		 */
		wgd::vector3d project(const vector3d &world);
		/** transform a point from screen coordinates to world coordinates
		 *  
		 * @param screen the screen coordinates relative to the viewport. 
		 * @return the position in the world
		 */
		wgd::vector3d unProject(const vector3d &screen);
		
		
		private:
		//calculate the OpenGL for manual projection
		//should really be one call to get the product of them, 
		//but i couldn't get that to work properly.
		void projectionMatrix(double*);
		void modelViewMatrix(double*);
		
		Quaternion m_quat;
		
		//frustum for view culling
		wgd::Frustum m_frustum;
	};
};

#endif
