#include <wgd/sprite3d.h>
#include <wgd/matrix.h>

using namespace wgd;

Sprite3D::Sprite3D() : m_drawable(0), m_width(1.0f), m_height(1.0f), m_sprite(0), m_frame(0), m_mode(0) {}
Sprite3D::Sprite3D(Sprite* sprite, const vector3d &position, const quaternion &orientation, float width, float height, int frame)
	: m_drawable(0), m_width(width), m_height(height), m_sprite(sprite), m_frame(frame), m_position(position), m_orientation(orientation), m_mode(2) {}

Sprite3D::Sprite3D(Sprite* sprite, const vector3d &position, float width, float height, int frame, float angle)
	: m_drawable(0), m_width(width), m_height(height), m_sprite(sprite), m_frame(frame), m_position(position), m_angle(angle), m_mode(0) {}

Sprite3D::Sprite3D(Sprite* sprite, const vector3d &position, const vector3d &direction, float width, float height, int frame)
	: m_drawable(0), m_width(width), m_height(height), m_sprite(sprite), m_frame(frame), m_position(position), m_direction(direction), m_mode(1) {}

Sprite3D::~Sprite3D() {
	if(m_drawable) {
		if(!m_drawable->queued) {
			m_drawable->deleteData();
			delete m_drawable;
		} else m_drawable->discard = true;
	}
}

DrawableBase *Sprite3D::draw(SceneGraph &graph, Camera3D *camera) {
	return graph.addAlpha(drawable(camera));
}

wgd::Drawable<SpriteVertex> *Sprite3D::drawable(Camera3D* cam) {
	if(!m_drawable) setup();
	switch(m_mode) {
	case 0: if(cam) makeOrientated(cam); break;
	case 1: if(cam) makeAligned(cam); break;
	case 2: makeAbsolute(); break; //absolute doesn't change with the camera
	}
	if(m_sprite) m_drawable->surface = m_sprite->get(ix::texture);
	else m_drawable->surface = Null;
	
	//set distance
	if(cam) m_drawable->distance = cam->position().distanceSquared(m_position);
	
	return m_drawable;
}

void Sprite3D::sprite(Sprite* spr) { m_sprite = spr; setFrame(m_frame); }
void Sprite3D::size(float width, float height) { m_width = width; m_height = height; }
void Sprite3D::position(const vector3d &v) { m_position = v; }
void Sprite3D::direction(const vector3d &v) { m_direction = v.normalise(); m_mode = 1; }
void Sprite3D::orientation(const quaternion &v) { m_orientation = v; m_mode=2; }
void Sprite3D::angle(float ang) { m_angle = ang; m_mode=0; }
void Sprite3D::colour(const wgd::colour &c) {
	if(!m_drawable) setup();
	for(int i=0; i<4; i++) m_drawable->data[i].colour = c;
}

void Sprite3D::setup() {
	//	set up drawable (4 vertices, GL_QUADS, etc)
	m_drawable = new Drawable<SpriteVertex>;
	m_drawable->data = new SpriteVertex[4];
	
	//initialise normals
	for(int i=0; i<4; i++) {
		m_drawable->data[i].normal.x = 0.0f;
		m_drawable->data[i].normal.y = 0.0f;
		m_drawable->data[i].normal.z = 1.0f;
		m_drawable->data[i].colour.r = 1.0f;
		m_drawable->data[i].colour.g = 1.0f;
		m_drawable->data[i].colour.b = 1.0f;
		m_drawable->data[i].colour.a = 1.0f;
	}
	
	m_drawable->size = 4;
	m_drawable->discard = false;
	m_drawable->mode = GL_QUADS;
	m_drawable->flags = (DrawFlags)(DEPTH_TEST + DEPTH_WRITE);
	setFrame(m_frame);
}

void Sprite3D::setFrame(int f) {
	if(m_sprite && m_drawable) {
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
				
		m_frame=f;
	}
}

void Sprite3D::drawSprite() {
	float w = m_width / 2;
	float h = m_height / 2;
	
	for(int i=0; i<4; i++) {
		m_drawable->data[i].position.x = (i<2)? w : -w;
		m_drawable->data[i].position.y = (i==1 || i==2)? h : -h;
		m_drawable->data[i].position.z = 0;
	}
}

void Sprite3D::makeOrientated(Camera3D* cam) {
	//transformation matrix
	matrix mat, mat2;
	cam->quaternion().createMatrix(mat);
	mat2.translate(m_position);
	matrix::fastMultiply(m_drawable->transform, mat2, mat);
	
	//sprite as usual
	drawSprite();
	
}
void Sprite3D::makeAligned(Camera3D *cam) {
	//sprite more complicated, or crazy matrix stuff.
	vector3d c2 = m_position - cam->position() + cam->direction();
	vector3d tangent = c2.cross(m_direction).normalise();
	vector3d normal  = tangent.cross(m_direction).normalise();
	//construct matrix from these vectors - may have got it the wrong way round...
	matrix mat, mat2;
	mat.m[0][0] = tangent.x;
	mat.m[0][1] = tangent.y;
	mat.m[0][2] = tangent.z;
	
	mat.m[1][0] = m_direction.x;
	mat.m[1][1] = m_direction.y;
	mat.m[1][2] = m_direction.z;
	
	mat.m[2][0] = normal.x;
	mat.m[2][1] = normal.y;
	mat.m[2][2] = normal.z;
	
	mat2.translate(m_position);
	matrix::fastMultiply(m_drawable->transform, mat2, mat);
	
	//sprite as usual
	drawSprite();
}

void Sprite3D::makeAbsolute() {
	matrix mat, mat2;
	m_orientation.createMatrix(mat);
	mat2.translate(m_position);
	matrix::fastMultiply(m_drawable->transform, mat2, mat);
	
	//sprite as usual
	drawSprite();
}


