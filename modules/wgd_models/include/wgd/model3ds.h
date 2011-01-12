#ifndef _WGD_MODEL3DS_
#define _WGD_MODEL3DS_

#include <wgd/model.h>

namespace wgd {
	
	class vector3d;
	class vector2d;
	
	class Model3DS : public wgd::ModelLoader {
		public:
		LOADER(Model3DS);

		Model3DS(cadence::File *f) : ModelLoader(f) {};	
		
		void load();
		
		static bool validate(cadence::File*);
		
		private:
		
		void processMesh(int index, char *material, int nFaces, unsigned short *faces, wgd::vector3d *vertices, wgd::vector2d *tex);
		
	};
};

#endif
