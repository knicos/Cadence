#ifndef _WGD_EXT_
#define _WGD_EXT_

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#ifdef LINUX
#include <GL/glext.h>
#include <GL/glx.h>
#endif

#include <wgd/dll.h>

/*
 * Type definitions and function pointers for the OpenGl ARB
 * multitexturing extentions if the graphics card supports it.
 * These extensions are used by terrain mesh to add detail
 */
#ifdef WIN32
typedef void (APIENTRY *PFNGLACTIVETEXTUREARBPROC)(GLenum);
typedef void (APIENTRY *PFNGLMULTITEXCOORD2FARBPROC)(GLenum, GLfloat, GLfloat);
typedef void (APIENTRY *PFNGLCLIENTACTIVETEXTUREARBPROC)(GLenum);

typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEARBPROC) (GLenum target, GLint level, void *img);


#define GL_MAX_TEXTURE_UNITS              0x84E2
#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3
#define GL_TEXTURE4_ARB                   0x84C4
#define GL_TEXTURE5_ARB                   0x84C5
#define GL_TEXTURE6_ARB                   0x84C6
#define GL_TEXTURE7_ARB                   0x84C7
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_COMPRESSED_RGB_ARB             0x84ED
#define GL_COMPRESSED_RGBA_ARB            0x84EE
#endif

extern BASEIMPORT PFNGLACTIVETEXTUREARBPROC WGDglActiveTextureARB;
extern BASEIMPORT PFNGLMULTITEXCOORD2FARBPROC WGDglMultiTexCoord2fARB;
extern BASEIMPORT PFNGLCLIENTACTIVETEXTUREARBPROC WGDglClientActiveTextureARB;
//#endif
//#ifndef HAVE_GLCOMPRESSTEXTURE
extern BASEIMPORT PFNGLCOMPRESSEDTEXIMAGE3DARBPROC    WGDglCompressedTexImage3DARB;
extern BASEIMPORT PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    WGDglCompressedTexImage2DARB;
extern BASEIMPORT PFNGLCOMPRESSEDTEXIMAGE1DARBPROC    WGDglCompressedTexImage1DARB;
extern BASEIMPORT PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC WGDglCompressedTexSubImage3DARB;
extern BASEIMPORT PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC WGDglCompressedTexSubImage2DARB;
extern BASEIMPORT PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC WGDglCompressedTexSubImage1DARB;
extern BASEIMPORT PFNGLGETCOMPRESSEDTEXIMAGEARBPROC   WGDglGetCompressedTexImageARB;
//#endif


//stuff stolen from glew #######################################################
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_INFO_LOG_LENGTH 0x8B84

typedef char GLchar;

