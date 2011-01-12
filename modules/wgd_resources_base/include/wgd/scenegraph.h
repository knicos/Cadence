#ifndef _WGD_SCENEGRAPH_
#define _WGD_SCENEGRAPH_

#include <wgd/drawable.h>

namespace wgd {
	class RESIMPORT SceneGraph {
		public:
		
		SceneGraph() {};
		~SceneGraph() {};
		
		/** Draw the scene graph */
		bool draw();		
		
		/** Add a drawable to scene graph - works out which list to add to */
		DrawableBase* add(DrawableBase *d);
		
		/** Add solid drawable to scene graph */
		DrawableBase* addSolid(DrawableBase *d);
		/** Add transparent drawable to scene graph - does alpha sorting on there */
		DrawableBase* addAlpha(DrawableBase *d);
		
		private:

		//Drawable drawing queues
		std::vector<DrawableBase*> m_solid;	//List of solid items, sorted by material
		std::vector<DrawableBase*> m_alpha;	//list of non-solid items - sorted by distance

	};
};

#endif
