#ifndef _WGD_MORPH_CONTROLLER_
#define _WGD_MORPH_CONTROLLER_

#include <wgd/instance3d.h>
#include <wgd/model.h>

namespace wgd {
	class MorphController {
		public:
		MorphController(wgd::Instance3D* inst);
		~MorphController();
		
		bool update();
		
		void changeMorph(int mesh, float transition=0.0);
		
		/** get the number of meshes in this morph */
		int meshes(){ return m_meshes; }
		
		private:
		
		void deform(wgd::Mesh* dest, wgd::Mesh *a, wgd::Mesh* b, float pcnt);
		
		//resize local memory
		void resize(int n);
		int m_meshes;
		
		wgd::Instance3D *m_inst;
		wgd::Model *m_model;
		
		//current morphs		
		float *m_time;		//time at the start of each morph
		float *m_duration;	//duration of each morph
		
		//need temporary meshes if i do transitions - maybe do that later
		//as i havent finished transitions in animationcontroller yet
	};
};

#endif
