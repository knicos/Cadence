#include <wgd/camera3d.h>
#include <wgd/math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>


using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

//Events
namespace wgd {
	OnEvent(Camera3D, evt_orientation){
		m_quat.createRotation(orientation());
		m_quat = m_quat.conjugate();
	}
	IMPLEMENT_EVENTS(Camera3D, Camera);
};

Camera3D::Camera3D(const cadence::doste::OID &id) : Camera(id) {
	//some defaults
	if(get(ix::near)==Null) nearclip(1.0f);
	if(get("far")==Null) farclip(128.0f);
	if(get(ix::fov)==Null) fov(60.0f);
	
	registerEvents();
}
Camera3D::~Camera3D(){}

void Camera3D::bind(){
	float aspect = (float)m_width / m_height;
	
	float vfovY = fov() / aspect;
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if (get("frustum") == Null) {
		gluPerspective(vfovY, aspect, nearclip(), farclip());
	} else {
		OID frust = get("frustum");
		glFrustum(frust.get("left"), frust.get("right"), frust.get("bottom"), frust.get("top"), nearclip(), farclip());
	}
	
	//create view frustum for culling
	m_frustum.create(this);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glClear(GL_DEPTH_BUFFER_BIT);
	
	//Transform ModelView
	
	//rotate
	float matrix[16];
	m_quat.createMatrix(matrix);
	glMultMatrixf(matrix);

	//SKYBOX GOES HERE.

	//translate
	vector3d pos = position();
	glTranslatef(-pos.x, -pos.y, -pos.z);
	
	glEnable(GL_DEPTH_TEST);
	
}
void Camera3D::unbind(){
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
}

void Camera3D::move(const vector3d &v){
	position(position() + v);
}
void Camera3D::translate(const vector3d &v){
	position(position().translate(m_quat, v));
}

const Quaternion Camera3D::quaternion() const{
	return m_quat.conjugate();
}
void Camera3D::quaternion(const Quaternion &q){
	m_quat = q.conjugate();
}

void Camera3D::rotate(const vector3d &r){
	orientation(orientation() + r);
}

void Camera3D::lookat(const vector3d &target) {
	vector3d direction = (target-position()).normalise();
	
	//calculate pitch and yaw
	float yaw = -atan2(direction.z, direction.x) - (float)wgd::PI/2.0f;
	float dist = sqrt(direction.x*direction.x + direction.z*direction.z);
	float pitch = atan2(direction.y, dist);
	float roll = get(ix::roll);
	
	orientation(vector3d(pitch, yaw, roll));
}

vector3d Camera3D::project(const vector3d &w) {
	double proj[16], mdl[16];
	int vp[4] = {0, 0, m_width, m_height };
	projectionMatrix(proj);
	modelViewMatrix(mdl);
	
	double rx, ry, rz;
	gluUnProject(w.x, w.y, w.z, mdl, proj, vp, &rx, &ry, &rz);
	
	return vector3d((float)rx, m_height - (float)ry, (float)rz);
}

vector3d Camera3D::unProject(const vector3d &s) {
	double proj[16], mdl[16];
	int vp[4] = {0, 0, m_width, m_height };
	projectionMatrix(proj);
	modelViewMatrix(mdl);
	
	double rx, ry, rz;
	gluUnProject(s.x, m_height - s.y, s.z, mdl, proj, vp, &rx, &ry, &rz);
	
	return vector3d((float)rx, (float)ry, (float)rz);
}

void Camera3D::projectionMatrix(double *m) {
	float aspect = (float)m_width / m_height;
	double fovy = degToRad(fov()) / aspect;
	double f = 1.0 / tan( fovy / 2.0 ); //cotangent(fovy/2);
	double far = farclip();
	double near = nearclip();
	
	memset(m, 0, 16 * sizeof(double));
	m[0] = f / aspect;
	m[5] = f;
	m[10] = (far + near) / (near - far);
	m[11] = -1.0;
	m[14] = (2 * far * near) / (near - far);
}
void Camera3D::modelViewMatrix(double *m) {
	matrix mat, tm, rm;
	tm.translate(-position());
	m_quat.createMatrix(rm);
	matrix::fastMultiply(mat, rm, tm);
	
	//convert to doubles
	float gl[16];
	mat.toOpenGL(gl);
	for(int i=0; i<16; i++) m[i] = gl[i];
}
