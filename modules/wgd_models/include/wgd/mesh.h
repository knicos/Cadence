#ifndef _WGD_MESH_
#define _WGD_MESH_

#include <wgd/dll.h>
#include <wgd/vertex.h>
#include <wgd/bone.h>
#include <wgd/drawable.h>
#include <cadence/object.h>

namespace wgd {
	
	class vector3d;
	
	typedef vertex_t meshVertex;
	
	struct MeshSkin {
		float weight;
		wgd::Bone *bone;
	};
	
	class MODELSIMPORT Mesh : public cadence::Object {
		friend class SkinController;
		friend class MorphController;
		friend class IModel;
		public:
		OBJECT(Object, Mesh);

		Mesh();
		Mesh(const cadence::doste::OID&);
		~Mesh();
		
		/** The material name for this mesh */
		PROPERTY_RF(cadence::doste::OID, material, ix::material);
		PROPERTY_WF(cadence::doste::OID, material, ix::material);
		
		/** The bone that this mesh is attached to - skeletal animation overrides this */
		PROPERTY_RF(cadence::doste::OID, bone, "bone");
		PROPERTY_WF(cadence::doste::OID, bone, "bone");
		
		//set data
		void vertices(int, meshVertex* vx);
		void skin(int, MeshSkin*);
		
		//get data
		int vertices()		{ if(m_drawable) return m_drawable->size; else return 0; };
		meshVertex *data()	{ if(m_drawable) return m_drawable->data; else return 0; };
		MeshSkin *skin()	{ return m_skin; };
		int weights()		{ return m_weightsPerVertex; }; 
		
		//for transformed meshes - setup space
		void create(int);
		
		/** get drawable object */
		Drawable<meshVertex>* drawable() { return m_drawable; }
		
		/** calculate normals from face data */
		void calcNormals();
		/** calculate tangents from normals and texture coordinates */
		void calcTangents();
		
		private:
		
		wgd::vector3d calcTangent(const meshVertex &vA, const meshVertex &vB, const meshVertex &vC);
		
		//Vertex Weights		
		int m_weightsPerVertex;
		MeshSkin *m_skin;
		
		//drawable - contains mesh data
		void setup();
		Drawable<meshVertex> *m_drawable;
		
	};
};

#endif
