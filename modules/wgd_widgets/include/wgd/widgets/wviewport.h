#ifndef _WGD_WVIEWPORT_
#define _WGD_WVIEWPORT_

#include <wgd/widgets/widgets.h>
#include <wgd/rendertarget.h>
#include <wgd/camera.h>
#include <wgd/scene.h>

namespace wgd {

	/**
	 * WViewport is a widget that acts as a special form of RenderTarget. You
	 * can either specify a source RenderTarget object (such as a Framebuffer) or
	 * directly specify a Camera and Scene. Using one of the above it will draw
	 * it to the screen in a rectangle given by the widgets position and size.
	 */
	class WViewport : public wgd::Widget {
		public:
		WViewport(const cadence::doste::OID &id);
		~WViewport();
		
		void draw(const ClipRect &);
		
		/** stuff from RenderTarget */
		
		PROPERTY_RF(RenderTarget, source, ix::source);
		PROPERTY_WF(RenderTarget, source, ix::source);		
		
		PROPERTY_RF(Camera, camera, ix::camera);
		PROPERTY_WF(Camera, camera, ix::camera);
		
		PROPERTY_RF(Scene, scene, ix::scene);
		PROPERTY_WF(Scene, scene, ix::scene);
		
		
		OBJECT(Widget, WViewport);
	};
};

#endif
