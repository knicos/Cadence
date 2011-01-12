#ifndef _WGD_MODEL_
#define _WGD_MODEL_

#include <wgd/dll.h>
#include <cadence/agent.h>
#include <cadence/dstring.h>
#include <wgd/vector.h>
#include <wgd/index.h>
#include <wgd/bone.h>
#include <wgd/mesh.h>
#include <wgd/animationset.h>
#include <cadence/file.h>
#include <wgd/loader.h>
#include <vector>

namespace wgd{
	class Material;
	class ModelLoader;
	
	class MODELSIMPORT Model : public cadence::Agent {
		friend class ModelLoader;
		
		public:
		Model(const char* filename);
		Model(cadence::File *file); 
		Model(const cadence::doste::OID&);
		~Model();
		
		void load();
		
		/** is the model loaded? */
		PROPERTY_RF(bool, loaded, "loaded");
		
		/** Model file name */
		PROPERTY_RF(cadence::File, file, "file");
		PROPERTY_WF(cadence::File, file, "file");
		
		PROPERTY_RF(float, fps, ix::fps); ///< the default fps for this model
		PROPERTY_WF(float, fps, ix::fps); ///< set the default fps for this model
		
		OBJECT(Agent, Model);
		
		BEGIN_EVENTS(Agent);
		EVENT(evt_file, (*this)("file"));
		END_EVENTS;		
		
		/** Get a meterial from teh model */
		Material *material(const cadence::doste::OID&) const;
		

		//Mesh functions
		int meshes();
		Mesh *mesh(int) const;
		
		//Animation Sets
		AnimationSet *animationSet(const cadence::doste::OID &name);
		
		// Bone functions //
		int bones() const { return m_bones.size(); };
		Bone *bone(const cadence::doste::OID &name);
		Bone *boneID(int bid) const;
		
		private:
		int m_nMeshes;
		
		//map of bones in this model
		cadence::Map m_boneMap;
		std::vector<wgd::Bone*> m_bones;
		
		//AnimationSets
		cadence::Map m_animationMap;
		
		//Bounding sphere
		float m_radius;
		wgd::vector3d m_centre;
		
		//Loading
		bool m_loaded;
		ModelLoader *m_loader;
		
	};
	
	class ModelLoader : public Loader {
		public:
		
		BASE_LOADER(ModelLoader);
		
		ModelLoader(cadence::File *f) : Loader(f), m_materialCounter(99), m_meshCounter(0) {};
		virtual ~ModelLoader() {}
		
		// Set the parent model //
		void parent(Model* m) { m_model = m; };
		
		// Load the model //
		virtual void load() = 0;
		
		protected:
		Model *m_model;
		
		Bone *createBone(const cadence::doste::OID &name);
		Mesh *createMesh();
		Material *createMaterial(cadence::doste::OID &name);
		AnimationSet *createAnimation(const cadence::doste::OID &name);
		
		// texture directory //
		char* directory(cadence::dstring, char*) const;
		
		private:
		
		//Used for automatically naming materials and meshes
		int m_materialCounter;
		int m_meshCounter;
		
	};
};

#endif
