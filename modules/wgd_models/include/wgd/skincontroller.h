#ifndef _WGD_SKIN_CONTROLLER_
#define _WGD_SKIN_CONTROLLER_

#include <wgd/common.h>
#include <wgd/animationcontroller.h>

namespace wgd {
	class SkinController {
		public:
		SkinController(wgd::AnimationController *c);
		~SkinController();
		
		bool update();
		
		private:
		//calculate final matrices the offset to nullify translation when transforming normals
		void calculateFinal();
		void deform(wgd::Mesh* dest, wgd::Mesh *src);
		
		vector3d *m_offset;
		AnimationController *m_anim;
		
		//final transformation matrices (includes skin offset)
		matrix *m_final;
		matrix *m_finalA;
	};
};

#endif
