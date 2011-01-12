#ifndef _WGD_SPRITE_3D_
#define _WGD_SPRITE_3D_

#include <wgd/sprite.h>
#include <wgd/scenegraph.h>
#include <wgd/camera3d.h>
#include <wgd/vertex.h>
#include <wgd/vector.h>
#include <wgd/quaternion.h>

namespace wgd {

	/**
	 *	Sprite3D is a c++ only sprite class to draw sprites in the 3D world.
	 *	There are three types of sprites you can use: <br>
	 *	absolute - where you specify the absolute orientation. <br>
	 *	orientated - where you specify the centre point, and it always faces the camera. <br>
	 *	aligned - where you specify a direction the sprite is aligned to, and it rotates around that to face the camera.
	 */
	class BASE3DIMPORT Sprite3D {
		public:
		Sprite3D();
		
		/** Create a new sprite3d with absolute positioning.
		 *	@param sprite The sprite resource to use
		 *	@param position The center of the sprite
		 *	@param orientation Quaternion describing the orientation of the sprite
		 *	@param width The width of the sprite in world units (default 1.0)
		 *	@param height The height of the sprite in world units (default 1.0)
		 *	@param frame The frame to initialise the sprite to (default 0)
		 */
		Sprite3D(Sprite* sprite, const vector3d &position, const quaternion &orientation, float width=1.0f, float height=1.0f, int frame=0);
		
		/** Create a new orientated sprite3d.
		 *	@param sprite The sprite resource to use
		 *	@param position The center of the sprite
		 *	@param width The width of the sprite in world units (default 1.0)
		 *	@param height The height of the sprite in world units (default 1.0)
		 *	@param frame The frame to initialise the sprite to (default 0)
		 *	@param angle The angle of sprite in relation to the camera (default 0)
		 */
		Sprite3D(Sprite* sprite, const vector3d &position, float width=1.0f, float height=1.0f, int frame=0, float angle=0);
		
		/** Create a new aligned sprite3d.
		 *	@param sprite The sprite resource to use
		 *	@param position The base position of the sprite
		 *	@param direction The direction to align the sprite to
		 *	@param width The width of the sprite in world units (default 1.0)
		 *	@param height The height of the sprite in world units (default 1.0)
		 *	@param frame The frame to initialise the sprite to (default 0)
		 */
		Sprite3D(Sprite* sprite, const vector3d &position, const vector3d &direction, float width=1.0f, float height=1.0f, int frame=0);
		~Sprite3D();
		
		/** Add the sprite to the scene graph */
		DrawableBase *draw(SceneGraph &graph, Camera3D *camera);
		
		/** Get the sprite drawable. Generates the sprite */
		Drawable<SpriteVertex> *drawable(Camera3D* cam);
		
		/**	Set the sprite resource */
		void sprite(Sprite* spr);
		/** Set the width and height of the sprite */
		void size(float width, float height);
		/** set the position of the center of the sprite, or the bottom of the sprite if direction is used */
		void position(const vector3d &v);
		/** Set the direction of the sprite if it is aligned, makes the sprite aligned */
		void direction(const vector3d &v);
		/** Set the absoulute orientation of the sprite  */
		void orientation(const quaternion &v);
		/** Set the angle of the sprite, makes the sprite orientated */
		void angle(float ang);
		/** set the curent frame */
		void frame(int f) {if(m_frame!=f) setFrame(f); };
		
		/** be able to tint sprites */
		void colour(const wgd::colour& c);
		
		
		private:
		//create geometry
		void setup();			//Initialise drawable
		void setFrame(int f);	//set texture coords for frame
		void makeOrientated(Camera3D* cam);
		void makeAligned(Camera3D *cam);
		void makeAbsolute();
		void drawSprite();
		
		Drawable<SpriteVertex> *m_drawable;
		
		float m_width;
		float m_height;
		
		Sprite *m_sprite;
		int m_frame;
		
		vector3d m_position;
		vector3d m_direction;
		quaternion m_orientation;
		float m_angle;
		
		int m_mode; // orientated, aligned, absolute 
	};
};

#endif
