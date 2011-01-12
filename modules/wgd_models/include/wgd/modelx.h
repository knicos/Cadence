#ifndef _WGD_MODELX_
#define _WGD_MODELX_

#include <wgd/model.h>
#include <wgd/common.h>
#include <cadence/token.h>

namespace wgd {
	
	class vector3d;
	class vector2d;
	class Bone;
	struct MeshSkin;
	class AnimationSet;
	
	class ModelX : public wgd::ModelLoader {
		public:
		LOADER(ModelX);

		ModelX(cadence::File* f) : ModelLoader(f) {};	
		
		static bool validate(cadence::File*);
		
		void load();
		
		private:
		
		
		void skipBlock();
		OID getBlock();
		
		OID startBlock();
		void endBlock();
		
		
		wgd::vector3d readVector(int n=3);
		
		OID readInline();
		void readFrame(wgd::Bone *parent);
		void readMatrix(float* m); 
		void readMesh(const OID&); 
		int readVertices(wgd::vector3d *&);
		int readFaces(int*&, int*&);
		int readNormals(wgd::vector3d *&);
		int readTexCoords(wgd::vector2d *&);
		int readMaterials(int count, int *&, OID *&, int *&);
		OID readMaterial();
		int readSkinHeader(wgd::MeshSkin *&);
		int readSkin(wgd::MeshSkin *&, int weights); 
		
		void processMesh(const OID&, int, int*, wgd::vector3d*, wgd::vector3d*, wgd::vector2d*, int*, OID*, int, wgd::MeshSkin*);
		
		void readAnimationSet();
		void readAnimation(AnimationSet*);
		void readAnimationKey(AnimationSet*, int);
	
	
		bool parseJunk(){
			file()->parse(cadence::token::WhiteSpace());
			//Parse comment
			while(file()->parse(cadence::token::Char<2>("//"))){
				char c = 0;
				//cadence::Warning(0, "Parsing Comment");
				while(c!='\n' && !file()->eof()) file()->read(c);
				file()->parse(cadence::token::WhiteSpace());
			}
			return true;
		}
		
		template <typename T>
		bool parse(T t){ return t.parse(*file()); };
		
		template <typename T>
		bool parseValue(T &t){ return t.parse(*file()); };
		
		template <typename T>
		wgd::vector3d parseVector(int n, T t){
			cadence::token::Float f[4];
			for(int i=0; i<n; i++){
				parseJunk();
				parseValue(f[i]); 
				parse(t);
			}
			return vector3d(f[0].value, f[1].value, f[2].value, f[3].value);
		}

	};
};

#endif
