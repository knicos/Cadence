//3d text - works identically to Sprite3D, just with Font rather than Sprite


#ifndef _WGD_TEXT3D_
#define _WGD_TEXT3D_

#include <wgd/font.h>
#include <wgd/sprite3d.h> //included for matrix functions
#include <wgd/colour.h>

#include <wgd/instance3d.h>

namespace wgd {
	class BASE3DIMPORT Text3D {
		public:
		Text3D();
		/** Create a new orientated sprite3d.
		 *	@param font The font to use
		 *	@param position The center of the text
		 *	@param scale The scale of the text in world units (default 1.0)
		 *	@param align Is the text alighed left, right or centered at position.
		 *	@param angle The angle of sprite in relation to the camera (default 0)
		 */
		Text3D(const char* text, Font* font, const vector3d &position, float scale=1.0f, TextAlign align=CENTRE, float angle=0);
		~Text3D();
		
		/** Add the sprite to the scene graph */
		DrawableBase *draw(SceneGraph &graph, Camera3D *camera);
		
		/** Get the sprite drawable. Generates the sprite */
		Drawable<SpriteVertex> *drawable(Camera3D* cam);
		
		/**	Set the sprite resource */
		void font(Font* font) { m_font=font; m_changed=true; }
		/** Set the font scale of the sprite */
		void scale(float s) { m_scale = s; }
		/** set alignment */
		void align(TextAlign align) { m_align=align; m_changed=true; }
		/** set the position of the center of the sprite, or the bottom of the sprite if direction is used */
		void position(const vector3d &v) { m_position=v; }
		/** Set the angle of the sprite, makes the sprite orientated */
		void angle(float ang) {m_angle = ang; }
		/** set the text */
		void text(const char* t);
		/** text colour */
		void colour(const wgd::colour& c);
		
		private:
		void setup();
		void build();
		
		bool m_changed;	//text has changed
		
		Drawable<SpriteVertex> *m_drawable; //drawable object
		vector3d m_position;
		float m_angle;
		float m_scale;
		TextAlign m_align;
		wgd::colour m_colour;
		char* m_text;
		Font *m_font;
	};
	
	
	//instance
	class IText3D : public Instance3D {
		public:
		OBJECT(Instance3D, IText3D);
		IText3D(const wgd::OID &);
		IText3D();
		
		virtual void draw(SceneGraph &graph, Camera3D *camera);
		
		PROPERTY(cadence::dstring, text);
		PROPERTY(float, scale);
		PROPERTY(float, angle);
		PROPERTY(wgd::colour, colour);
		PROPERTY(Font, font);
		void align(TextAlign align);
		TextAlign align();
		
		BEGIN_EVENTS(Instance3D);
		EVENT(evt_text,  (*this)(ix::text));
		EVENT(evt_scale, (*this)(ix::scale));
		EVENT(evt_angle, (*this)("angle"));
		EVENT(evt_align, (*this)("align"));
		EVENT_COLOUR(evt_colour, (*this)(ix::colour));
		EVENT_VECTOR(evt_position, (*this)(ix::position));
		EVENT(evt_font, (*this)(ix::font));
		END_EVENTS;
		
		private:
		Text3D m_text;
	};
	
	
};

#endif
