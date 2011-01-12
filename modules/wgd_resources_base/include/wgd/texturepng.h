#ifndef _WGD_TEXTUREPNG_H_
#define _WGD_TEXTUREPNG_H_

#include <wgd/texture.h>

namespace wgd {
	class TexturePNG : public TextureLoader {
		public:

		LOADER(TexturePNG);

		TexturePNG(cadence::File *f) : TextureLoader(f) {}
		~TexturePNG() {};

		bool load();

		static bool validate(cadence::File *f);
	};
};

#endif
