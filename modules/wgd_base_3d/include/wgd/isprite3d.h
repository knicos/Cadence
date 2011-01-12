#ifndef _WGD_ISPRITE3D_
#define _WGD_ISPRITE3D_

#include <wgd/common.h>
#include <wgd/instance3d.h>
#include <wgd/sprite3d.h>

#include <cadence/doste/doste.h>
#include <cadence/notation.h>

namespace wgd {
	class BASE3DIMPORT ISprite3D : public Instance3D {
		public:
		
		OBJECT(Instance3D, ISprite3D);
		
		ISprite3D();
		ISprite3D(const OID &id);
		~ISprite3D();
		
		/** Draws the sprite */
		void draw(SceneGraph &graph, Camera3D *camera);
		
		/** get the sprite that this instance uses */
		PROPERTY_RF(Sprite, sprite, ix::sprite);
		/** Set the sprite that this object instanciates */
		PROPERTY_WF(Sprite, sprite, ix::sprite);
		
		/** get the colour of the sprite */
		PROPERTY_RF(wgd::colour, colour, ix::colour);
		/** set the sprite colour */
		PROPERTY_WF(wgd::colour, colour, ix::colour);
		
		/** Get the direction the sprite points in if it is in aligned mode */
		PROPERTY_RF(vector3d, direction, ix::direction);
		/** set the direction the sprite aligns to if in aligned mode */
		PROPERTY_WF(vector3d, direction, ix::direction);	
		
		/** Get the mode of this sprite */
		PROPERTY_RF(OID, mode, ix::mode);
		/** 
		 *	Set the mode this ISprite uses, can be 'point', 'aligned' or 'absolute'. <br>
		 *	point mode is used if no mode is set. <br>
		 *	In point mode, only the roll (z) coordinate of the orientation is used.
		 */
		PROPERTY_WF(OID, mode, ix::mode);
		
		/** The width of the sprite in world units */
		PROPERTY_RF(float, width, ix::width);
		PROPERTY_WF(float, width, ix::width);
		
		/** The height of the sprite in world units */
		PROPERTY_RF(float, height, ix::height);
		PROPERTY_WF(float, height, ix::height);		
		
		/** The current frame */
		PROPERTY_RF(int, frame, ix::frame);
		/** set the current frame */
		void frame(int f);
		
		/** frame rate of the sprite */
		PROPERTY_RF(float, fps, ix::fps);
		PROPERTY_WF(float, fps, ix::fps);
		
		// events to buffer database - will do the rest later
		BEGIN_EVENTS(Instance3D);
		EVENT(evt_sprite, (*this)(ix::sprite));
		EVENT(evt_frame, (*this)(ix::frame));
		EVENT(evt_size, (*this)(ix::width)(cadence::doste::modifiers::Seq)(*this)(ix::height));
		EVENT_COLOUR(evt_colour, (*this)(ix::colour));
		END_EVENTS;
		
		private:
		Sprite3D m_sprite;
		float m_start;
	};
};

#endif
