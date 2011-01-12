#ifndef _WGD_SPRITEFONT_
#define _WGD_SPRITEFONT_

#include <wgd/font.h>
#include <wgd/sprite.h>

namespace wgd {
	
	/**
	 *	Sprite fonts allow you to use a drawn font, rather than a system font.
	 *	They are basically a sprite, seperated into its frames, and a
	 *	list of characters that each frame represents.
	 */
	class RESIMPORT SpriteFont : public Font {
		public:
		OBJECT(Font, SpriteFont);
		SpriteFont();
		SpriteFont(const OID &id);
		SpriteFont(Sprite *spr);
		~SpriteFont();
		
		
		PROPERTY_RF(Sprite, sprite, ix::sprite); ///< Get the sprite that this font uses
		PROPERTY_WF(Sprite, sprite, ix::sprite); ///< Set the sprite to use for this font
		
		PROPERTY_RF(cadence::dstring, characters, "characters"); ///< The string of characters corresponding to the frames of the sprite
		PROPERTY_WF(cadence::dstring, characters, "characters"); ///< The string of characters corresponding to the frames of the sprite
		
		BEGIN_EVENTS(Font);
		EVENT(evt_build, (*this)(ix::sprite)(cadence::doste::modifiers::Seq)(*this)("characters"));
		END_EVENTS;
		
		private:
		void buildFont();
		
	};
};

#endif
