#ifndef _WGD_WIDGETS_
#define _WGD_WIDGETS_

#include <wgd/vector.h>
#include <wgd/sprite.h>
#include <wgd/vertex.h>
#include <wgd/font.h>
#include <cadence/doste/doste.h>
#include <cadence/agent.h>
#include <cadence/notation.h>
#include <cadence/doste/modifiers.h>
#include <wgd/dll.h>
#include <vector>
#include <list>

namespace wgd {

	class WidgetManager;
	
	typedef cadence::doste::OID WidgetID;
	typedef cadence::doste::OID WidgetName;
	
	/**	Clip rectangle structure for widget clipping */
	struct ClipRect { int top; int left; int right; int bottom; };
	
	/**
	 * Represenst GUI Widgets. <br>
	 * Widgets are independant of the game scene system and the multiple cameras
	 * They are always defined in window pixel coordinates from the top left corner.<br>
	 * All widgets MUST have a unique name if they are to be drawn.<br>
	 * You can create widgets as children of other widgets, so they inherit the position
	 * and visible status of their parent.<br>
	 * If you do not specify width and height of a widget, that widget will be drawn from the
	 * center of the widget rather than from the corner, and will have a size specified by its texture.
	 * 
	 */
	class WIDIMPORT Widget : public cadence::Agent {
		friend class WidgetManager;

		public:

		OBJECT(Agent, Widget);

		Widget(const cadence::doste::OID &id);
		virtual ~Widget();
		
		/**
		 * Construct a widget with these parameters.
		 * @param x X-Coordinate.
		 * @param y Y-Coordinate.
		 * @param w Widget width.
		 * @param h Widget height.
		 */
		void make(int x=0, int y=0, int w=16, int h=16);
		
		/** Get the position of the top left corner of the widget */
		vector2d position() {
			return vector2d(property(ix::x), property(ix::y));
		};
		
		/** Set the position of the top left corner of the widget */
		void position(const vector2d &v) {
			property(ix::x, v.x);
			property(ix::y, v.y);
		};
		
		/** Set the width of the widget */
		PROPERTY_WF(int, width, ix::width);
		PROPERTY_RF(int, width, ix::width);
		PROPERTY_WF(int, height, ix::height);
		PROPERTY_RF(int, height, ix::height);
		
		/** The sprite that the widget uses */
		PROPERTY_WF(wgd::Sprite, sprite, ix::sprite);
		PROPERTY_RF(wgd::Sprite, sprite, ix::sprite);
		
		/** The border of the sprite in pixels that is not stretched when drawing */
		PROPERTY_W(int, border);
		PROPERTY_R(int, border);
		
		/** Whether the widget responds to the mouse events - needs a better name */
		PROPERTY_W(bool, solid);
		PROPERTY_R(bool, solid);		

		/**Z position - for sorting, a heigher z position means the widget is in front */
		PROPERTY_WF(int, z, ix::z);
		PROPERTY_RF(int, z, ix::z);
		
		/** Whethet this widget and its children exists on screen */
		PROPERTY_WF(bool, visible, ix::visible);
		PROPERTY_RF(bool, visible, ix::visible);
		
		/** The current frame of the widget - some widgets will override this */
		PROPERTY_WF(int, frame, ix::frame);
		PROPERTY_RF(int, frame, ix::frame);

		/** Tint colour of the sprite */
		PROPERTY_RF(wgd::colour, tint, ix::tint);
		PROPERTY_WF(wgd::colour, tint, ix::tint);

		/** Set the colout of the text on the button */
		PROPERTY_WF(wgd::colour, textColour, "textcolour");
		PROPERTY_RF(wgd::colour, textColour, "textcolour");
		
		PROPERTY_WF(int, textX, "textx");
		PROPERTY_RF(int, textX, "textx");

		PROPERTY_WF(int, textY, "texty");
		PROPERTY_RF(int, textY, "texty");

		PROPERTY_WF(int, textWidth, "textwidth");
		PROPERTY_RF(int, textWidth, "textwidth");
		PROPERTY_WF(int, textHeight, "textheight");
		PROPERTY_RF(int, textHeight, "textheight");
		
		/** Set the buttons caption */
		PROPERTY_WF(cadence::dstring, caption, ix::caption);
		PROPERTY_RF(cadence::dstring, caption, ix::caption);
		
		/** Set the caption font */
		PROPERTY_WF(wgd::Font, font, ix::font);
		PROPERTY_RF(wgd::Font, font, ix::font);

