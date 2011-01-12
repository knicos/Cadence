#include <wgd/isprite3d.h>
#include <wgd/quaternion.h>

namespace wgd {
	OnEvent(ISprite3D, evt_sprite) {
		m_sprite.sprite(sprite());
	}
	OnEvent(ISprite3D, evt_frame) {
		m_sprite.frame(frame());
	}
	OnEvent(ISprite3D, evt_size) {
		m_sprite.size(width(), height());
	}
	OnEvent(ISprite3D, evt_colour) {
		m_sprite.colour(colour());
	}
	IMPLEMENT_EVENTS(ISprite3D, Instance3D);
};

using namespace wgd;

ISprite3D::ISprite3D() : Instance3D() {
	registerEvents();
	set(ix::type, type());
	if(mode()==Null) mode("point");
	if(get(ix::width)==Null) { width(1.0f); height(1.0f); }
	m_start = WGD.get(ix::time);
}

ISprite3D::ISprite3D(const OID &id) : Instance3D(id) {
	registerEvents();
	if(mode()==Null) mode("point");
	if(get(ix::width)==Null) { width(1.0f); height(1.0f); }
	m_start = WGD.get(ix::time);
}

ISprite3D::~ISprite3D() {}


void ISprite3D::draw(SceneGraph &graph, Camera3D *camera) {
	m_sprite.position(position());
	if(mode()==OID("aligned")) {
		m_sprite.direction(direction());
	} else if(mode() == OID("absolute")) {
		wgd::quaternion quat;
		quat.createRotation(orientation());
		m_sprite.orientation(quat);
	} else {
		m_sprite.angle(orientation().z);
	}
	m_sprite.draw(graph, camera);
}

void ISprite3D::frame(int f) {
	if(fps() != 0.0) {
		float time = WGD.get(ix::time);
		m_start = time - f * fps();
	}
	set(ix::frame, f);
}
