#include <wgd/instance3d.h>

namespace wgd {
	OnEvent(Instance3D, evt_orientation) {
		m_update=true;
	}
	OnEvent(Instance3D, evt_position) {
		m_update=true;
	}

	IMPLEMENT_EVENTS(Instance3D, Agent);
};

wgd::Instance3D::Instance3D() : Agent(), m_update(true) {
	registerEvents();
}
wgd::Instance3D::Instance3D(const cadence::doste::OID &id) : Agent(id), m_update(true) {
	registerEvents();
}

void wgd::Instance3D::draw(SceneGraph &graph, Camera3D *camera) {	
}

const wgd::matrix &wgd::Instance3D::localMatrix() {
	//recalculate matrix if stuff has changed since last call
	if(m_update) {
		m_update=false;
		m_quat.createRotation(orientation());
		matrix t, r;
		t.translate(position());
		m_quat.createMatrix(r);
		matrix::fastMultiply(m_localMatrix, t, r);
	}
	return m_localMatrix;
}

void wgd::Instance3D::localMatrix(const wgd::matrix& m) {
	m_localMatrix = m;
	m_update = false;
}

