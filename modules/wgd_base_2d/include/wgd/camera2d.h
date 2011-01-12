#ifndef _WGD_CAMERA2D_
#define _WGD_CAMERA2D_

#include <wgd/camera.h>
#include <wgd/vector.h>

namespace wgd {
	class BASE2DIMPORT Camera2D : public wgd::Camera {
		public:
		OBJECT(Camera, Camera2D);
		Camera2D(const OID&);
		~Camera2D();
		
		void bind();
		void unbind();
		
		
		/** Position of camera in the world */
		PROPERTY_WF(vector2d, position, ix::position);
		PROPERTY_RF(vector2d, position, ix::position);
		
		/** Move the camera relative to its position */
		void move(const vector2d&);
		/** Move the camera relative to its position and angle */
		void translate(const vector2d&);
		
		/** The orientation of the camera in radians */
		PROPERTY_WF(float, orientation, ix::orientation);
		PROPERTY_RF(float, orientation, ix::orientation);
		
		/** Rotate the camera (radians) */
		void rotate(float);
		
		/** Set the camera zoom */
		PROPERTY_WF(float, zoom, ix::zoom);
		PROPERTY_RF(float, zoom, ix::zoom);		
		
		/** project a point in the scene to screen coordinates */
		vector2d project(const vector2d&);
		/** get the point in the scene at a point in screen coordinates */
		vector2d unproject(const vector2d&);
	};
};

#endif
