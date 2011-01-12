#ifndef _WGD_SCENE3D_
#define _WGD_SCENE3D_

#include <wgd/scene.h>
#include <wgd/scenegraph.h>
#include <wgd/instance3d.h>
#include <cadence/doste/doste.h>
#include <cadence/doste/definition.h>

#include <map>
#include <list>
#include <vector>

namespace wgd {
	class Camera;
	
	class BASE3DIMPORT Scene3D : public wgd::Scene {
		public:
		Scene3D(const cadence::doste::OID &);
		~Scene3D();
		
		void update();
		void draw(Camera *);
		
		//Octree dimensions
		PROPERTY_RF(float, width,  ix::width);
		PROPERTY_RF(float, height, ix::height);
		PROPERTY_RF(float, depth,  ix::depth);
		PROPERTY_WF(float, levels, ix::levels);
		PROPERTY_RF(float, levels, ix::levels);
		
		//events
		BEGIN_EVENTS(Scene);
		EVENT(evt_add, (*this)(ix::instances)(cadence::doste::All));
		END_EVENTS;
		
		OBJECT(Scene, Scene3D);
		
		/** get the scene graph object to manually add drawables */
		SceneGraph &graph() { return m_graph; };
				
		private:
		
		//octree node structure
		struct Node{
			wgd::vector3d centre; //center point of node
			uint key; //Morton key for this node
			unsigned char hasChild; //Bitmask or each child (optional)
			std::list<Instance3D*> instances; //list of instances in this node
		};
		//linear octree - 'Real Time Collision Detection' [ch7.3.1 p314]
		std::map<uint, Node*> m_octree;
		
		//Map of instance pointers to location codes
		std::map<wgd::Instance3D*, uint> m_location;
		
		//lights
		std::vector<Instance3D*> m_lights;
		//Skyboxes
		//std::vector<Instance3D*> m_skyboxes;
		
		//Morton key calculations
		uint morton(const vector3d &);
		uint truncate(int depth, uint key);
		int calcDepth(uint key);
		
		//octree stuff - per instance
		void add(Instance3D* inst);
		uint insert(Node*, Instance3D*, const wgd::vector3d &low, const wgd::vector3d &high, int d=0);
		uint remove(Instance3D *inst); 
		
		//global octree functions
		void build();
		void build(const cadence::doste::OID&);
		void clear();
		
		//has the octree been built
		bool m_built;
		bool m_doclear;
		
		//scene graphs
		SceneGraph m_graph;
		SceneGraph m_skyboxes;
		
		
	};
	
};

#endif
