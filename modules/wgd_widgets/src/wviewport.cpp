#include <wgd/widgets/wviewport.h>
#include <wgd/window.h>
#include <wgd/extensions.h>

#include <iostream>

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

WViewport::WViewport(const cadence::doste::OID &id) : Widget(id) {

}

WViewport::~WViewport() {

}

void WViewport::draw(const ClipRect &rect) {

	if (get(ix::source) == Null) {
		//Use camera and scene
		Camera *cam = camera();
		Scene *sc = scene();

		if (cam == 0 || sc == 0) return;

		if (Extensions::hasColourClamp()) {
			WGDglClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
			WGDglClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
		}

		vector2d pos = screenPosition();
		int w = width();
		int h = height();
		int wh = wgd::window->height() - h;
		
		//clip window
		ClipRect r;
		r.top = (int)pos.y;
		r.left = (int)pos.x;
		r.right = r.left + w;
		r.bottom = r.top + h;
		clip(rect, r);
		
		cam->size(w, h);

		//glScissor((int)pos.x,wh-(int)pos.y,w,h);
		glViewport((int)pos.x,wh-(int)pos.y,w,h);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(1);

		cam->bind();
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glShadeModel(GL_SMOOTH);
		glDisable(GL_TEXTURE_2D);
		//glEnable(GL_BLEND);
		float poss[] = {0.0, 1.0f, 1.0f, 0.0};
		glLightfv(GL_LIGHT0, GL_POSITION, poss);
		float amb[] = {0.3f, 0.3f, 0.3f, 1.0f};
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		float diff[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
		float spec[] = {0.5f, 0.5f, 0.5f, 1.0f};
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
		
		//clear colour - would be cool to do some stencil stuff so we can draw to an arbitrary shape
		if(get("clear")!=Null) {
			colour c = get("clear");
			if(c.a > 0.0f) {
				glClearColor(c.r, c.g, c.b, c.a);
				glClear(GL_COLOR_BUFFER_BIT);
			}
		}
		
		sc->draw(cam);

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		cam->unbind();

		if (Extensions::hasColourClamp()) {
			WGDglClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_TRUE);
			WGDglClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FIXED_ONLY_ARB);
		}

		glDepthMask(0);
		glDisable(GL_DEPTH_TEST);

		glViewport(0,0,wgd::window->width(),wgd::window->height());

	} else {

		RenderTarget *src = source();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		src->draw();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		vector2d pos = screenPosition();
		int w = width();
		int h = height();
		int wh = wgd::window->height() - h;

		glScissor((int)pos.x,wh-(int)pos.y,w,h);
		glViewport((int)pos.x,wh-(int)pos.y,w,h);

		glMatrixMode(GL_PROJECTION);						
		glPushMatrix();										// Store The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		glOrtho(0,w,0,h,-1,1);			// Set Up An Ortho Screen
		glMatrixMode(GL_MODELVIEW);							
		glPushMatrix();										// Store The Modelview Matrix
		glLoadIdentity();									// Reset The Modelview Matrix
	
		//Reset raster position
		glRasterPos2f(0.0,0.0);
		glTranslatef(0.0, 0.0, -1.0);

		//glEnable(GL_DEPTH_TEST);
		//glDepthMask(1);

		//Use rendertarget.
		src->bind();
		glColor4f(1.0,1.0,1.0,1.0);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
		glTexCoord2f(1.0, 0.0); glVertex3f((float)w, 0.0, 0.0);
		glTexCoord2f(1.0, 1.0); glVertex3f((float)w, (float)h, 0.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(0.0, (float)h, 0.0);
		glEnd();
		src->unbind();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glViewport(0,0,wgd::window->width(),wgd::window->height());
	}

	drawChildren(rect);
}
