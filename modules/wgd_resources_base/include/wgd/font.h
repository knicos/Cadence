/*
 * WGD Library
 *
 * Authors: 
 * Date: 19/9/2007
 *
 */

#ifndef _WGD_FONT_
#define _WGD_FONT_

#ifdef WIN32
#include <windows.h>
#endif

#include <wgd/index.h>
#include <GL/gl.h>
#include <wgd/texture.h>

#include <wgd/dll.h>
#include <cadence/agent.h>
#include <cadence/doste/modifiers.h>
#include <cadence/doste/definition.h>

namespace wgd {
	struct colour;
	
	/** Text alignment */
	enum TextAlign { LEFT, CENTRE, RIGHT };

	/**
	 * Font resource. For each font you need to construct a Font
	 * resource and then use that resource with IText etc. A
	 * font has a name, size and can be bold. Here is an example
	 * of how to create and use a font.<br/><br/>
	 * <code>
	 * Font *myfont = Resource::create\<Font\>("myfont");<br/>
	 * myfont->name("Arial");<br/>
	 * myfont->size(12);<br/>
	 * myfont->bold(true);<br/><br/>
	 * IText *message = Instance::create\<IText\>("message1");<br/>
	 * message->text("Hello World");<br/>
	 * message->font(myfont);<br/>
	 * </code>
	 */
	class RESIMPORT Font : public cadence::Agent {

		public:

		OBJECT(Agent, Font);
		Font();
		Font(const char* name, int size, bool bold=false);
		Font(const cadence::doste::OID &res);
		~Font();

		/**
		 * Change the font size.
		 * @param v Font size.
		 */
		PROPERTY_WF(int, size, cadence::doste::Size);
		/**
		 * Get this fonts size.
		 * @return Font size.
		 */
		PROPERTY_RF(int, size, cadence::doste::Size);

		/**
		 * Change this fonts name.
		 * @param v The name of the font face.
		 */
		PROPERTY_WF(cadence::dstring, name,ix::name);
		/**
		 * This fonts current name.
		 * @return Name of the font.
		 */
		PROPERTY_RF(cadence::dstring, name,ix::name);

		/**
		 * Set bold to true or false.
		 * @param v True for bold text.
		 */
		PROPERTY_WF(bool, bold, ix::bold);
		/**
		 * Is this font bold.
		 * @return True if bold.
		 */
		PROPERTY_RF(bool, bold,ix::bold);

		PROPERTY_WF(wgd::Texture, texture, ix::texture);
		PROPERTY_RF(wgd::Texture, texture, ix::texture);

		void bind();
		void unbind();
		/** build the font if not already built (creates the texture) */
		void build() { if(m_needsbuild) buildFont(); }
		
		//DB_GET_INT(width, ix::width);
		//DB_SET_INT(width, ix::width);
		//DB_GET_INT(height, ix::height);
		//DB_SET_INT(height, ix::height);

		int CharWidth(char c);
		int CharX(char c);
		int CharY(char c);
		int CharHeight(char c);

		BEGIN_EVENTS(Agent);
		EVENT(evt_make, (*this)("_make"));
		EVENT(evt_changed,	(*this)(ix::name)(cadence::doste::modifiers::Seq)((*this))(ix::size)(cadence::doste::modifiers::Seq)((*this))(ix::bold));
		END_EVENTS;

		protected:

		bool m_needsbuild;

		struct CData {
			int x;
			int y;
			int width;
			int height;
		};

		CData m_cdata[128];

		virtual void buildFont();
		void destroyFont();

		//static void initialise();
		//static void finalise();
	};
};

#endif 
