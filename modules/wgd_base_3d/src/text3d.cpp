#include <wgd/text3d.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning ( disable : 4996 )
#endif

using namespace wgd;

Text3D::Text3D()
	: m_drawable(0), m_angle(0), m_scale(1.0f), m_align(CENTRE), m_colour(0,0,0), m_text(0), m_font(0) {
	setup();
}
Text3D::Text3D(const char* t, Font* font, const vector3d &position, float scale, TextAlign align, float angle) 
	: m_drawable(0), m_position(position), m_angle(angle), m_scale(scale), m_align(align), m_colour(0,0,0), m_text(0), m_font(font) {
	text(t);
	setup();
}
Text3D::~Text3D() {
	if(m_text) delete [] m_text;
	if(m_drawable) m_drawable->destroy();
}

DrawableBase *Text3D::draw(SceneGraph &graph, Camera3D *camera) {
	return graph.addAlpha(drawable(camera));
}

wgd::Drawable<SpriteVertex> *Text3D::drawable(Camera3D* cam) {
	if(m_changed) build();
	//change matrix
	matrix rcam, p, s, r;
	cam->quaternion().createMatrix(rcam);
	p.translate(m_position);
	s.scale(m_scale, m_scale, m_scale);
	r.rotate(0, 0, m_angle);
	m_drawable->transform = p * rcam * r * s;
	
	return m_drawable;
}


void Text3D::text(const char* t) {
	if(m_text) delete [] m_text;
	m_text = strdup(t);
	m_changed = true;
}

void Text3D::colour(const wgd::colour& c) {
	m_colour = c;
}

void Text3D::setup() {
	m_drawable = new Drawable<SpriteVertex>;
	m_drawable->mode = GL_QUADS;
	m_drawable->flags = DEPTH_TEST | DEPTH_WRITE | ALPHA_TEST;
}

void Text3D::build() {
	if(!m_text || !m_font) return;
	
	m_font->build();
	m_drawable->surface = *m_font->texture();
	
	m_drawable->deleteData();
	SpriteVertex v;
	
	float tx, ty, tw, th; //source
	float texw = (float)m_font->texture()->width();
	float texh = (float)m_font->texture()->height();
	
	float x=0, y=0;
	vector3d normal(0.0f, 0.0f, 1.0f);
	
	int chr = -1;
	while(m_text[++chr]) {
		tx = (float)m_font->CharX(m_text[chr]) / texw;
		ty = (float)m_font->CharY(m_text[chr]) / texh;
		tw = (float)m_font->CharWidth(m_text[chr]) / texw;
		th = (float)m_font->CharHeight(m_text[chr]) / texh;
		
		v.colour = m_colour;
		v.position = vector3d(x, y, 0.0f);
		v.normal = normal;
		v.texcoords[0].u = tx;
		v.texcoords[0].v = ty;
		m_drawable->add(v);
		
		v.colour = m_colour;
		v.position = vector3d(x, y-fabs(th), 0.0f);
		v.normal = normal;
		v.texcoords[0].u = tx;
		v.texcoords[0].v = ty+th;
		m_drawable->add(v);
		
		x += tw;
		v.colour = m_colour;
		v.position = vector3d(x, y-fabs(th), 0.0f);
		v.normal = normal;
		v.texcoords[0].u = tx+tw;
		v.texcoords[0].v = ty+th;
		m_drawable->add(v);
		
		v.colour = m_colour;
		v.position = vector3d(x, y, 0.0f);
		v.normal = normal;
		v.texcoords[0].u = tx+tw;
		v.texcoords[0].v = ty;
		m_drawable->add(v);
	}
	
	//adjust position with alignment;
	if(m_align != LEFT) {
		float delta;
		if(m_align==CENTRE) delta=x/2.0f; else delta = x;
		
		for(unsigned int i=0; i<m_drawable->size; i++) {
			m_drawable->data[i].position.x -= delta;
		}
	}
	
}



////////////////////////////////////////////
//instane stuff
IText3D::IText3D(const wgd::OID &id) : Instance3D(id) { registerEvents(); }
IText3D::IText3D() : Instance3D() { registerEvents(); }
void IText3D::draw(SceneGraph &graph, Camera3D *cam) { m_text.draw(graph, cam); }
void IText3D::align(TextAlign align) {
	switch(align) {
	case LEFT: set("align", ix::left); break;
	case RIGHT: set("align", ix::right); break;
	case CENTRE: set("align", OID("centre")); break;
	}
}
TextAlign IText3D::align() {
	OID a = get("align");
	if(a==ix::left) return LEFT;
	if(a==ix::right) return RIGHT;
	return CENTRE;
}


namespace wgd {
	OnEvent(IText3D, evt_text) {  m_text.text((const char*)text()); }
	OnEvent(IText3D, evt_scale) { if(scale()) m_text.scale(scale()); }
	OnEvent(IText3D, evt_angle) { m_text.angle(angle()); }
	OnEvent(IText3D, evt_align) { m_text.align(align()); }
	OnEvent(IText3D, evt_colour) { m_text.colour(colour()); }
	OnEvent(IText3D, evt_font) { m_text.font(font()); }
	OnEvent(IText3D, evt_position) { m_text.position(position()); }
	IMPLEMENT_EVENTS(IText3D, Instance3D);
};
