#include <wgd/font.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#ifdef LINUX
#include <GL/glx.h>
#include <X11/Xlib.h>
#endif

#include <wgd/window.h>
#include <wgd/colour.h>

#include <iostream>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

wgd::Font::Font() : Agent() {
	registerEvents();
}

wgd::Font::Font(const OID &res)
	: Agent(res) {
	//m_base = 0xFFFFFFFF;
	//m_needsbuild = false;

	registerEvents();

	//m_needsbuild = true;
	
	//if (get(ix::name) != Null)
	//	buildFont();
}

wgd::Font::Font(const char* fname, int fsize, bool fbold) : Agent() {
	registerEvents();
	name(fname);
	size(fsize);
	bold(fbold);
	buildFont();
}

wgd::Font::~Font() {
	//if (m_base != 0xFFFFFFFF)
	//	destroyFont();
}

namespace wgd {
	OnEvent(Font, evt_changed) {
		m_needsbuild = true;
		//if (m_needsbuild) buildFont();
	}
	
	OnEvent(Font, evt_make) {
		
	}

	IMPLEMENT_EVENTS(Font,Agent);
};

int wgd::Font::CharHeight(char c) {
	return m_cdata[(int)c].height;
}

int wgd::Font::CharWidth(char c) {
	return m_cdata[(int)c].width;
}

int wgd::Font::CharX(char c) {
	return m_cdata[(int)c].x;
}

int wgd::Font::CharY(char c) {
	return m_cdata[(int)c].y;
}

void wgd::Font::bind() {
	if (m_needsbuild) buildFont();
	m_needsbuild = false;
	Texture *tex = texture();
	if (tex != 0) tex->bind();
}

void wgd::Font::unbind() {
	Texture *tex = texture();
	if (tex != 0) tex->unbind();
}

