#include <wgd/render.h>
#include <wgd/matrix.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

using namespace wgd;

Camera3D *Render::s_cam3d = 0;
Camera2D *Render::s_cam2d = 0;

void Render::begin(Camera3D *cam) {
	s_cam2d = 0;
	s_cam3d = cam;
}

void Render::end() {
	s_cam2d = 0;
	s_cam3d = 0;
}

void Render::transform(const matrix &mat) {
	#ifdef _MSC_VER
	__declspec(align(16)) float mat2[16];
	#else
	float mat2[16] __attribute__((aligned(16)));
	#endif
	mat.toOpenGL(mat2);
	glPushMatrix();
	glMultMatrixf(mat2);
}

void Render::untransform() {
	glPopMatrix();
}

void Render::sprite(Sprite *spr, int frame, const vector3d &pos) {
	if (s_cam3d == 0) return;
	sprite(spr,frame,pos,s_cam3d->quaternion());
}

void Render::sprite(Sprite *spr, int frame, const vector3d &pos, const quaternion &quat) {
	float mat[16];

	quat.createMatrix(mat);

	glPushMatrix();
	glTranslatef(pos.x,pos.y,pos.z);
	glMultMatrixf(mat);

	sprite(spr, frame);

	glPopMatrix();
}

void Render::sprite(Sprite *spr, int frame) {
	int cols = spr->columns();
	float tw = 1.0f / (float)cols;
	float th = 1.0f / (float)spr->rows();
	float tx = tw*(float)(frame%cols);
	float ty = th*(float)(frame/cols);

	Texture *tex = spr->texture();
	if (tex == 0) return;
	tex->bind();

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	glBegin(GL_QUADS);
		//Calculate texture coords from sprite info.
		glTexCoord2f(tx, ty);
		glVertex3f(-0.5f, 0.5f, 0.0);
		glTexCoord2f(tx+tw, ty);
		glVertex3f(0.5f, 0.5f, 0.0);
		glTexCoord2f(tx+tw, ty+th);
		glVertex3f(0.5f, -0.5f, 0.0);
		glTexCoord2f(tx, ty+th);
		glVertex3f(-0.5f, -0.5f, 0.0);
	glEnd();

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);

	tex->unbind();
}
