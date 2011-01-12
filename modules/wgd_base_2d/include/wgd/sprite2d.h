#ifndef _WGD_SPRITE2D_
#define _WGD_SPRITE2D_

#include <wgd/common.h>
#include <wgd/sprite.h>
#include <wgd/vertex.h>
#include <wgd/colour.h>
#include <wgd/drawable.h>
#include <wgd/instance2d.h>

namespace wgd {
	class BASE2DIMPORT Sprite2D {
		public:
		/// Create a new sprite
		Sprite2D();
		/** create a sprite
		 * @param sprite The sprite resource to use
		 * @param position The position of the sprite in the world
		 * @param scale The sprite scale compared to the source texture size
		 * @param angle The angle in radians that the sprite is drawn at
		 */
		Sprite2D(Sprite* sprite, const vector2d &position, float scale=1.0f, float angle=0.0f, int frame=0);
		
		//Set the sprite resource
		void sprite(Sprite *spr);
		/// Set the position of the center of the sprite.
		void position(const vector2d &p);
		/// set the angle of the sprite
		void angle(float ang);
		///set the sprite scale compared to the source texture size 
		void scale(float s);
		///specify different scales fro the x and y axes
		void stretch(float sx, float sy);
		/// set the absolute size of the sprite in pixels
		void size(float width, float height);
		//set the sprite colour
		void colour(const wgd::colour &c);
		//set the sprite frame
		void frame(int f);
		//the depth
		void depth(float d);
		
		
		void draw(SceneGraph &graph);
		Drawable<SpriteVertex> *drawable();
		
		private:
		Sprite *m_sprite;
		Drawable<SpriteVertex> *m_drawable;
		vector2d m_position;
		float m_angle;
		vector2d m_size;
		int m_frame;
		float m_depth;
		
		bool m_changed;
		
		void setup();
		void build();
		
	};
	
	class BASE2DIMPORT ISprite2D : public Instance2D {
		public:
		OBJECT(Instance2D, ISprite2D);
		ISprite2D(const OID &id);
		ISprite2D();
		~ISprite2D();
		
		virtual void draw(SceneGraph &graph, Camera2D *camera);
		
		PROPERTY_RF(float, width, ix::width); ///< the width of the sprite on screen
		PROPERTY_WF(float, width, ix::width); ///< set width of the sprite
		PROPERTY_RF(float, height, ix::height); ///< the height of the sprite on screen
		PROPERTY_WF(float, height, ix::height); ///< set height of the sprite
		
		PROPERTY_RF(Sprite, sprite, ix::sprite); ///< The sprite resource
		PROPERTY_WF(Sprite, sprite, ix::sprite); ///< set the sprite to draw
		
		PROPERTY_RF(int, frame, ix::frame); ///< The current frame of the sprite
		PROPERTY_WF(int, frame, ix::frame); ///< set the sprite frame
		
		PROPERTY_RF(float, depth, ix::depth); ///< The depth of the sprite in the scene. Greater values are further away
		PROPERTY_WF(float, depth, ix::depth); ///< Set the depth of the sprite in the scene
		
		/// set the sprite scale - this sets the width and height property based on the original sprite
		void scale(float s);
		/// specify different scales fro the x and y axes
		void stretch(float sx, float sy);
		
		BEGIN_EVENTS(Instance2D);
		EVENT(evt_size, (*this)(ix::width)(cadence::doste::modifiers::Seq)(*this)(ix::height));
		EVENT(evt_sprite, (*this)(ix::sprite));
		EVENT(evt_frame, (*this)(ix::frame));
		END_EVENTS;
		
		private:
		Sprite2D m_sprite;
	};
};

#endif
