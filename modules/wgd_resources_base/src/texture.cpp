
#ifdef WIN32
#include <windows.h>
#endif

#include <wgd/texture.h>
#include <wgd/extensions.h>
#include <wgd/colour.h>
#include <cadence/dstring.h>
#include <cadence/messages.h>
#include <wgd/loader.h>
#include <wgd/texturebmp.h>
#include <wgd/texturetga.h>
#include <wgd/texturepng.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#ifdef LINUX
#include <GL/glext.h>
#include <GL/glx.h>
//#include "../config.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace cadence;
using namespace cadence::doste;
using namespace wgd;

int wgd::Texture::s_bind = 0;
int wgd::Texture::s_maxtex = 1;
int wgd::Texture::s_maxmem = 0x8000000;	//128Mb
int wgd::Texture::s_curmem = 0;
Texture *wgd::Texture::s_curtex = 0;

IMPLEMENT_LOADER(TextureLoader);

void wgd::Texture::initialise() {
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &s_maxtex);
	Loader::regFileType<TextureBMP>();
	Loader::regFileType<TextureTGA>();
	Loader::regFileType<TexturePNG>();
}

void wgd::Texture::finalise() {

}

namespace wgd {
	OnEvent(Texture, evt_reload) {
		if (m_loaded & !m_failed) {
			m_loaded = false;
			m_failed = false;
			m_loading = false;

			if (!keep()) {
				glDeleteTextures(1, &m_glid);
			} else {
				if (m_data != 0) {
					delete [] m_data;
					m_data=0;
				}
			}
		}
	}

	IMPLEMENT_EVENTS(Texture,Agent);
};

wgd::Texture::Texture()
	: m_data(0), m_loading(false), m_loaded(false), m_failed(false), m_bound(-1) {
	//Default to using texture compression
	compress(true);
	set(Type, type());
	registerEvents();
}

wgd::Texture::Texture(int w, int h, TextureFormat format, bool clmp)
	: m_data(0), m_loading(false), m_loaded(false), m_failed(false), m_bound(-1) {
	set(Type, type());
	registerEvents();
	width(w);
	height(h);
	clamp(clmp);
	make(format);
}

wgd::Texture::Texture(cadence::File *f)
	: m_data(0), m_loading(false), m_loaded(false), m_failed(false), m_bound(-1) {
	
	//Default to using texture compression
	compress(true);
	set(Type, type());
	registerEvents();
	//set texture file
	file(f);
}

wgd::Texture::Texture(const char* filename)
	: m_data(0), m_loading(false), m_loaded(false), m_failed(false), m_bound(-1) {
	//Default to using texture compression
	compress(true);
	set(Type, type());
	registerEvents();
	//set texture file
	file(new LocalFile(filename));
}


wgd::Texture::Texture(const OID &res)
	: Agent(res), m_data(0), m_loading(false), m_loaded(false), m_failed(false), m_bound(-1) {
	
	//Default to using texture compression
	if (get(ix::compress) == Null) {
		compress(true);
	}

	registerEvents();
}

bool wgd::Texture::make(TextureFormat format) {
	TextureLoader *l = new CustomTexture();
	l->data(0);
	l->width(width());
	l->height(height());
	//l->alpha(true);
	//l->hdr(hdr());
	l->format(format);
	//m_cdepth = 32;

	//Load this texture into OpenGL.
	m_loaded = true;
	return buildTexture(l);
};

void wgd::Texture::load() {
	if (m_loaded || m_loading)
		return;

	File *f = file();
	if (f == 0) return;

	Info(Info::LOADING, "Loading texture " + f->filename());

	TextureLoader *l = Loader::create<TextureLoader>(f);
	if (l == 0) return;

	if (!l->load()) return;

	m_loading = true;
	buildTexture(l);
	delete l;
	m_loaded = true;
	m_loading = false;
}