#ifdef WIN32
typedef void (APIENTRY * PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY * PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar* name);
typedef void (APIENTRY * PFNGLBLENDEQUATIONSEPARATEPROC) (GLenum, GLenum);
typedef void (APIENTRY * PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRY * PFNGLCREATEPROGRAMPROC) (void);
typedef GLuint (APIENTRY * PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRY * PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRY * PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint);
typedef void (APIENTRY * PFNGLDRAWBUFFERSPROC) (GLsizei n, const GLenum* bufs);
typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint);
typedef void (APIENTRY * PFNGLGETACTIVEATTRIBPROC) (GLuint program, GLuint index, GLsizei maxLength, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
typedef void (APIENTRY * PFNGLGETACTIVEUNIFORMPROC) (GLuint program, GLuint index, GLsizei maxLength, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
typedef void (APIENTRY * PFNGLGETATTACHEDSHADERSPROC) (GLuint program, GLsizei maxCount, GLsizei* count, GLuint* shaders);
typedef GLint (APIENTRY * PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const GLchar* name);
typedef void (APIENTRY * PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (APIENTRY * PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint* param);
typedef void (APIENTRY * PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (APIENTRY * PFNGLGETSHADERSOURCEPROC) (GLint obj, GLsizei maxLength, GLsizei* length, GLchar* source);
typedef void (APIENTRY * PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint* param);
typedef GLint (APIENTRY * PFNGLGETUNIFORMLOCATIONPROC) (GLint programObj, const GLchar* name);
typedef void (APIENTRY * PFNGLGETUNIFORMFVPROC) (GLuint program, GLint location, GLfloat* params);
typedef void (APIENTRY * PFNGLGETUNIFORMIVPROC) (GLuint program, GLint location, GLint* params);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERVPROC) (GLuint, GLenum, GLvoid*);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBDVPROC) (GLuint, GLenum, GLdouble*);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBFVPROC) (GLuint, GLenum, GLfloat*);
typedef void (APIENTRY * PFNGLGETVERTEXATTRIBIVPROC) (GLuint, GLenum, GLint*);
typedef GLboolean (APIENTRY * PFNGLISPROGRAMPROC) (GLuint program);
typedef GLboolean (APIENTRY * PFNGLISSHADERPROC) (GLuint shader);
typedef void (APIENTRY * PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);
typedef void (APIENTRY * PFNGLSTENCILFUNCSEPARATEPROC) (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
typedef void (APIENTRY * PFNGLSTENCILMASKSEPARATEPROC) (GLenum, GLuint);
typedef void (APIENTRY * PFNGLSTENCILOPSEPARATEPROC) (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void (APIENTRY * PFNGLUNIFORM1FPROC) (GLint location, GLfloat v0);
typedef void (APIENTRY * PFNGLUNIFORM1FVPROC) (GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRY * PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef void (APIENTRY * PFNGLUNIFORM1IVPROC) (GLint location, GLsizei count, const GLint* value);
typedef void (APIENTRY * PFNGLUNIFORM2FPROC) (GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRY * PFNGLUNIFORM2FVPROC) (GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRY * PFNGLUNIFORM2IPROC) (GLint location, GLint v0, GLint v1);
typedef void (APIENTRY * PFNGLUNIFORM2IVPROC) (GLint location, GLsizei count, const GLint* value);
typedef void (APIENTRY * PFNGLUNIFORM3FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRY * PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRY * PFNGLUNIFORM3IPROC) (GLint location, GLint v0, GLint v1, GLint v2);
typedef void (APIENTRY * PFNGLUNIFORM3IVPROC) (GLint location, GLsizei count, const GLint* value);
typedef void (APIENTRY * PFNGLUNIFORM4FPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRY * PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRY * PFNGLUNIFORM4IPROC) (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (APIENTRY * PFNGLUNIFORM4IVPROC) (GLint location, GLsizei count, const GLint* value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX2FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX3FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (APIENTRY * PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLVALIDATEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1DPROC) (GLuint index, GLdouble x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1DVPROC) (GLuint index, const GLdouble* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FPROC) (GLuint index, GLfloat x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1FVPROC) (GLuint index, const GLfloat* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1SPROC) (GLuint index, GLshort x);
typedef void (APIENTRY * PFNGLVERTEXATTRIB1SVPROC) (GLuint index, const GLshort* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2DPROC) (GLuint index, GLdouble x, GLdouble y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2DVPROC) (GLuint index, const GLdouble* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FPROC) (GLuint index, GLfloat x, GLfloat y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2FVPROC) (GLuint index, const GLfloat* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2SPROC) (GLuint index, GLshort x, GLshort y);
typedef void (APIENTRY * PFNGLVERTEXATTRIB2SVPROC) (GLuint index, const GLshort* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3DVPROC) (GLuint index, const GLdouble* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3FVPROC) (GLuint index, const GLfloat* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3SPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (APIENTRY * PFNGLVERTEXATTRIB3SVPROC) (GLuint index, const GLshort* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NBVPROC) (GLuint index, const GLbyte* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NIVPROC) (GLuint index, const GLint* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NSVPROC) (GLuint index, const GLshort* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUBPROC) (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUBVPROC) (GLuint index, const GLubyte* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUIVPROC) (GLuint index, const GLuint* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4NUSVPROC) (GLuint index, const GLushort* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4BVPROC) (GLuint index, const GLbyte* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4DPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4DVPROC) (GLuint index, const GLdouble* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4FVPROC) (GLuint index, const GLfloat* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4IVPROC) (GLuint index, const GLint* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4SPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4SVPROC) (GLuint index, const GLshort* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4UBVPROC) (GLuint index, const GLubyte* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4UIVPROC) (GLuint index, const GLuint* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIB4USVPROC) (GLuint index, const GLushort* v);
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
#endif

extern BASEIMPORT PFNGLCREATESHADERPROC WGDglCreateShader;
extern BASEIMPORT PFNGLSHADERSOURCEPROC WGDglShaderSource;
extern BASEIMPORT PFNGLCOMPILESHADERPROC WGDglCompileShader;

extern BASEIMPORT PFNGLCREATEPROGRAMPROC WGDglCreateProgram;
extern BASEIMPORT PFNGLATTACHSHADERPROC WGDglAttachShader;
extern BASEIMPORT PFNGLLINKPROGRAMPROC WGDglLinkProgram;

extern BASEIMPORT PFNGLGETPROGRAMIVPROC WGDglGetProgramiv;
extern BASEIMPORT PFNGLGETSHADERIVPROC WGDglGetShaderiv;
extern BASEIMPORT PFNGLGETPROGRAMINFOLOGPROC WGDglGetProgramInfoLog;
extern BASEIMPORT PFNGLGETSHADERINFOLOGPROC WGDglGetShaderInfoLog;

extern BASEIMPORT PFNGLISPROGRAMPROC WGDglIsProgram;

extern BASEIMPORT PFNGLGETUNIFORMLOCATIONPROC WGDglGetUniformLocation;
extern BASEIMPORT PFNGLGETATTRIBLOCATIONPROC WGDglGetAttribLocation;

extern BASEIMPORT PFNGLUSEPROGRAMPROC WGDglUseProgram;

extern BASEIMPORT PFNGLUNIFORM1FVPROC WGDglUniform1fv;
extern BASEIMPORT PFNGLUNIFORM1FPROC WGDglUniform1f;
extern BASEIMPORT PFNGLUNIFORM2FPROC WGDglUniform2f;
extern BASEIMPORT PFNGLUNIFORM3FPROC WGDglUniform3f;
extern BASEIMPORT PFNGLUNIFORM4FPROC WGDglUniform4f;

extern BASEIMPORT PFNGLUNIFORM1IVPROC WGDglUniform1iv;
extern BASEIMPORT PFNGLUNIFORM1IPROC WGDglUniform1i;

extern BASEIMPORT PFNGLVERTEXATTRIB1FVPROC WGDglVertexAttrib1fv;
extern BASEIMPORT PFNGLVERTEXATTRIB1FPROC WGDglVertexAttrib1f;
extern BASEIMPORT PFNGLVERTEXATTRIB2FPROC WGDglVertexAttrib2f;
extern BASEIMPORT PFNGLVERTEXATTRIB3FPROC WGDglVertexAttrib3f;
extern BASEIMPORT PFNGLVERTEXATTRIB4FPROC WGDglVertexAttrib4f;
extern BASEIMPORT PFNGLBINDATTRIBLOCATIONPROC WGDglBindAttribLocation;

extern BASEIMPORT PFNGLENABLEVERTEXATTRIBARRAYPROC WGDglEnableVertexAttribArray;
extern BASEIMPORT PFNGLDISABLEVERTEXATTRIBARRAYPROC WGDglDisableVertexAttribArray;
extern BASEIMPORT PFNGLVERTEXATTRIBPOINTERPROC WGDglVertexAttribPointer;

extern BASEIMPORT PFNGLDELETESHADERPROC WGDglDeleteShader;
extern BASEIMPORT PFNGLDELETEPROGRAMPROC WGDglDeleteProgram;

//##############################################################################

#ifdef WIN32

typedef void (APIENTRY * PFNGLPOINTPARAMETERFVPROC) (GLenum pname, GLfloat *params);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFPROC) (GLenum pname, GLfloat param);

#define GL_ALIASED_POINT_SIZE_RANGE 0x846d
#define GL_POINT_SPRITE 0x8861
#define GL_COORD_REPLACE 0x8861
#define GL_POINT_DISTANCE_ATTENUATION 0x8129

#endif

extern BASEIMPORT PFNGLPOINTPARAMETERFVPROC WGDglPointParameterfv;
extern BASEIMPORT PFNGLPOINTPARAMETERFPROC WGDglPointParameterf;

//VBO
#ifdef WIN32
#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);
#endif

extern BASEIMPORT PFNGLGENBUFFERSARBPROC WGDglGenBuffersARB;	// VBO Name Generation Procedure
extern BASEIMPORT PFNGLBINDBUFFERARBPROC WGDglBindBufferARB;	// VBO Bind Procedure
extern BASEIMPORT PFNGLBUFFERDATAARBPROC WGDglBufferDataARB;	// VBO Data Loading Procedure
extern BASEIMPORT PFNGLDELETEBUFFERSARBPROC WGDglDeleteBuffersARB;	// VBO Deletion Procedure


//FBO + RenderBuffer
#ifdef WIN32
#define GL_FRAMEBUFFER_EXT 0x8D40
#define GL_RENDERBUFFER_EXT 0x8D41
#define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
#define GL_DEPTH_ATTACHMENT_EXT 0x8D00
#define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5

#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_DEPTH_COMPONENT32 0x81A7

#define GL_RGBA16F_ARB 0x881A
#define GL_HALF_FLOAT_ARB 0x140B

#define GL_CLAMP_VERTEX_COLOR_ARB 0x891A
#define GL_CLAMP_FRAGMENT_COLOR_ARB 0x891B
#define GL_FIXED_ONLY_ARB 0x891D

typedef void (APIENTRY * PFNGLGENRENDERBUFFERSEXTPROC) (GLsizei n, GLuint* renderbuffers);
typedef void (APIENTRY * PFNGLBINDFRAMEBUFFEREXTPROC) (GLenum target, GLuint framebuffer);
typedef void (APIENTRY * PFNGLBINDRENDERBUFFEREXTPROC) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRY * PFNGLDELETEFRAMEBUFFERSEXTPROC) (GLsizei n, const GLuint* framebuffers);
typedef void (APIENTRY * PFNGLDELETERENDERBUFFERSEXTPROC) (GLsizei n, const GLuint* renderbuffers);
typedef void (APIENTRY * PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRY * PFNGLGENFRAMEBUFFERSEXTPROC) (GLsizei n, GLuint* framebuffers);
typedef void (APIENTRY * PFNGLRENDERBUFFERSTORAGEEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef GLenum (APIENTRY * PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) (GLenum target);
typedef void (APIENTRY * PFNGLCLAMPCOLORARBPROC) (GLenum target, GLenum clamp);
#endif

extern BASEIMPORT PFNGLGENRENDERBUFFERSEXTPROC WGDglGenRenderbuffersEXT;
extern BASEIMPORT PFNGLBINDFRAMEBUFFEREXTPROC WGDglBindFramebufferEXT;
extern BASEIMPORT PFNGLBINDRENDERBUFFEREXTPROC WGDglBindRenderbufferEXT;
extern BASEIMPORT PFNGLDELETEFRAMEBUFFERSEXTPROC WGDglDeleteFramebuffersEXT;
extern BASEIMPORT PFNGLDELETERENDERBUFFERSEXTPROC WGDglDeleteRenderbuffersEXT;
extern BASEIMPORT PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC WGDglFramebufferRenderbufferEXT;
extern BASEIMPORT PFNGLFRAMEBUFFERTEXTURE2DEXTPROC WGDglFramebufferTexture2DEXT;
extern BASEIMPORT PFNGLGENFRAMEBUFFERSEXTPROC WGDglGenFramebuffersEXT;
extern BASEIMPORT PFNGLRENDERBUFFERSTORAGEEXTPROC WGDglRenderbufferStorageEXT;
extern BASEIMPORT PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC WGDglCheckFramebufferStatusEXT;
extern BASEIMPORT PFNGLCLAMPCOLORARBPROC WGDglClampColorARB;

namespace wgd {

	/**
	 * Detects available OpenGL and processor extensions.
	 */
	class BASEIMPORT Extensions {

		public:
		/**
		 * Do we have multi-texture support.
		 * @return True if we have multi-texture support.
		 */
		static bool hasMultiTexture() { return s_multitex; };
		/**
		 * Can OpenGL use compressed textures.
		 * @return True if DXF texture compression is supported.
		 */
		static bool hasTextureCompression() { return s_texcompress; };

		/**
		 * Is antialiasing possible with hardware.
		 */
		static bool hasMultisample() { return s_multisample; };

		/**
		 * Are vertex and fragment shaders available.
		 * @return True if both vertex and fragment shaders exist.
		 */
		static bool hasShaders() { return s_shaders; };

		/**
		 * Do we have Vertex Buffer Objects.
		 */
		static bool hasVBO() { return s_vbo; };

		static bool hasMMX() { return s_mmx; };
		/**
		 * Is this processor SSE capable.
		 * @return True if we have SSE SIMD instructions.
		 */
		static bool hasSSE() { return s_sse; };

		static bool hasFBO() { return s_framebuf; };
		static bool hasHalfFloat() { return s_halffloat; };
		static bool hasColourClamp() { return s_colorbuf; };

		static void initialise();
		static void finalise();

		private:
		static bool isinstring(char *, const char *);

		struct SCpuidRegs
		{
			int eax;
			int ebx;
			int ecx;
			int edx;
		};

		static void getCpuid(int num, SCpuidRegs &regs)
		{
			//Use _asm in VC++
			
			#ifndef WINVC
			//asm (	"cpuid"
			//	: "=a" (regs.eax),
			//	"=b" (regs.ebx),
			//	"=c" (regs.ecx),
			//	"=d" (regs.edx)
			//	: "a" (num));
			#else
			#endif
		}

		static bool s_multitex;
		static bool s_texcompress;
		static bool s_shaders;
		static bool s_mmx;
		static bool s_sse;
		static bool s_framebuf;
		static bool s_halffloat;
		static bool s_multisample;
		static bool s_vbo;
		static bool s_colorbuf;
	};
};

#endif
