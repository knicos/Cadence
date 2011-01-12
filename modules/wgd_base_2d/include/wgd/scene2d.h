#ifndef _WGD_SCENE2D_
#define _WGD_SCENE2D_

#include <wgd/scene.h>
#include <wgd/camera.h>
#include <wgd/scenegraph.h>

namespace wgd {
	class BASE2DIMPORT Scene2D : public Scene {
		public:
		OBJECT(Scene, Scene2D);
		Scene2D(const OID&);
		~Scene2D();
		
		void update();
		void draw(Camera *);
		
		SceneGraph &graph() { return m_graph; };
		
		//quadtree stuff later
		private:
		SceneGraph m_graph;
	};
};

#endif