bool wgd::Texture::makeHDR() {
	/*//First save current texture pointers.
	unsigned char *difdata = m_data;
	int difw = m_width;
	int difh = m_height;
	int difd = m_cdepth;

	//Load glow texture.
	cadence::dstring fname(get(ix::glowfilename));
	std::cout << "Loading glow texture '" << (const char*)fname << "'\n";
	if (!loadFromFile(fname)) {
		std::cout << "Failed to load glow texture '" << (const char*)fname << "'\n";
		return false;
	}

	//Process to generate float texture data.
	unsigned char *gdata = m_data;
	//int gwidth = m_width;
	//int gheight = m_height;
	int gdepth = m_cdepth;

	float sfactor = glowMax();

	//Allocate float data
	float *fdata = new float[difw*difh*4];
	int j=0;
	int k=0;
	for (int i=0; i<difw*difh*4; i+=4) {
		fdata[i] = ((float)difdata[j] / 255.0f) * (1.0f + ((float)gdata[k] / 255.0f) * sfactor);
		fdata[i+1] = ((float)difdata[j+1] / 255.0f) * (1.0f + ((float)gdata[k+1] / 255.0f) * sfactor);
		fdata[i+2] = ((float)difdata[j+2] / 255.0f) * (1.0f + ((float)gdata[k+2] / 255.0f) * sfactor);
		if (difd == 32)
			fdata[i+3] = ((float)difdata[j+3] / 255.0f);
		else
			fdata[i+3] = 1.0;

		j += (difd >> 3);
		k += (gdepth >> 3);
	}

	delete [] gdata;
	delete [] difdata;
	m_data = (unsigned char*)fdata;
	m_width = difw;
	m_height = difh;
	m_cdepth = 32;
	m_ishdr = true;*/

	return true;
}

void wgd::Texture::bind(int num) {
	//either cannot bind or already bound.
	if (m_failed || (s_curtex == this))
		return;

	//DMsg msg;
	//msg << "Binding " << *this << " to " << num << "\n";

	//Load if not already loaded from file.
	if (!m_loaded)
		load();

	//Select the correct texture unit.
	activate(num);

	//Textures must be enabled and bound
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_glid);
	s_bind = num;
	m_bound = num;
	s_curtex = this;
}

void wgd::Texture::unbind() {
	//Nothing is bound so return.
	if (m_bound == -1)
		return;

	//DMsg msg;
	//msg << "Unbinding " << *this << " to " << m_bound << "\n";

	//Select correct texture unit.
	activate(m_bound);

	//Disable OpenGL textures for this texture unit.
	glDisable(GL_TEXTURE_2D);
	m_bound = -1;
	s_curtex = 0;
}

void wgd::Texture::activate(int num) {
	//Check to see that its a valid texture unit.
	if (num >= s_maxtex) {
		Error(0, "Exceeding maximum supported texture units.");
		return;
	}

	//If not already using the correct texture unit then switch it.
	if (s_bind != num) {
		WGDglActiveTextureARB(GL_TEXTURE0_ARB+num);
		s_bind = num;
	}
}

#undef True

