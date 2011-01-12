#include <wgd/rendertarget.h>
#include <wgd/extensions.h>

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

RenderTarget::RenderTarget(const OID &o)
 : Agent(o) {
	
	//check extension
	if(!Extensions::hasFBO()) {
		std::cout << "Framebuffers not supported\n";
		std::cout.flush();
		return;
	}
	
	if (get("clear") == Null) clear(true);

	Texture *tex = texture();
	m_width = tex->get("width"); tex->width(m_width);
	m_height = tex->get("height"); tex->height(m_height);
	tex->compress(false);
	if (tex->isLoaded() == false)
		tex->make(RGBA_HDR);

	Texture *dtex = depthTexture();
	m_dbuf = 0;

	if (depth()) {
		if (dtex == 0) {
			WGDglGenRenderbuffersEXT(1, &m_dbuf);
			WGDglBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_dbuf);
			WGDglRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT32, m_width, m_height);
		} else {
			dtex->compress(false);
			dtex->width(m_width);
			dtex->height(m_height);
			if (dtex->isLoaded() == false) dtex->make(DEPTH_32);
		}
	}
	
	// create FBO itself
	WGDglGenFramebuffersEXT(1, &m_fbo);
	WGDglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
	WGDglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex->getGLID(), 0);
	if (depth()) {
		if (dtex == 0) {
			WGDglFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_dbuf);
		} else {
			WGDglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, dtex->getGLID(), 0);
		}
	}

	if (WGDglCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) {
		std::cout << "Framebuffer creation error\n";
	}

	WGDglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

RenderTarget::~RenderTarget() {
	WGDglDeleteFramebuffersEXT(1, &m_fbo);
	//Destroy texture?
	//Resource::destroy(m_tex);

	if (depth() && (m_dbuf != 0)) {
		WGDglDeleteRenderbuffersEXT(1, &m_dbuf);
	}
}

void RenderTarget::draw() {
	if (get(ix::source) == Null) {
		//Use camera and scene
		Camera *cam = camera();
		Scene *sc = scene();

		if (cam == 0 || sc == 0) return;

		//glEnable(GL_TEXTURE_2D);
		begin();
		if (clear()) doclear();

		if (Extensions::hasColourClamp()) {
			WGDglClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
			WGDglClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
		}

		//glViewport(0,m_height,m_width,m_height);
		cam->size(m_width, m_height);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(1);

		cam->bind();
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glShadeModel(GL_SMOOTH);
		glDisable(GL_TEXTURE_2D);
		//glEnable(GL_BLEND);
		float poss[] = {0.0f,0.0f,0.0f,1.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, poss);
		float amb[] = {0.2f,0.2f,0.2f, 1.0f};
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		float diff[] = {1.0f,1.0f,1.0f, 1.0f};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
		float spec[] = {1.0f,1.0f,1.0f, 1.0f};
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		sc->draw(cam);

		//glDisable(GL_BLEND);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		cam->unbind();

		glDepthMask(0);
		glDisable(GL_DEPTH_TEST);

		if (Extensions::hasColourClamp()) {
			WGDglClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_TRUE);
			WGDglClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FIXED_ONLY_ARB);
		}

		end();
	} else {
		//glEnable(GL_DEPTH_TEST);
		//glDepthMask(1);

		//Use rendertarget.
		RenderTarget *src = source();
		Material *mat = material();

		src->draw();

		begin();

		if (clear()) doclear();

		src->bind();
		if (mat) mat->bind();

		glColor4f(1.0,1.0,1.0,1.0);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2i(-1, -1);
		glTexCoord2f(1.0, 0.0); glVertex2i(1, -1);
		glTexCoord2f(1.0, 1.0); glVertex2i(1, 1);
		glTexCoord2f(0.0, 1.0); glVertex2i(-1, 1);
		glEnd();

		if (mat) mat->unbind();
		src->unbind();

		//glDepthMask(0);
		//glDisable(GL_DEPTH_TEST);
		end();
	}
}

void RenderTarget::begin() {
	WGDglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
	glViewport(0, 0, m_width, m_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(0, m_width, m_height, 0, 0, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderTarget::end() {
	WGDglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void RenderTarget::bind() {
	texture()->bind();
}

void RenderTarget::unbind() {
	texture()->unbind();
}

void RenderTarget::doclear() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | (depth() ? GL_DEPTH_BUFFER_BIT : 0));
}
