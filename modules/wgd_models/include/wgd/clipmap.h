#ifndef _WGD_CLIPMAP_
#define _WGD_CLIPMAP_

#include <cadence/agent.h>
#include <wgd/common.h>
#include <wgd/model.h>
#include <vector>

namespace wgd {
	/**
	 *	The Clipmap object is an interface to do per-polygon collision detection on a model
	 *	generally used for levels or static structures. The polygon data is stored in an octree
	 *	structure to reduce the number of tests needed. (this will be optional for speed purposes)<br>
	 *	The object acts like a static IModel instance that does not get drawn, so you can have
	 *	a simple clipmap over more complex geometry.<br>
	 *	Currently supports line segment intersection (rays) and sphere collision detection.<br>
	 *	You have the option of caching transformed vertices in an octree to reduce the number
	 *	of polygon tests and transformations for each collision test.
	 */
	class MODELSIMPORT ClipMap : public cadence::Agent {
		public:
		OBJECT(cadence::Agent, ClipMap);
		ClipMap(const wgd::OID&);
		/**	Create a new clipmap object from a model 
		 *	@param model The model object to use as a data source
		 *	@param depth The depth of the octree to use if cache is true
		 *	@param cache Cache transformed vertices in an octree
		 */
		ClipMap(wgd::Model* model, int depth=0, bool cache = true);
		~ClipMap();
		
		/** The model to use for collision */
		PROPERTY(wgd::Model, model);
		/** depth of octree - defaults to 0 = no octree optimisations. Only applies if cache is true */
		PROPERTY(int, depth);
		
		/** 
		 *	Cache transformed vertices in an octree
		 *	massive speedup for large static objects, but not
		 *	good for objects that move.
		 *	When cache is false, the transformations occur for each test.
		 */
		PROPERTY(bool, cache);
		
		/** clipmap position */
		PROPERTY_F(wgd::vector3d, position, wgd::ix::position);
		/** orientation */
		PROPERTY_F(wgd::vector3d, orientation, wgd::ix::orientation);
		/** scale */
		PROPERTY_F(wgd::vector3d, scale, wgd::ix::scale);
		
		/**
		 *	Test a line segment to see if it intersects any polygons in the clip map.
		 *	This gets the closest collision to the start of the segment if it hits.
		 *	@param start The start of the line segment
		 *	@param end The end of the line segment
		 *	@param point The point of collision - return value
		 *	@param normal The normal of the polygon collided with (not normlised)
		 *	@return the number of colisions on the line segment.
		 */
		int test(const wgd::vector3d &start, const wgd::vector3d &end, wgd::vector3d &point, wgd::vector3d &normal);
		
		/** Test a capsule with the clipmap - not implemented */
		int test(const wgd::vector3d &a, const wgd::vector3d &b, float radius, wgd::vector3d *point, wgd::vector3d *normal, int max);
			
		/**
		 *	Test a sphere for collision with the clipmap. 
		 *	This returns all collisions with the sphere and the clipmap geometry
		 *	sorted by the penetration distance (greatest first)
		 *	@param centre The centre of the sphere
		 *	@param radius The sphere radius
		 *	@param point Array of collision points
		 *	@param normal Array of collision normals
		 *	@param max The maximum number of collisions to return
		 *	@return The number of collisions
		 */
		int test(const wgd::vector3d &centre, float radius, wgd::vector3d *point, wgd::vector3d *normal, int max);
		
		//events
		BEGIN_EVENTS(cadence::Agent);
		EVENT(evt_build, (*this)(wgd::ix::model)("loaded"));
		EVENT_VECTOR(evt_scale, (*this)(wgd::ix::scale));
		EVENT_VECTOR(evt_position, (*this)(wgd::ix::position));
		EVENT_VECTOR(evt_orientation, (*this)(wgd::ix::orientation));
		EVENT_VECTOR(evt_cache, (*this)("cache"));
		END_EVENTS;
		
		///DEBUG: get number of polygons tested in the last call to test()
		int tests() { return  m_tests; }
		
		private:
		
		//collision data struct
		struct CData {
			CData(const wgd::vector3d &pos,const wgd::vector3d &nrm, float dst) {
				memcpy(p, &pos.x, 3 * sizeof(float));
				memcpy(n, &nrm.x, 3 * sizeof(float));
				d = dst;
			}
			bool operator<(const CData &a) const { return a.d < d; }
			float p[3];
			float n[3];
			float d;
		};
		
		//number of polygons tested in the last test
		int m_tests;
		
		/** build clipmap from model - gets AABB and creates octree */
		void build();
		bool m_built;
			
		//vertex data for octree
		struct NodeData { int mesh, polygon; };
		
		//pointer based octree
		struct Node {
			Node* n[8];	//child nodes
			wgd::vector3d centre;	//node centre
			wgd::vector3d size;	//node size
			int depth;	//node depth
			
			//the data - polygon indices
			std::vector<NodeData> data;
		} m_tree;
		int m_depth; //maximum depth of octree

		//add a polygon to the octree
		void clearTree(Node &n);
		void addTree(Node &n, NodeData &data);
		Node &addNode(Node &n, int index); //add node to tree
		
		//local matrix for transformation
		void calcLocal();
		wgd::matrix m_localMatrix;
		wgd::matrix m_inverseMatrix;
		
		//transformed vertices
		wgd::vector3d *m_vertex;
		int m_vcount; //cached vertex count
		wgd::Mesh **m_mesh; //caches meshes (when not caching vertices)
		int m_meshes;

		//AABB
		wgd::vector3d m_high, m_low;
		
		//octree ray test - this one seems very slow so is not currently used
		int testTree(Node &n, const wgd::vector3d &a, const wgd::vector3d &b, std::vector<CData> &data, float length);
		
		//octree sphere test
		int testTree(Node &n, const wgd::vector3d &centre, float radius, std::vector<CData> &data);
	};
};

#endif
