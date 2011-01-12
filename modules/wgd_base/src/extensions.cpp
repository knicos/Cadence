#include <wgd/extensions.h>
#include <wgd/common.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <string.h>
#include <iostream>

using namespace wgd;
using namespace cadence;

bool Extensions::s_multitex = false;
bool Extensions::s_texcompress = false;
bool Extensions::s_shaders = false;
bool Extensions::s_sse = false;
bool Extensions::s_mmx = false;
bool Extensions::s_framebuf = false;
bool Extensions::s_halffloat = false;
bool Extensions::s_multisample = false;
bool Extensions::s_vbo = false;
bool Extensions::s_colorbuf = false;

BASEIMPORT PFNGLACTIVETEXTUREARBPROC WGDglActiveTextureARB = 0;
BASEIMPORT PFNGLMULTITEXCOORD2FARBPROC WGDglMultiTexCoord2fARB = 0;
BASEIMPORT PFNGLCLIENTACTIVETEXTUREARBPROC WGDglClientActiveTextureARB = 0;
//#endif
//#ifndef HAVE_GLCOMPRESSTEXTURE
BASEIMPORT PFNGLCOMPRESSEDTEXIMAGE3DARBPROC    WGDglCompressedTexImage3DARB = 0;
BASEIMPORT PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    WGDglCompressedTexImage2DARB = 0;
BASEIMPORT PFNGLCOMPRESSEDTEXIMAGE1DARBPROC    WGDglCompressedTexImage1DARB = 0;
BASEIMPORT PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC WGDglCompressedTexSubImage3DARB = 0;
BASEIMPORT PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC WGDglCompressedTexSubImage2DARB = 0;
BASEIMPORT PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC WGDglCompressedTexSubImage1DARB = 0;
BASEIMPORT PFNGLGETCOMPRESSEDTEXIMAGEARBPROC   WGDglGetCompressedTexImageARB = 0;

BASEIMPORT PFNGLCREATESHADERPROC WGDglCreateShader = 0;
BASEIMPORT PFNGLSHADERSOURCEPROC WGDglShaderSource = 0;
BASEIMPORT PFNGLCOMPILESHADERPROC WGDglCompileShader = 0;

BASEIMPORT PFNGLCREATEPROGRAMPROC WGDglCreateProgram = 0;
BASEIMPORT PFNGLATTACHSHADERPROC WGDglAttachShader = 0;
BASEIMPORT PFNGLLINKPROGRAMPROC WGDglLinkProgram = 0;

BASEIMPORT PFNGLGETPROGRAMIVPROC WGDglGetProgramiv = 0;
BASEIMPORT PFNGLGETSHADERIVPROC WGDglGetShaderiv = 0;
BASEIMPORT PFNGLGETPROGRAMINFOLOGPROC WGDglGetProgramInfoLog = 0;
BASEIMPORT PFNGLGETSHADERINFOLOGPROC WGDglGetShaderInfoLog = 0;

BASEIMPORT PFNGLISPROGRAMPROC WGDglIsProgram = 0;

BASEIMPORT PFNGLGETUNIFORMLOCATIONPROC WGDglGetUniformLocation = 0;
BASEIMPORT PFNGLGETATTRIBLOCATIONPROC WGDglGetAttribLocation = 0;

BASEIMPORT PFNGLUSEPROGRAMPROC WGDglUseProgram = 0;

BASEIMPORT PFNGLUNIFORM1FVPROC WGDglUniform1fv = 0;
BASEIMPORT PFNGLUNIFORM1FPROC WGDglUniform1f = 0;
BASEIMPORT PFNGLUNIFORM2FPROC WGDglUniform2f = 0;
BASEIMPORT PFNGLUNIFORM3FPROC WGDglUniform3f = 0;
BASEIMPORT PFNGLUNIFORM4FPROC WGDglUniform4f = 0;

