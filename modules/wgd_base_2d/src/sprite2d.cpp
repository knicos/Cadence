#include <wgd/sprite2d.h>

using namespace wgd;

Sprite2D::Sprite2D() : m_sprite(0), m_angle(0), m_frame(0), m_depth(0) { setup(); }
Sprite2D::Sprite2D(Sprite* spr, const vector2d &pos, float s, float ang, int f)
 : m_sprite(spr), m_position(pos), m_angle(ang), m_frame(f), m_depth(0) {
	scale(s);
	setup();
}

void Sprite2D::sprite(Sprite *spr) { m_sprite = spr; m_changed = true; }
void Sprite2D::position(const vector2d &p) { m_position = p; }
void Sprite2D::angle(float ang) { m_angle = ang; }
void Sprite2D::scale(float s) { stretch(s, s); }
void Sprite2D::depth(float d) { m_depth=d; }
void Sprite2D::stretch(float sx, float sy) { //need to check for null
	if(!m_sprite) return;
	m_sprite->texture()->load();
	int cols = m_sprite->columns();
	int rows = m_sprite->rows();
	if(cols==0) cols=1;
	if(rows==0) rows=1;
	m_size.x = m_sprite->texture()->width()  / cols * sx;
	m_size.y = m_sprite->texture()->height() / rows * sy;
	m_changed = true;
}

void Sprite2D::size(float width, float height) {
	m_size.x = width;
	m_size.y = height;
	m_changed = true;
}

void Sprite2D::colour(const wgd::colour &c) {
	for(int i=0; i<4; i++) {
		m_drawable->data[i].colour = c;
	}
}

void Sprite2D::frame(int f) {
	m_frame = f;
	if(m_sprite) {
		f=-f; //for some reason, its backwards
		int cols = m_sprite->columns();
		int rows = m_sprite->rows();
		float tw = 1.0f / (float)cols;
		float th = 1.0f / (float)m_sprite->rows();
		float tx = tw*(float)(cols-(f%cols));
		float ty = th*(float)(rows-1+(f/cols));
		
		m_drawable->data[0].texcoords[0].u = tx+tw;
		m_drawable->data[0].texcoords[0].v = ty;
		m_drawable->data[1].texcoords[0].u = tx+tw;
		m_drawable->data[1].texcoords[0].v = ty+th;
		
		m_drawable->data[2].texcoords[0].u = tx;
		m_drawable->data[2].texcoords[0].v = ty+th;	
		m_drawable->data[3].texcoords[0].u = tx;
		m_drawable->data[3].texcoords[0].v = ty;
	}
}

void Sprite2D::draw(SceneGraph &graph) {
	graph.add(drawable());
}

wgd::Drawable<SpriteVertex> *Sprite2D::drawable() {
	if(m_changed) build();
	
	matrix t, r;
	t.translate(m_position.x, m_position.y, -m_depth);
	r.rotate(vector3d(0.0f, 0.0f, m_angle));
	m_drawable->transform = t * r;
	m_drawable->distance = m_depth;
	
	return m_drawable;
}

void Sprite2D::setup() {
	//create drawable
	m_drawable = new Drawable<SpriteVertex>;
	m_drawable->mode = GL_QUADS;
	m_drawable->flags = DEPTH_TEST | DEPTH_WRITE;
	
	m_drawable->deleteData();
	m_drawable->data = new SpriteVertex[4];
	m_drawable->size = 4;
	
	vector3d normal(0.0, 1.0, 0.0); //normals should not do anything but ill stick them in anyway
	for(int i=0; i<4; i++) { 
		m_drawable->data[i].normal = normal;
		m_drawable->data[i].colour = wgd::colour(1.0f, 1.0f, 1.0f);
		m_drawable->data[i].position = vector3d();
	}
	
	build();
}

void Sprite2D::build() {
	float w = m_size.x / 2.0f;
	float h = m_size.y / 2.0f;
	
	for(int i=0; i<4; i++) {
		m_drawable->data[i].position.x = (i<2)? w : -w;
		m_drawable->data[i].position.y = (i==1 || i==2)? -h : h;
		m_drawable->data[i].position.z = 0;
	}
	
	//set surface
	if(m_sprite && m_sprite->texture()) m_drawable->surface = *m_sprite->texture();
	
	frame(m_frame);
	m_changed = false;
}

 //////////////////////////////////////////////////////////////////

namespace wgd {
	OnEvent(ISprite2D, evt_size) {
		m_sprite.size(width(), height());
	}
	OnEvent(ISprite2D, evt_sprite) {
		m_sprite.sprite(sprite());
	}
	OnEvent(ISprite2D, evt_frame) {
		m_sprite.frame(frame());
	}
	IMPLEMENT_EVENTS(ISprite2D, Instance2D);
};

ISprite2D::ISprite2D(const OID &id) : Instance2D(id) { 
	if(width()==0 && height()==0) { width(32); height(32); }
	registerEvents();
}
ISprite2D::ISprite2D() {
	set(ix::type, type());
	if(width()==0 && height()==0) { width(32); height(32); }
	registerEvents(); 
}
ISprite2D::~ISprite2D() {}

void ISprite2D::draw(SceneGraph &graph, Camera2D *camera) {
	m_sprite.position(position());
	m_sprite.depth(depth());
	m_sprite.angle(orientation());
	m_sprite.draw(graph);
}

void ISprite2D::scale(float s) {
	stretch(s, s);
}

void ISprite2D::stretch(float sx, float sy) {
	Sprite *spr = sprite(); //need to check for null
	int cols = spr->columns();
	int rows = spr->rows();
	if(cols==0) cols=1;
	if(rows==0) rows=1;
	width(spr->texture()->width() / cols * sx);
	height(spr->texture()->height() / rows * sy);
}
