/*
 * WGD Library
 *
 * Authors: 
 * Date: 17/9/2007
 *
 */

#ifndef _WGD_TEXTURE_
#define _WGD_TEXTURE_

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include <wgd/dll.h>
#include <cadence/agent.h>
#include <cadence/dstring.h>
#include <wgd/colour.h>
#include <cadence/file.h>
#include <wgd/loader.h>
#include <string.h>


namespace wgd {
	/**
	 * Used for texture names.
	 */
	typedef const cadence::doste::OID &TexName;
	//class Game;
	//class File;
	//class FrameBuffer;

	enum TextureFormat { RGBA, RGB, RGBA_HDR, RGB_HDR, DEPTH_16, DEPTH_24, DEPTH_32, ALPHA, LUMINANCE };

	class RESIMPORT TextureLoader : public Loader {
		public:
		TextureLoader(cadence::File *f) : Loader(f), m_data(0), m_width(0), m_height(0) {}
		virtual ~TextureLoader() {
			if (m_data != 0) delete [] m_data;
		};

		virtual bool load()=0;

		unsigned char *data() const { return m_data; }
		int width() const { return m_width; }
		int height() const { return m_height; }
		//bool alpha() const { return m_alpha; }
		//bool hdr() const { return m_float; }
		TextureFormat format() const { return m_format; }

		BASE_LOADER(TextureLoader);

		//protected:
		void data(unsigned char *d) { m_data = d; }
		void width(int w) { m_width = w; }
		void height(int h) { m_height = h; }
		//void alpha(bool a) { m_alpha = a; }
		//void hdr(bool f) { m_float = f; }
		void format(TextureFormat f) { m_format = f; }

		private:
		unsigned char *m_data;
		int m_width;
		int m_height;
		//bool m_alpha;
		//bool m_float;
		TextureFormat m_format;
	};

	class CustomTexture : public TextureLoader {
		public:
		CustomTexture() : TextureLoader(0) {}

		bool load() { return true; };
	};

	/**
	 * A texture resource. This resource will be loaded from the specified file
	 * when needed. TGA and BMP are currently supported. Multi-texturing is also
	 * possible using this class by specifying the texture unit when binding, note
	 * however that using multi-texturing is more involved than this. Instead of
	 * using a texture directly you could use Material which provides more options.<br/><br/>
	 * e.g.<br/>
	 * <code>
	 * //In initialisation.<br/>
	 * Resource::create\<Texture\>("plank")->filename("./data/plank.tga");<br/>
	 * //In update where you wish to use the texture.<br/>
	 * Resource::get\<Texture\>("plank")->bind();<br/><br/>
	 * </code>
	 * With the texture object you can call bind() when you want to use it. This
	 * class supports multi-texturing so you can call bind and specify the texture
	 * unit to use (defaults to 0). activate() is also provided to switch to
	 * a specified texture unit.<br/><br/>
	 * Here is an example of a texture in the config files:<br/><br/>
	 * <code>
	 * wgd(textures)(plank) = {(filename, "./data/plank.tga"),(clamp,false),(mipmap,true)};
	 * </code><br/><br/>
	 * @see Material
	 */
	class RESIMPORT Texture : public cadence::Agent {
		//friend class wgd::FrameBuffer;

		public:

		OBJECT(Agent, Texture);

		Texture();				///<default constructor
		Texture(int width, int height, TextureFormat format=RGBA, bool clamp=false); ///<create a blank texture
		Texture(cadence::File*);			///< load a texture
		Texture(const cadence::doste::OID &);	///< Database constructor - used by cadence
		Texture(const char* filename);		///< load a texture
		~Texture();

		/**
		 * Make a blank texture to generate procedurally. Use setPixels to actually
		 * define it properly. Note that these textures will not be regenerated if the
		 * screen resolution is changed, it is your responsibility.
		 */
		bool make(TextureFormat format);
		
		/** Is the texture loaded? */
		bool isLoaded() { return m_loaded; }

