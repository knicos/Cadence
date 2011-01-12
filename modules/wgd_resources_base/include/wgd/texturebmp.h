#ifndef _WGD_TEXTUREBMP_H_
#define _WGD_TEXTUREBMP_H_

#include <wgd/texture.h>

namespace wgd {
	class TextureBMP : public TextureLoader {
		public:

		LOADER(TextureBMP);

		TextureBMP(cadence::File *f) : TextureLoader(f) {}
		~TextureBMP() {};

		bool load();

		static bool validate(cadence::File *f);
	};
};

#endif
