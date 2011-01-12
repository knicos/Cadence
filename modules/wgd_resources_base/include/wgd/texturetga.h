#ifndef _WGD_TEXTURETGA_H_
#define _WGD_TEXTURETGA_H_

#include <wgd/texture.h>

namespace wgd {
	class TextureTGA : public TextureLoader {
		public:

		LOADER(TextureTGA);

		TextureTGA(cadence::File *f) : TextureLoader(f) {}
		~TextureTGA() {};

		bool load();

		static bool validate(cadence::File *f);
	};
};

#endif