void wgd::Font::buildFont() {

	//Delete any existing font display lists
	//if (m_base != 0xFFFFFFFF)
	//	destroyFont();

	//Create new GL display lists for this font
	//m_base = glGenLists(96);

	Texture *tex;
		//Make a texture for it
	if (get(ix::texture) == Null) {
		tex = new Texture();
		tex->compress(false);
		tex->nearest(true);
		if (size() < 32) {
			tex->width(256);
			tex->height(256);
		} else {
			tex->width(512);
			tex->height(512);
		}
		texture(tex);
		//Processor::processAll();
		tex->make(RGBA);
		//Processor::processAll();
	} else {
		tex = texture();
		if (!tex->isLoaded()) {
			tex->compress(false);
			tex->nearest(true);
			if (size() < 32) {
				tex->width(256);
				tex->height(256);
			} else {
				tex->width(512);
				tex->height(512);
			}
			tex->make(RGBA);
		}
	}
	
	if (tex == 0) return;
	
	#ifdef WIN32
	HFONT font;
	HFONT oldfont;
	
	#ifdef UNICODE
	int wLen = name().size() + 1;
	wchar_t *wFont = new wchar_t[wLen];
	mbstowcs(wFont, name(), wLen);
	#else
	char *wFont = strdup(name());
	#endif
	
	
	//Ask windows for a font
	font = CreateFont(	-size(),//Height
				0,	//Width
				0,	//Angle
				0,	//Orientation
				(bold()) ? FW_BOLD : FW_REGULAR,//Weight
				FALSE,	//Italic
				FALSE,	//UNDERLINE
				FALSE,	//Strikeout
				ANSI_CHARSET,
				OUT_TT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY,
				FF_DONTCARE | DEFAULT_PITCH,
				wFont);	//Font name

	HDC memDC = CreateCompatibleDC(wgd::window->getHDC());
	//Select this new font and save the current one.
	oldfont = (HFONT)SelectObject(memDC, font);
	//Get windows to generate display lists for this font
	//wglUseFontBitmaps(memDC, 32, 96, m_base);

	HBITMAP bmp = CreateCompatibleBitmap(memDC, tex->width(),tex->height());
	HBITMAP oldbmp = (HBITMAP)SelectObject(memDC, bmp);
	
	char letter = '@';
	int x = 0;
	int y = 0;// size();
	int w;
	int h;
	//GetCharWidth(memDC, letter, letter, &w);
	TEXTMETRIC tmet;
	GetTextMetrics(memDC, &tmet);
	SetTextColor(memDC, 0x00FFFFFF);
	SetBkColor(memDC, 0x00000000);

	w = tmet.tmAveCharWidth;
	h = tmet.tmHeight;
	//width(w);
	//height(h);

	for (int i=32; i<128; i++) {
		//if (i >= 65 && i <= 90)
		//	letter = i+33;
		//else
		letter = i;
		GetCharWidth(memDC, letter, letter, &w);
		
		#ifdef UNICODE
		wchar_t *wbuf = new wchar_t[2];
		mbstowcs(wbuf, &letter, 2);
		TextOut(memDC, x, y, (LPCWSTR)wbuf, 1);
		#else
		TextOut(memDC, x, y, &letter, 1);
		#endif

		m_cdata[i].width = w;
		m_cdata[i].x = x;
		m_cdata[i].y = y;
		m_cdata[i].height = h;

		x += w;
		if (x+w > tex->width()) {
			y += h;
			x = 0;
		}
	};

	BITMAPINFO binfo;
	binfo.bmiHeader.biSize = sizeof(binfo);
	binfo.bmiHeader.biWidth = tex->width();
	binfo.bmiHeader.biHeight = -tex->height();
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biBitCount = 32;
	binfo.bmiHeader.biCompression = BI_RGB;
	binfo.bmiHeader.biSizeImage = 0;
	colour4i *data = new colour4i[tex->width()*tex->height()];
	GetDIBits(memDC, bmp, 0, tex->width(), data, &binfo, DIB_RGB_COLORS);

	for (int i=0; i<tex->width()*tex->height(); i++) {
		if (data[i].r != 0 || data[i].g != 0 || data[i].b != 0)
			data[i].a = 255;
		//data[i].r = 255;
	}

	if (!tex->setPixels(0,0,tex->width(),tex->height(), data)) {
		std::cout << "Failed to set font pixels\n";
	}

	delete [] data;

	//Restore the previous font.
	SelectObject(memDC, oldfont);
	SelectObject(memDC, oldbmp);

	DeleteObject(bmp);
	DeleteObject(font);
	DeleteDC(memDC);
	#endif

	#ifdef LINUX
	XFontStruct *font;

	//Linux needs a font string so create it
	char *fstring = new char[1000];
	sprintf(fstring,"-*-%s-%s-r-*-*-*-%i-75-75-*-*-*-*", (const char*)name(), ((bold()) ? "bold" : "medium"), size()*10);
	//std::cout << "Font String: " << fstring << std::endl;

	//Ask X to find and then load this font if it exists.
	font = XLoadQueryFont(wgd::window->getXDisplay(), fstring);
			//"-*-helvetica-bold-r-normal--24-*-*-*-p-*-iso8859-1");
	delete [] fstring;

	//Font did not exist so load the default font which surely will exist
	if (font == NULL) {
		font = XLoadQueryFont(wgd::window->getXDisplay(), "fixed");
		std::cout << "Unable to load correct font.\n";
	}

	//Ask glX to create display lists using this X font.
	//glXUseXFont(font->fid, 32, 96, m_base);

	unsigned long valuemask = 0;
	XGCValues values;
	Pixmap pix = XCreatePixmap(wgd::window->getXDisplay(), wgd::window->getXWindow(), tex->width(), tex->height(), 32);
	GC gc = XCreateGC(wgd::window->getXDisplay(), pix, valuemask, &values);
	XSetFont(wgd::window->getXDisplay(), gc, font->fid);
	XSetForeground(wgd::window->getXDisplay(), gc, BlackPixel(wgd::window->getXDisplay(), wgd::window->getXScreen()));
	XFillRectangle(wgd::window->getXDisplay(), pix, gc, 0, 0, tex->width(), tex->height());
	XSetForeground(wgd::window->getXDisplay(), gc, WhitePixel(wgd::window->getXDisplay(), wgd::window->getXScreen()));


	char letter = '@';
	int x = 0;
	int h = font->max_bounds.ascent + font->max_bounds.descent;
	int y = font->max_bounds.ascent;
	int w = XTextWidth(font, &letter, 1);
	
	std::cout << "Font height = " << h << "\n";
	//width(w);
	//height(size());
	
	//XCharStruct charinfo;

	for (int i=32; i<128; i++) {
		letter = i;
		w = XTextWidth(font, &letter, 1);
		//XTextExtents(font, &letter, 1, 0, 0, 0, &charinfo);
		
		XDrawString(wgd::window->getXDisplay(), pix, gc, x, y, &letter, 1);

		m_cdata[i].width = w;
		m_cdata[i].height = h;
		m_cdata[i].x = x;
		m_cdata[i].y = y-font->max_bounds.ascent;

		x += w;
		if (x+w >= tex->width()) {
			y += h;
			x = 0;
		}
	};

	XImage *img = XGetImage(wgd::window->getXDisplay(), pix, 0, 0, tex->width(), tex->height(), AllPlanes, ZPixmap);

	//wgd::cout << "Image bits: " << img->depth << "\n";

	colour4i *data = (colour4i*)img->data;
	for (int i=0; i<tex->width()*tex->height(); i++) {
		if (data[i].r != 0 || data[i].g != 0 || data[i].b != 0)
			data[i].a = 255;
		data[i].r = 255;
		data[i].g = 255;
		data[i].b = 255;
	}

	if (img == 0) {
		std::cout << "No font image\n";
	} else if (img->data == 0) {
		std::cout << "No font image data\n";
	} else {
		if (!tex->setPixels(0,0,tex->width(),tex->height(), data)) {
			std::cout << "Failed to set font pixels\n";
		}
	}

	XFreePixmap(wgd::window->getXDisplay(), pix);
	XFreeGC(wgd::window->getXDisplay(), gc);

	XFreeFont(wgd::window->getXDisplay(), font);
	#endif
}

void wgd::Font::destroyFont() {
	//glDeleteLists(m_base, 96);
}
