#include <wgd/camera2d.h>

using namespace wgd;

Camera2D::Camera2D(const OID &id) : Camera(id) {
	if(zoom() == 0) zoom(1.0f);
}
Camera2D::~Camera2D() {}

void Camera2D::bind() {
	double right = (double)m_width / 2.0;
	double left = -right;
	double top = (double)m_height / 2.0;
	double bottom = -top;

	//set up projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(left, right, bottom, top, 1.0, 128.0);
	
	//set up modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//transform modelview
	float r = orientation();
	float s = zoom();
	vector2d t = position();
	glRotatef(r * 57.29578f,  0.0f, 0.0f, 1.0f);
	glScalef(s, -s, s); 
	glTranslatef(-t.x, -t.y, -10.0f);
	
	glEnable(GL_DEPTH_TEST);
}

void Camera2D::unbind() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
}

void Camera2D::move(const vector2d &v) {
	position(position() + v);
}

void Camera2D::translate(const vector2d &v) {
	position(position().translate(orientation(), v));
}

void Camera2D::rotate(float r) {
	orientation(orientation() + r);
}
vector2d Camera2D::project(const vector2d&) {
	//meh - later
	return vector2d();
}
vector2d Camera2D::unproject(const vector2d&) {
	return vector2d();
}