		/**
		 * Set a region of pixels within the OpenGL texture. You must disable
		 * texture compression for this to work correctly. Depending on your
		 * graphics card this may be very slow and should not be done all the
		 * time. The smaller the change the better.
		 * @param x X-Coordinate of region to change.
		 * @param y Y-Coordinate of region to change.
		 * @param width Width of region in pixels.
		 * @param height Height of region in pixels.
		 * @param data An array of 32bit colour information.
		 */
		template <typename T>
		bool setPixels(int x, int y, int width, int height, const T *data) {
			FormatMap map;
			lookupFormat(map, m_format);
			if (map.colourformat != T::format || x < 0 || y < 0 || (x+width) > (int)m_width || (y+height) > (int)m_height || !m_loaded) return false;
			bind();
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, map.format, map.type, (char*)data);
			unbind();
			return true;
		}

		template <typename T>
		bool setPixel(int x, int y, const T &pixel) {
			FormatMap map;
			lookupFormat(map, m_format);
			if (map.colourformat != T::format || x < 0 || y < 0 || x >= m_width || y >= m_height || !m_loaded) return false;
			bind();
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, map.format, map.type, (char*)&pixel);
			unbind();
			return true;
		}

		template <typename T>
		bool getPixels(T *data) {
			FormatMap map;
			lookupFormat(map, m_format);
			if (map.colourformat != T::format || !m_loaded) return false;
			bind();
			glGetTexImage(GL_TEXTURE_2D, 0, map.format, map.type, (char*)data);
			unbind();
			return true;
		}

		template <typename T>
		bool getPixels(int x, int y, int width, int height, T *data) {
			FormatMap map;
			lookupFormat(map, m_format);
			if (map.colourformat != T::format || x < 0 || y < 0 || (x+width) > (int)m_width || (y+height) > (int)m_height || !m_loaded || !keep() || !m_data) return false;

			int start = (x + (y*m_width));
			int offset = start;
			int offset2 = 0;

			T *idata = (T*)m_data;

			for (int i=0; i<height; i++) {
				memcpy((char*)&data[offset2], (char*)&idata[offset], sizeof(T)*width);
				offset2 += width;
				offset += m_width;
			}

			return true;
		}

		template <typename T>
		bool getPixel(int x, int y, T &data) {
			FormatMap map;
			lookupFormat(map, m_format);
			if (map.colourformat != T::format || x < 0 || y < 0 || x >= m_width || y >= m_height || !m_loaded || !keep()) return false;
			data = *(T*)&m_data[(x + (y*m_width)) * sizeof(T)];
			return true;
		}


		/**
		 * Get the width of the texture in pixels.
		 * @return Width.
		 */
		int width() const { return m_width; };
		/**
		 * Get the height of the texture in pixels.
		 * @return Height.
		 */
		int height() const { return m_height; };

		void width(int w) {
			m_width = w;
			set(ix::width, w);
		}

		void height(int h) {
			m_height = h;
			set(ix::height, h);
		}

		//bool depth() const { return m_depth; }
		//void depth(bool d) { m_depth = d; }

		/**
		 * Get the colour depth in bits per pixel.
		 * @return Colour Depth.
		 */
		//int colourDepth() const { return m_cdepth; };
		/**
		 * Use this texture by binding it to a specified texture unit.
		 * If you are not multi-texturing then don't specify a parameter
		 * and it will default to the first texture unit.
		 * @param num Texture unit number, 0 to maxTextureUnits()-1.
		 */
		void bind(int num=0);
		/**
		 * This will unbind and disable this texture. You do not need
		 * to call this if you are simply binding to another texture on
		 * the same texture unit.
		 */
		void unbind();

		/**
		 * Get the textures default colour. This is the colour used when make() is called.
		 */
		PROPERTY_RF(colour, defaultColour, "defaultcolour");
		/**
		 * Set the default colour. Used by make() when generating the texture.
		 */
		PROPERTY_WF(colour, defaultColour, "defaultcolour");

		/**
		 * Clamp the texture to the edge, do not repeat. Useful for
		 * something like a skybox where there must be no seams.
		 * @return True if the texture is clamped.
		 */
		PROPERTY_RF(bool, clamp, ix::clamp);
		/**
		 * Clamp the texture to the edge, do not repeat. Useful for
		 * something like a skybox where there must be no seams.
		 * @param v True to clamp, false will repeat.
		 */
		PROPERTY_WF(bool, clamp, ix::clamp);

		/**
		 * Should the texture be kept in system memory.
		 * Needed if you wish to get pixel values, eg use it as a heightmap.
		 */
		PROPERTY_WF(bool, keep, ix::keep);
		/** Is this texture kept in system memory. */
		PROPERTY_RF(bool, keep, ix::keep);

		/**
		 * Is this texture mipmapped.
		 * @return True if it is mipmapped.
		 */
		PROPERTY_RF(bool, mipmap, ix::mipmap);
		/**
		 * Mipmapping. This will produce better looking results as it correctly
		 * chooses texture detail with distance, however there are cases when you
		 * do not want mipmapping for better results, eg for a shader or sprite.
		 * @param v True to enable mipmapping.
		 */
		PROPERTY_WF(bool, mipmap, ix::mipmap);

		PROPERTY_RF(bool, nearest, "nearest");
		PROPERTY_WF(bool, nearest, "nearest");

		/**
		 * Should DXF texture compression be used. By default this is true and
		 * will allow you to use more or larger textures. This is OpenGL supported
		 * hardware texture compression extension. The disadvantage is that some
		 * gradient textures will have colour errors (e.g. skybox).
		 */
		PROPERTY_WF(bool, compress, ix::compress);
		/** Is this texture using hardware compression. */
		PROPERTY_RF(bool, compress, ix::compress);

		//PROPERTY_WF(bool, hdr, "hdr");
		//PROPERTY_RF(bool, hdr, "hdr");

		/**
		 * For HDR effects you can modify a normal texture with a glow texture. This can be
		 * a 24 or 32 bit texture that scales the orginary texture based on value of each component.
		 */
		PROPERTY_WF(cadence::dstring, glowFilename, ix::glowfilename);
		PROPERTY_RF(cadence::dstring, glowFilename, ix::glowfilename);

		/**
		 * Defines the maximum glow factor for a glow texture. In a glow texture 0 (black) corresponds
		 * to a factor of 1.0 or no change from original image. 255 (white) corresponds to a factor
		 * of glowMax(). This defaults to 10.0.
		 */
		PROPERTY_WF(float, glowMax, ix::glowmax);
		PROPERTY_RF(float, glowMax, ix::glowmax);

		/**
		 * Current filename.
		 * @return String representing the filename.
		 */
		//PROPERTY_RF(cadence::dstring, filename, ix::filename);
		PROPERTY_RF(cadence::File, file, "file");
		/**
		 * Textures filename. TGA (compressed and uncompressed) and BMP are supported
		 * texture types currently. You must set the filename before loading or using
		 * the texture.
		 * @param v The filename string
		 */
		//PROPERTY_WF(cadence::dstring, filename, ix::filename);
		PROPERTY_WF(cadence::File, file, "file");

		/**
		 * Force load the texture. If not called then the texture will not be loaded until
		 * bind is first called. If this first use is inside a display list then you
		 * will have problems so called load() first. This also validates the GLID.
		 */
		void load();
		
		/**
		 * OpenGL texture ID. Only valid after first use or load() have been called.
		 * @return OpenGL texture ID.
		 */
		GLuint getGLID() const { return m_glid; };

		/**
		 * Use to activate specific texturing units. Only needed if multi-texturing
		 * and you need to be certain of which texture unit is active. bind() and
		 * unbind() will automatically call this so immediately after bind() the
		 * corresponding texture unit will still be active.
		 * @param num Texture unit number, 0 to maxTextureUnits()-1.
		 */
		static void activate(int);

		/**
		 * Get the maximum number of supported texture units.
		 * @return Number of texture units.
		 */
		static int maxTextureUnits() { return s_maxtex; };

		static void initialise();
		static void finalise();

		BEGIN_EVENTS(Agent);
		EVENT(evt_reload, (*this)("reload"));
		//hdr
		//mipmap
		//nearest
		//file
		//compress
		//clamp
		END_EVENTS;

		private:
		unsigned int m_width;
		unsigned int m_height;
		//int m_cdepth;
		unsigned char *m_data;
		GLuint m_glid;
		bool m_loading;
		bool m_loaded;
		bool m_failed;
		int m_bound;
		TextureFormat m_format;
		//bool m_ishdr;
		//bool m_depth;

		bool buildTexture(TextureLoader *);
		bool makeHDR();

		static int s_maxtex;
		static int s_bind;
		static int s_maxmem;
		static int s_curmem;
		static Texture *s_curtex;

		struct FormatMap {
			int internal;
			int type;
			int format;
			ColourFormat colourformat;
		};
		bool lookupFormat(FormatMap &map, TextureFormat format);
	};
};

#endif
