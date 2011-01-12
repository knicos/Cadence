#ifndef _WGD_RENDERTARGET_
#define _WGD_RENDERTARGET_

#include <cadence/agent.h>
#include <wgd/index.h>
#include <wgd/camera.h>
#include <wgd/scene.h>
#include <wgd/texture.h>
#include <wgd/material.h>

namespace wgd {

	/**
	 * A render target is something that can be rendered into. This
	 * includes Framebuffers and Textures. The screen itself is a special
	 * form of render target and as a result does not inherit this
	 * class. It is possible to chain several targets together which has
	 * the effect of one render target using another as its source. This
	 * can enable fullscreen special effects such as bloom.
	 */
	class RESIMPORT RenderTarget : public cadence::Agent {
		public:
		RenderTarget(const cadence::doste::OID &id);
		~RenderTarget();
		
		void draw();
		
		/**
		 * Get the camera used by this render target. If a RenderTarget
		 * has a camera then it must also have a screen and cannot have
		 * another RenderTarget as its source. If you do give both then
		 * the result is undefined.
		 * @return The camera object or null.
		 */
		PROPERTY_RF(Camera, camera, ix::camera);
		/**
		 * Set the camera to be used by this target. Must be combined with
		 * a scene object and not a source RenderTarget.
		 * @param v A camera object used for drawing a scene.
		 */
		PROPERTY_WF(Camera, camera, ix::camera);
		
		/**
		 * Get the current source RenderTarget if there is one. 
		 * @return Source RenderTarget object or null.
		 */
		PROPERTY_RF(RenderTarget, source, ix::source);
		PROPERTY_WF(RenderTarget, source, ix::source);		
		
		PROPERTY_RF(Scene, scene, ix::scene);
		PROPERTY_WF(Scene, scene, ix::scene);

		PROPERTY_RF(Material, material, ix::material);
		PROPERTY_WF(Material, material, ix::material);

		PROPERTY_RF(Texture, texture, ix::texture);
		PROPERTY_WF(Texture, texture, ix::texture);

		PROPERTY_RF(Texture, depthTexture, "depthtexture");
		PROPERTY_WF(Texture, depthTexture, "depthtexture");

		PROPERTY_RF(bool, depth, ix::depth);
		PROPERTY_WF(bool, depth, ix::depth);

		PROPERTY_RF(bool, clear, "clear");
		PROPERTY_WF(bool, clear, "clear");
		
		OBJECT(Agent, RenderTarget);

		void doclear();
		void bind();
		void unbind();
		void begin();
		void end();

		private:
		int m_width;
		int m_height;
		bool m_depth;
		bool m_hfloat;
		bool m_linear;
		unsigned int m_fbo;
		unsigned int m_dbuf;
	};
};

#endif
