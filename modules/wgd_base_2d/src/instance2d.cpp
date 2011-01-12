#include <wgd/instance2d.h>

using namespace wgd;

Instance2D::Instance2D() : Agent() {}
Instance2D::Instance2D(const OID &id) : Agent(id) {}
Instance2D::~Instance2D() {}

void Instance2D::draw(SceneGraph &graph, Camera2D *camera) {
	//?
}

void Instance2D::move(const vector2d &v) {
	position(position()+v);
}

void Instance2D::translate(const vector2d &v) {
	vector2d p = position();
	float ang = orientation();
	p.x += cos(ang) * v.x;
	p.y += sin(ang) * v.y;
	position(p);
}

vector2d Instance2D::worldPosition() {
	Instance2D *p = parent();
	if(p) {
		return p->worldPosition().translate(p->worldOrientation(), position());
	}
	return position();
}
void Instance2D::rotate(float r) {
	orientation(orientation() + r);
}
float Instance2D::worldOrientation() {
	if(parent()) {
		return orientation() + parent()->worldOrientation();
	}
	return orientation();
}
