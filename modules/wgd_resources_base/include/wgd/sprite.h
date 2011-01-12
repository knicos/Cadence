/*
 * Warwick Game Design Library
 *
 * Authors: 
 * Date: 18/9/2007
 *
 */

#ifndef _WGD_SPRITE_
#define _WGD_SPRITE_

#include <wgd/texture.h>
#include <wgd/index.h>

namespace wgd {

	/**
	 * A sprite resource. Here you describe a sprite, such as its textures, animation columns
	 * and rows, number of frames and whether it is a billboard or not. Use an ISprite to
	 * actually draw an instance of this sprite in the game.
	 */
	class RESIMPORT Sprite : public cadence::Agent {

		public:

		OBJECT(Agent, Sprite);
		
		Sprite();
		Sprite(const char* filename);
		Sprite(cadence::File *texture);
		Sprite(const cadence::doste::OID &res);
		~Sprite();

		/**
		 * Set the texture for this sprite.
		 * Multiple frames are specified within a single texture by using tiles.
		 */
		PROPERTY_WF(wgd::Texture, texture, ix::texture);
		/**
		 * Get the sprites texture.
		 * @return Pointer to texture object or NULL if no object has been specified.
		 */
		PROPERTY_RF(wgd::Texture, texture, ix::texture);

		/** Width of sprite texture in pixels. */
		PROPERTY_RF(int, width, ix::width);
		/**
		 * Width of sprite texture in pixels.
		 * This allows you to have non-power-of-two sprites within a power-of-two texture.
		 */
		PROPERTY_WF(int, width, ix::width);
		/** Height of sprite texture in pixels*/
		PROPERTY_RF(int, height, ix::height);
		/** Height of sprite texture in pixels*/
		PROPERTY_WF(int, height, ix::height);

		/**
		 * Frame columns. If multiple frames are in one texture then specify
		 * the number of frame columns with this. Defaults to 1
		 */
		PROPERTY_WF(int, columns,ix::columns);
		/** Get frame columns. */
		PROPERTY_RF(int, columns,ix::columns);

		/**
		 * Frame rows. The number of animation frames in the Y direction.
		 * Defaults to 1.
		 */
		PROPERTY_WF(int, rows,ix::rows);
		/**
		 * Get number of frame rows
		 */
		PROPERTY_RF(int, rows,ix::rows);

		/** Flip the sprite vertically */
		PROPERTY_WF(bool, flipv, ix::flipv);
		/** Is this sprite flipped vertically. */
		PROPERTY_RF(bool, flipv, ix::flipv);
		/** Flip the sprite horizontally */
		PROPERTY_WF(bool, fliph, ix::fliph);
		/** Is this sprite flipped horizontally */
		PROPERTY_RF(bool, fliph, ix::fliph);

		/**
		 * Specify the total number of animation frames. Frames are taken from
		 * left to right and then row by row with the top left being frame one
		 * and bottom right being the last frame.
		 */
		PROPERTY_WF(int, frames,ix::frames);
		/** Get number of animation frames. */
		PROPERTY_RF(int, frames,ix::frames);

		/** specify the default frame rate for this sprite */
		PROPERTY_WF(float, fps, ix::fps);
		/** get the default framerate for this sprite */
		PROPERTY_RF(float, fps, ix::fps);

		static void initialise();
		static void finalise();
		
		private:
		void setup();
	};
};

#endif