BASEIMPORT PFNGLUNIFORM1IVPROC WGDglUniform1iv = 0;
BASEIMPORT PFNGLUNIFORM1IPROC WGDglUniform1i = 0;

BASEIMPORT PFNGLVERTEXATTRIB1FVPROC WGDglVertexAttrib1fv = 0;
BASEIMPORT PFNGLVERTEXATTRIB1FPROC WGDglVertexAttrib1f = 0;
BASEIMPORT PFNGLVERTEXATTRIB2FPROC WGDglVertexAttrib2f = 0;
BASEIMPORT PFNGLVERTEXATTRIB3FPROC WGDglVertexAttrib3f = 0;
BASEIMPORT PFNGLVERTEXATTRIB4FPROC WGDglVertexAttrib4f = 0;
BASEIMPORT PFNGLBINDATTRIBLOCATIONPROC WGDglBindAttribLocation = 0;

BASEIMPORT PFNGLENABLEVERTEXATTRIBARRAYPROC WGDglEnableVertexAttribArray = 0;
BASEIMPORT PFNGLDISABLEVERTEXATTRIBARRAYPROC WGDglDisableVertexAttribArray = 0;
BASEIMPORT PFNGLVERTEXATTRIBPOINTERPROC WGDglVertexAttribPointer = 0;

BASEIMPORT PFNGLDELETESHADERPROC WGDglDeleteShader = 0;
BASEIMPORT PFNGLDELETEPROGRAMPROC WGDglDeleteProgram = 0;

BASEIMPORT PFNGLPOINTPARAMETERFVPROC WGDglPointParameterfv = 0;
BASEIMPORT PFNGLPOINTPARAMETERFPROC WGDglPointParameterf = 0;

BASEIMPORT PFNGLGENBUFFERSARBPROC WGDglGenBuffersARB = 0;					// VBO Name Generation Procedure
BASEIMPORT PFNGLBINDBUFFERARBPROC WGDglBindBufferARB = 0;					// VBO Bind Procedure
BASEIMPORT PFNGLBUFFERDATAARBPROC WGDglBufferDataARB = 0;					// VBO Data Loading Procedure
BASEIMPORT PFNGLDELETEBUFFERSARBPROC WGDglDeleteBuffersARB = 0;				// VBO Deletion Procedure


BASEIMPORT PFNGLGENRENDERBUFFERSEXTPROC WGDglGenRenderbuffersEXT = 0;
BASEIMPORT PFNGLBINDFRAMEBUFFEREXTPROC WGDglBindFramebufferEXT = 0;
BASEIMPORT PFNGLBINDRENDERBUFFEREXTPROC WGDglBindRenderbufferEXT = 0;
BASEIMPORT PFNGLDELETEFRAMEBUFFERSEXTPROC WGDglDeleteFramebuffersEXT = 0;
BASEIMPORT PFNGLDELETERENDERBUFFERSEXTPROC WGDglDeleteRenderbuffersEXT = 0;
BASEIMPORT PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC WGDglFramebufferRenderbufferEXT = 0;
BASEIMPORT PFNGLFRAMEBUFFERTEXTURE2DEXTPROC WGDglFramebufferTexture2DEXT = 0;
BASEIMPORT PFNGLGENFRAMEBUFFERSEXTPROC WGDglGenFramebuffersEXT = 0;
BASEIMPORT PFNGLRENDERBUFFERSTORAGEEXTPROC WGDglRenderbufferStorageEXT = 0;
BASEIMPORT PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC WGDglCheckFramebufferStatusEXT = 0;
BASEIMPORT PFNGLCLAMPCOLORARBPROC WGDglClampColorARB=0;