		//Fundamental events.
		/*DB_GET_BOOL(clicked, ix::clicked);
		DB_GET_BOOL(mouseOver, ix::mouseover);
		DB_GET_BOOL(mouseExit, ix::mouseexit);
		DB_GET_OBJECT(mouseDown, ix::mousedown);
		DB_GET_OBJECT(mouseUp, ix::mouseup);
		DB_GET_OBJECT(keyDown, ix::keydown);
		DB_GET_OBJECT(keyUp, ix::keyup);
		DB_GET_BOOL(enter, ix::enter);
		DB_GET_BOOL(keyPressed, ix::keypressed);*/

		/**
		 * Set a custom variable of this widget in the database
		 * @param var variable name
		 * @param value Variable value
		 */
		void property(const cadence::doste::OID &var, const cadence::doste::OID &value) { set(var, value); };
		/**
		 * Get the value of a custom variable of this widget
		 * @param var Variable name
		 * @return value variable value - returns Null if not set
		 */
		 cadence::doste::OID property(const cadence::doste::OID &var){ return get(var); }; 
		
		/*DOSTE_BEGINEVENTS;
		DOSTE_EVENT(1000, changeVisible);	//Visible
		DOSTE_EVENT(1001, changed);	//width
		DOSTE_EVENT(1002, changed);	//height
		DOSTE_EVENT(1003, changed);	//frame
		DOSTE_EVENT(1004, changed);	//sprite
		DOSTE_ENDEVENTS;*/

		BEGIN_EVENTS(Agent);
		EVENT(evt_frame, (*this)(ix::frame));
		EVENT(evt_changed, (*this)(ix::width)(cadence::doste::modifiers::Seq)(*this)(ix::height)(cadence::doste::modifiers::Seq)(*this)(ix::sprite));
		EVENT(evt_text, (*this)(ix::caption)(cadence::doste::modifiers::Seq)(*this)(ix::font));
		EVENT(evt_child, (*this)(ix::children)(cadence::doste::All)); //This also needs to trigger when a widget gets registered
		//EVENT(evt_child, _DASM({ .children $; @root modules }));
		END_EVENTS;

		//class ZCompare {
		//	public:
		//	bool operator()(const cadence::OID& a, const cadence::OID& b) const {return (int)a[ix::z] > (int)b[ix::z]; };
		//};
		
		protected:
		//these are called by drawall and updateall
		virtual void draw(const ClipRect &);
		virtual void update();
		bool mouse(int x, int y);
		//clip a widget - calls glScissor(...)
		ClipRect clip(const ClipRect &, const ClipRect&);
		
		//there are some specific drawing functions:
		void drawChildren(const ClipRect &);
		void drawText(int ox, int oy, const char* s, wgd::Font* fnt = NULL); //draws some text at an offset
		void clearFrame(); //start again
		void addFrame(int x, int y, int w, int h, int cframe=0, int cborder=0); //adds a sprite frame to the vertex array
		void drawSprite(); //draws the vertex array
		void resizeText();
		void makefocus(Widget *w);
		void setfocus();
		void addFocus(Widget *w); //adds object to front focus list with z order restraints
		
		vector2d screenPosition() { return vector2d(get(ix::screenx), get(ix::screeny)); }
		
		//Throw an event - calls event handler
		void event(const cadence::doste::OID& type);
		
		//These functions are only called of this widget has focus
		virtual void keyDown(const cadence::doste::OID& key, bool shift){};
		virtual void keyUp(const cadence::doste::OID& key, bool shift){};
		
		//Vertex array
		wgd::vertex* m_vertices;
		unsigned char* m_indices;
		int m_nVertices;
		int m_nIndices;
		bool m_remake;
		
		//dimensions - depreciated
		float m_tw, m_th, m_tx, m_ty;
		
		std::list<Widget*> m_focus;

		static Widget *s_prevmouse;
	};

	class WidgetManager : public cadence::Agent {
		public:

		OBJECT(Agent, WidgetManager);

		WidgetManager(const cadence::doste::OID &o);
		~WidgetManager();
		
		void draw();
		
		/** position offset for all widgets in this widget manager */
		vector2d offset() { return vector2d((float)(int)get("ox"), (float)(int)get("oy")); };
	
		BEGIN_EVENTS(WidgetManager);
		EVENT(evt_mouse, (*this)(ix::x)(cadence::doste::modifiers::Seq)((*this))(ix::y));
		EVENT(evt_focus, (*this)(ix::focus));
		END_EVENTS;
		
		/** Widget that currently has focus */
		static cadence::doste::OID s_focus;
		private:

		static bool s_rebuild;

	};
};


#endif