bool wgd::Texture::buildTexture(TextureLoader *loader) {

	//Update the database values for width and height.
	if (get(ix::width) == Null) set(ix::width, loader->width());
	if (get(ix::width) == Null) set(ix::height, loader->height());

	m_width = loader->width();
	m_height = loader->height();
	m_format = loader->format();

	//If we are not keeping the texture in system memory then
	//Send to video memory via OpenGL texture objects.
	if (!keep()) {
		//Create one texture object
		glGenTextures(1, &m_glid);
		//Not alignment at end of each row, packed pixel array.
		glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
		//Select this new texture object
		glBindTexture(GL_TEXTURE_2D, m_glid);
	
		//Set the texture parameters. Clamp is used for skybox etc...
		if (clamp())
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // If the u,v coordinates overflow the range 0,1 the image is repeated
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		} else { 
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		if (!nearest()) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		FormatMap map;
		lookupFormat(map, m_format);
	
		//Check that this is a valid texture resolution.
		int mwidth;
		glTexImage2D(GL_PROXY_TEXTURE_2D, 0, map.internal,  loader->width(), loader->height(), 0, map.format, map.type, NULL);
		glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&mwidth);
		if (mwidth == 0) {
			m_failed = true;
			Error(0, dstring("Unsupported texture resolution (") + loader->width() + "x" + loader->height() + ").");
			return false;
		}
	
		//Use GLU to build mimaps if required. Otherwise use standard OpenGL
		if (mipmap()) {
			gluBuild2DMipmaps(GL_TEXTURE_2D, map.internal, loader->width(), loader->height(), map.format, map.type, loader->data());
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, map.internal, loader->width(), loader->height(), 0, map.format, map.type, loader->data());
		}
	};

	//s_curmem += m_width*m_height*(m_cdepth >> 3);
	//std::cout << "Texture memory usage: " << ((float)s_curmem/(1024.0*1024.0)) << "Mb\n";

	return true;
}

bool Texture::lookupFormat(FormatMap &map, TextureFormat format) {
	switch (format) {
	case RGBA:	map.internal = (Extensions::hasTextureCompression() && compress()) ? GL_COMPRESSED_RGBA_ARB : GL_RGBA;
			map.type = GL_UNSIGNED_BYTE;
			map.format = GL_RGBA;
			map.colourformat = RGBA_CHAR;
			break;

	case RGB:	map.internal = (Extensions::hasTextureCompression() && compress()) ? GL_COMPRESSED_RGB_ARB : GL_RGB;
			map.type = GL_UNSIGNED_BYTE;
			map.format = GL_RGB;
			map.colourformat = RGB_CHAR;
			break;

	case RGBA_HDR:	map.internal = GL_RGBA16F_ARB;
			map.type = GL_HALF_FLOAT_ARB;
			map.format = GL_RGBA;
			map.colourformat = RGBA_FLOAT;
			break;

//	case RGB_HDR:	map.internal = GL_RGB16F_ARB;
//			map.type = GL_HALF_FLOAT_ARB;
//			map.format = GL_RGB;
//			map.colourformat = RGB_FLOAT;
//			break;

//	case DEPTH_16:	map.internal = GL_DEPTH_COMPONENT16;
//			map.type = GL_UNSIGNED_BYTE;
//			map.format = GL_DEPTH_COMPONENT;
//			map.colourformat = RG_CHAR;
//			break;

	case DEPTH_24:	map.internal = GL_DEPTH_COMPONENT24;
			map.type = GL_UNSIGNED_BYTE;
			map.format = GL_DEPTH_COMPONENT;
			map.colourformat = RGB_CHAR;
			break;

	case DEPTH_32:	map.internal = GL_DEPTH_COMPONENT32;
			map.type = GL_UNSIGNED_BYTE;
			map.format = GL_DEPTH_COMPONENT;
			map.colourformat = RGBA_CHAR;
			break;

	case ALPHA:	map.internal = GL_ALPHA;
			map.type = GL_UNSIGNED_BYTE;
			map.format = GL_ALPHA;
			map.colourformat = R_CHAR;
			break;

	case LUMINANCE:	map.internal = GL_LUMINANCE;
			map.type = GL_UNSIGNED_BYTE;
			map.format = GL_LUMINANCE;
			map.colourformat = R_CHAR;
			break;

	default:	return false;
	}

	return true;
}

wgd::Texture::~Texture() {
	//Should check first that it was loaded?
	if (m_loaded && !keep()) {
		glDeleteTextures(1, &m_glid);
	} else {
		if (m_data != 0) {
			delete [] m_data;
		}
	}

	set(ix::width, Null);
	set(ix::height, Null);

	//s_curmem -= m_width*m_height*(m_cdepth>>3);
}