void Extensions::initialise() {
	char *ext = (char*)glGetString(GL_EXTENSIONS);

	if (ext == 0)
		return;

	if (strstr(ext, "GL_ARB_texture_compression")) s_texcompress = true;
	if (strstr(ext, "GL_ARB_multitexture")) s_multitex = true;
	if (strstr(ext, "GL_EXT_framebuffer_object")) s_framebuf = true;
	if (strstr(ext, "GL_ARB_half_float_pixel")) s_halffloat = true;
	if (strstr(ext, "GL_ARB_multisample")) s_multisample = true;
	if (strstr(ext, "GL_ARB_vertex_shader")) s_shaders = true;
	if (strstr(ext, "GL_ARB_vertex_buffer_object")) s_vbo = true;
	if (strstr(ext, "GL_ARB_color_buffer_float")) s_colorbuf = true;

	SCpuidRegs regs;
	getCpuid(1,regs);
	s_sse = (regs.edx & 0x01000000) ? true : false;	//Do we have SSE instructions
	s_mmx = (regs.edx & 0x00400000) ? true : false;	//Do we have MMX instructions

	if (s_shaders) Info(0,"We have shaders");
	if (s_framebuf) Info(0,"We have framebuffers");
	if (s_halffloat) Info(0,"We have half-float");

	//Multi-texturing
	#ifdef WIN32
		WGDglActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
		WGDglMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
		WGDglClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
	#endif
	
		/* Get the OpenGL ARB extention function pointers if they exist */
	#ifdef LINUX
		//#ifndef HAVE_GLMULTITEXTURE
		WGDglActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glXGetProcAddress((const GLubyte*)"glActiveTextureARB");
		WGDglMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)glXGetProcAddress((const GLubyte*)"glMultiTexCoord2fARB");
		WGDglClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)glXGetProcAddress((const GLubyte*)"glClientActiveTextureARB");
		//#endif
	#endif

	//Texture Compression
	#ifdef WIN32
		/* ARB_texture_compression */
		//glCompressedTexImage3DARB = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)wglGetProcAddress("glCompressedTexImage3DARB");
		WGDglCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");
		//glCompressedTexImage1DARB = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)wglGetProcAddress("glCompressedTexImage1DARB");
		//glCompressedTexSubImage3DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)wglGetProcAddress("glCompressedTexSubImage3DARB");
		//glCompressedTexSubImage2DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexSubImage2DARB");
		//glCompressedTexSubImage1DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)wglGetProcAddress("glCompressedTexSubImage1DARB");
		WGDglGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)wglGetProcAddress("glGetCompressedTexImageARB");
	#endif

	#ifdef LINUX
		//#ifndef HAVE_GLCOMPRESSTEXTURE
		/* ARB_texture_compression */
		//glCompressedTexImage3DARB = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)wglGetProcAddress("glCompressedTexImage3DARB");
		WGDglCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)glXGetProcAddress((const GLubyte*)"glCompressedTexImage2DARB");
		//glCompressedTexImage1DARB = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)wglGetProcAddress("glCompressedTexImage1DARB");
		//glCompressedTexSubImage3DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)wglGetProcAddress("glCompressedTexSubImage3DARB");
		//glCompressedTexSubImage2DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexSubImage2DARB");
		//glCompressedTexSubImage1DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)wglGetProcAddress("glCompressedTexSubImage1DARB");
		WGDglGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)glXGetProcAddress((const GLubyte*)"glGetCompressedTexImageARB");
		//#endif
	#endif

	//Shader
	#ifdef WIN32
		WGDglCreateShader	= (PFNGLCREATESHADERPROC)	wglGetProcAddress("glCreateShader");
		WGDglShaderSource	= (PFNGLSHADERSOURCEPROC)	wglGetProcAddress("glShaderSource");
		WGDglCompileShader	= (PFNGLCOMPILESHADERPROC)	wglGetProcAddress("glCompileShader");
		
		WGDglCreateProgram	= (PFNGLCREATEPROGRAMPROC)	wglGetProcAddress("glCreateProgram");
		WGDglAttachShader	= (PFNGLATTACHSHADERPROC)	wglGetProcAddress("glAttachShader");
		WGDglLinkProgram	= (PFNGLLINKPROGRAMPROC)	wglGetProcAddress("glLinkProgram");
		
		WGDglGetProgramiv	= (PFNGLGETPROGRAMIVPROC)	wglGetProcAddress("glGetProgramiv");
		WGDglGetShaderiv	= (PFNGLGETSHADERIVPROC)	wglGetProcAddress("glGetShaderiv");
		
		WGDglIsProgram	= (PFNGLISPROGRAMPROC)		wglGetProcAddress("glIsProgram");
		
		WGDglGetProgramInfoLog	= (PFNGLGETPROGRAMINFOLOGPROC)	wglGetProcAddress("glGetProgramInfoLog");
		WGDglGetShaderInfoLog 	= (PFNGLGETSHADERINFOLOGPROC) 	wglGetProcAddress("glGetShaderInfoLog");
		
		WGDglGetUniformLocation	= (PFNGLGETUNIFORMLOCATIONPROC)	wglGetProcAddress("glGetUniformLocation");
		WGDglGetAttribLocation	= (PFNGLGETATTRIBLOCATIONPROC)	wglGetProcAddress("glGetAttribLocation");
		
		WGDglUseProgram	= (PFNGLUSEPROGRAMPROC)		wglGetProcAddress("glUseProgram");
		
		WGDglUniform1fv	= (PFNGLUNIFORM1FVPROC)		wglGetProcAddress("glUniform1fv");
		WGDglUniform1f	= (PFNGLUNIFORM1FPROC)		wglGetProcAddress("glUniform1f");
		WGDglUniform2f	= (PFNGLUNIFORM2FPROC)		wglGetProcAddress("glUniform2f");
		WGDglUniform3f	= (PFNGLUNIFORM3FPROC)		wglGetProcAddress("glUniform3f");
		WGDglUniform4f	= (PFNGLUNIFORM4FPROC)		wglGetProcAddress("glUniform4f");
		
		WGDglUniform1iv	= (PFNGLUNIFORM1IVPROC)		wglGetProcAddress("glUniform1iv");
		WGDglUniform1i	= (PFNGLUNIFORM1IPROC)		wglGetProcAddress("glUniform1i");
		
		WGDglVertexAttrib1fv= (PFNGLVERTEXATTRIB1FVPROC)	wglGetProcAddress("glVertexAttrib1fv");
		WGDglVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)		wglGetProcAddress("glVertexAttrib1f");
		WGDglVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)		wglGetProcAddress("glVertexAttrib2f");
		WGDglVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)		wglGetProcAddress("glVertexAttrib3f");
		WGDglVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)		wglGetProcAddress("glVertexAttrib4f");
		
		WGDglBindAttribLocation 	   = (PFNGLBINDATTRIBLOCATIONPROC)	wglGetProcAddress("glBindAttribLocation");
		
		WGDglEnableVertexAttribArray  = (PFNGLENABLEVERTEXATTRIBARRAYPROC)	wglGetProcAddress("glEnableVertexAttribArray");
		WGDglDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)	wglGetProcAddress("glDisableVertexAttribArray");
		WGDglVertexAttribPointer	   = (PFNGLVERTEXATTRIBPOINTERPROC)	wglGetProcAddress("glVertexAttribPointer");
		
		WGDglDeleteShader	= (PFNGLDELETESHADERPROC) 	wglGetProcAddress("glDeleteShader");
		WGDglDeleteProgram	= (PFNGLDELETEPROGRAMPROC)	wglGetProcAddress("glDeleteProgram");
	#endif

	#ifdef LINUX
		WGDglCreateShader	= (PFNGLCREATESHADERPROC)	glXGetProcAddress((unsigned char*)"glCreateShader");
		WGDglShaderSource	= (PFNGLSHADERSOURCEPROC)	glXGetProcAddress((unsigned char*)"glShaderSource");
		WGDglCompileShader	= (PFNGLCOMPILESHADERPROC)	glXGetProcAddress((unsigned char*)"glCompileShader");
		
		if (WGDglCreateShader == 0) {
			s_shaders = false;
		}

		WGDglCreateProgram	= (PFNGLCREATEPROGRAMPROC)	glXGetProcAddress((unsigned char*)"glCreateProgram");
		WGDglAttachShader	= (PFNGLATTACHSHADERPROC)	glXGetProcAddress((unsigned char*)"glAttachShader");
		WGDglLinkProgram	= (PFNGLLINKPROGRAMPROC)	glXGetProcAddress((unsigned char*)"glLinkProgram");
		
		WGDglGetProgramiv	= (PFNGLGETPROGRAMIVPROC)	glXGetProcAddress((unsigned char*)"glGetProgramiv");
		WGDglGetShaderiv	= (PFNGLGETSHADERIVPROC)	glXGetProcAddress((unsigned char*)"glGetShaderiv");

		WGDglIsProgram		= (PFNGLISPROGRAMPROC)		glXGetProcAddress((unsigned char*)"glIsProgram");
		
		WGDglGetProgramInfoLog	= (PFNGLGETPROGRAMINFOLOGPROC)	glXGetProcAddress((unsigned char*)"glGetProgramInfoLog");
		WGDglGetShaderInfoLog 	= (PFNGLGETSHADERINFOLOGPROC) 	glXGetProcAddress((unsigned char*)"glGetShaderInfoLog");
		
		WGDglGetUniformLocation	= (PFNGLGETUNIFORMLOCATIONPROC)	glXGetProcAddress((unsigned char*)"glGetUniformLocation");
		WGDglGetAttribLocation	= (PFNGLGETATTRIBLOCATIONPROC)	glXGetProcAddress((unsigned char*)"glGetAttribLocation");
		
		WGDglUseProgram	= (PFNGLUSEPROGRAMPROC)		glXGetProcAddress((unsigned char*)"glUseProgram");
		
		WGDglUniform1fv	= (PFNGLUNIFORM1FVPROC)		glXGetProcAddress((unsigned char*)"glUniform1fv");
		WGDglUniform1f	= (PFNGLUNIFORM1FPROC)		glXGetProcAddress((unsigned char*)"glUniform1f");
		WGDglUniform2f	= (PFNGLUNIFORM2FPROC)		glXGetProcAddress((unsigned char*)"glUniform2f");
		WGDglUniform3f	= (PFNGLUNIFORM3FPROC)		glXGetProcAddress((unsigned char*)"glUniform3f");
		WGDglUniform4f	= (PFNGLUNIFORM4FPROC)		glXGetProcAddress((unsigned char*)"glUniform4f");
		
		WGDglUniform1iv	= (PFNGLUNIFORM1IVPROC)		glXGetProcAddress((unsigned char*)"glUniform1iv");
		WGDglUniform1i	= (PFNGLUNIFORM1IPROC)		glXGetProcAddress((unsigned char*)"glUniform1i");
		
		WGDglVertexAttrib1fv= (PFNGLVERTEXATTRIB1FVPROC)	glXGetProcAddress((unsigned char*)"glVertexAttrib1fv");
		WGDglVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)		glXGetProcAddress((unsigned char*)"glVertexAttrib1f");
		WGDglVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)		glXGetProcAddress((unsigned char*)"glVertexAttrib2f");
		WGDglVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)		glXGetProcAddress((unsigned char*)"glVertexAttrib3f");
		WGDglVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)		glXGetProcAddress((unsigned char*)"glVertexAttrib4f");
		
		WGDglBindAttribLocation 	   = (PFNGLBINDATTRIBLOCATIONPROC)	glXGetProcAddress((unsigned char*)"glBindAttribLocation");
		
		WGDglEnableVertexAttribArray  = (PFNGLENABLEVERTEXATTRIBARRAYPROC)	glXGetProcAddress((unsigned char*)"glEnableVertexAttribArray");
		WGDglDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress((unsigned char*)"glDisableVertexAttribArray");
		WGDglVertexAttribPointer	   = (PFNGLVERTEXATTRIBPOINTERPROC)	glXGetProcAddress((unsigned char*)"glVertexAttribPointer");
		
		WGDglDeleteShader	= (PFNGLDELETESHADERPROC) 	glXGetProcAddress((unsigned char*)"glDeleteShader");
		WGDglDeleteProgram	= (PFNGLDELETEPROGRAMPROC)	glXGetProcAddress((unsigned char*)"glDeleteProgram");
	#endif

	//Point Sprites
	#ifdef LINUX
		WGDglPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)glXGetProcAddress((unsigned char*)"glPointParameterfv");
		WGDglPointParameterf = (PFNGLPOINTPARAMETERFPROC)glXGetProcAddress((unsigned char*)"glPointParameterf");
	#endif
	#ifdef WIN32
		WGDglPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)wglGetProcAddress("glPointParameterfv");
		WGDglPointParameterf = (PFNGLPOINTPARAMETERFPROC)wglGetProcAddress("glPointParameterf");
	#endif

	//VBOs
	#ifdef LINUX
		WGDglGenBuffersARB = (PFNGLGENBUFFERSARBPROC)glXGetProcAddress((unsigned char*)"glGenBuffersARB");
		WGDglBindBufferARB = (PFNGLBINDBUFFERARBPROC)glXGetProcAddress((unsigned char*)"glBindBufferARB");
		WGDglBufferDataARB = (PFNGLBUFFERDATAARBPROC)glXGetProcAddress((unsigned char*)"glBufferDataARB");
		WGDglDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)glXGetProcAddress((unsigned char*)"glDeleteBuffersARB");
	#endif
	#ifdef WIN32
		WGDglGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
		WGDglBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
		WGDglBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
		WGDglDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	#endif

	//FBOs
	#ifdef LINUX
		WGDglGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) glXGetProcAddress((unsigned char*)"glGenRenderbuffersEXT");
		WGDglBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) glXGetProcAddress((unsigned char*)"glBindFramebufferEXT");
		WGDglBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) glXGetProcAddress((unsigned char*)"glBindRenderbufferEXT");
		WGDglDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) glXGetProcAddress((unsigned char*)"glDeleteFramebuffersEXT");
		WGDglDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) glXGetProcAddress((unsigned char*)"glDeleteRenderbuffersEXT");
		WGDglFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) glXGetProcAddress((unsigned char*)"glFramebufferRenderbufferEXT");
		WGDglFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) glXGetProcAddress((unsigned char*)"glFramebufferTexture2DEXT");
		WGDglGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) glXGetProcAddress((unsigned char*)"glGenFramebuffersEXT");
		WGDglRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) glXGetProcAddress((unsigned char*)"glRenderbufferStorageEXT");
		WGDglCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) glXGetProcAddress((unsigned char*)"glCheckFramebufferStatusEXT");
		WGDglClampColorARB = (PFNGLCLAMPCOLORARBPROC) glXGetProcAddress((unsigned char*)"glClampColorARB");
	#endif
	#ifdef WIN32
		WGDglGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) wglGetProcAddress("glGenRenderbuffersEXT");
		WGDglBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) wglGetProcAddress("glBindFramebufferEXT");
		WGDglBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) wglGetProcAddress("glBindRenderbufferEXT");
		WGDglDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
		WGDglDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
		WGDglFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");
		WGDglFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
		WGDglGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) wglGetProcAddress("glGenFramebuffersEXT");
		WGDglRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) wglGetProcAddress("glRenderbufferStorageEXT");
		WGDglCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
		WGDglClampColorARB = (PFNGLCLAMPCOLORARBPROC) wglGetProcAddress("glClampColorARB");
	
	#endif
}

void Extensions::finalise() {

}

bool Extensions::isinstring(char *str, const char *search) {
	return false;
}
