#include <wgd/iline.h>

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

ILine3D::ILine3D(const OID &obj) : Instance3D(obj) {

}

ILine3D::~ILine3D() {

}

void ILine3D::draw(SceneGraph &graph, Camera3D *camera) {
	wgd::colour c = colour();
	vector3d p1 = point1();
	vector3d p2 = point2();

	//Disable lighting
	glDisable(GL_LIGHTING);
	
	glLineWidth((float)width());
	glBegin(GL_LINES);
		glColor4f(c.r, c.g, c.b, c.a);
		glVertex3f(p1.x,p1.y,p1.z);
		glVertex3f(p2.x,p2.y,p2.z);
	glEnd();
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	//Re-enable lighting
	glEnable(GL_LIGHTING);
}
