/*
 * WGD Library
 *
 * Authors: 
 * Date: 17/9/2007
 *
 */

#ifndef _WGD_SHADER_
#define _WGD_SHADER_

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <map>
#include <cadence/doste/oid.h>
#include <cadence/dstring.h>
#include <cadence/agent.h>
#include <cadence/file.h>
#include <string>
#include <wgd/index.h>


namespace wgd {

	class vector3d;
	class Texture;
	
	/**
	 * Shader Resource. This contains everything a shader needs to run,
	 * you apply the shader to an object the same way you would apply a texture
	 * with bind() and unbind(). The shader replaces normal textures so if
	 * you use a shader, you dont bind textures directly to the object.<br>
	 * You must specify both vertex shader and fragment shader source, as well as
	 * all the textures that the shader uses. <br>
	 * Shaders that need Binormals and Tangents will automatically get the tangent
	 * if applied to models and primitives, bot you must calculate the binormal
	 * in your vertex shader using: cross(gl_Normal, tangent); <br>
	 * The tangent varying variable must be named "tangent" for it to work.<br>
	 * If a shader program fails to compile, or will not run on your machine
	 * nothing will happen when you try to bind it.
	 * <br/><br/>
	 * 
	 */
	class RESIMPORT Shader : public cadence::Agent {

		public:

		OBJECT(Agent, Shader);
		
		/** Create an empty shader object */
		Shader();
		/** Construct a shader from database - used by the library */
		Shader(const cadence::doste::OID &);
		/** Create a shader, specifying the source files to use as File objects */
		Shader(cadence::File &vert, cadence::File &frag);
		/** Create a shader, specifying the source files to use */
		Shader(const char* vertfile, const char* fragfile);
		~Shader();
		/**
		 * Vertex shader source filename.  You must set both vertex and fragment
		 * shader filenames before loading or using the shader.
		 * @param v The filename string
		 */
		PROPERTY_WF(cadence::File, vert, ix::vert);
		
		/**
		 * Current vertex shader filename.
		 * @return String representing the filename.
		 */		
		PROPERTY_RF(cadence::File, vert, ix::vert);
		
		/**
		 * Fragment shader source filename.  You must set both vertex and fragment
		 * shader filenames before loading or using the shader.
		 * @param v The filename string
		 */		
		PROPERTY_WF(cadence::File, frag, ix::frag);
		/**
		 * Current fragment shader filename.
		 * @return String representing the filename.
		 */		
		PROPERTY_RF(cadence::File, frag, ix::frag);

		PROPERTY_WF(bool, debug, ix::debug);
		PROPERTY_RF(bool, debug, ix::debug);
		
		/**
		 * Build a shader program from string arrays instead of files.
		 * @return Did it work...
		 */
		bool make(const char *vert, const char *frag);
		
		/**
		 * Force load and compile the shader. If not called then the shader will not be loaded or compiled
		 * until bind() is first called. 
		 */		
		bool load();
		
		/**
		 * Use this shader.
		 */		
		void bind();
		/**
		 * Stop using the shader. 
		 * You need to call this when you have finished drawing with the shader.
		 */		
		void unbind();
		
		/**
		 * Get the currently bound shader. Returns Null if no shaders are bound.
		 * @return Current shader.
		 */		
		static Shader *current();

		static void current(Shader *sh);
		
		/**
		 * This shows whether the shader uses a 'tangent' varying variable.
		 * used by IModel to derermine whether to try to write to it.
		 * @return 'tangent' variable used.
		 */		
		bool tangents(){ return m_tangents; };
		
		/**
		 * Set a float variable in the shader. The function will determine it it
		 * is uniform or varying.
		 * @param name the variable to write to
		 * @param v1 The value to set
		 */
		void setVariable(const char *name, float v1);
		/**
		 * Set a vec2 variable in the shader.
		 * @param name the variable to write to
		 * @param v1 The 'x' part of the vector
		 * @param v2 The 'y' part of the vector
		 */		
		void setVariable(const char *name, float v1, float v2);
		/**
		 * Set a vec3 variable in the shader.
		 * @param name the variable to write to
		 * @param v1 The 'x' part of the vector
		 * @param v2 The 'y' part of the vector
		 * @param v3 The 'z' part of the vector
		 */		
		void setVariable(const char *name, float v1, float v2, float v3);
		/**
		 * Set a vec4 variable in the shader.
		 * @param name the variable to write to
		 * @param v1 The 'x' part of the vector
		 * @param v2 The 'y' part of the vector
		 * @param v3 The 'z' part of the vector
		 * @param v4 The 'w' part of the vector
		 */		
		void setVariable(const char *name, float v1, float v2, float v3, float v4);
		/**
		 * Set a vec3 variable in the shader.
		 * @param name the variable to write to
		 * @param vec3 The vector to set
		 */			
		void setVariable(const char *name, const wgd::vector3d &vec3);
		
		
		/**
		 * Set an integer variable in the shader.
		 * @param name the variable to write to
		 * @param v1 The integer value to set
		 */		
		void setVariable(const char *name, int v1);
		
		/**
		 * Set an integer variable array in the shader
		 * Note variable name must be postfixed with [0] to write to the start of the array
		 * @param name the variable to write to
		 * @param size the number of elements in the array
		 * @param data pointer to the array
		 */
		 void setVariable(const char *name, int size, int* data);
		 
		/**
		 * Set an float variable array in the shader
		 * Note variable name must be postfixed with [0] to write to the start of the array
		 * @param name the variable to write to
		 * @param size the number of elements in the array
		 * @param data pointer to the array
		 */		 
		 void setVariable(const char *name, int size, float* data);
		 
		
		/**
		 * Enable using an attribute variable in a vertex array
		 * similar to glEnableClientState();
		 * Must call disableAttribArray() when done.
		 * @param name the name of the varying variable to set
		 */		
		void enableAttribArray(const char *name);
		/**
		 * Set a pointer to an attribute variable array
		 * when drawing from vertex arrays.
		 * similar to glVertexPointer()
		 * @param name the name of the varying variable to be modified.
		 * @param size Specifies the number of values for each element of the vertex attribute array. Must be 1, 2, 3, or 4.
		 * @param type Specifies the data type of each component in the array. Symbolic constants GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, and GL_DOUBLE are accepted.
		 * @param normalised Specifies whether fixed-point data values should be normalized (GL_TRUE ) or converted directly as fixed-point values (GL_FALSE ) when they are accessed.
		 * @param Specifies the byte offset between consecutive attribute values. If stride is 0, the attribute values are understood to be tightly packed in the array.
		 * @param pointer Specifies a pointer to the first component of the first attribute value in the array.
		 */		
		void attribPointer(const char *name, GLint size, GLenum type, GLboolean normalised, GLsizei stride, const void *pointer);
		/**
		 * Disable using an attribute variable in a vertex array
		 * similar to glDisableClientState();
		 * @param name the name of the varying variable to that was set
		 */
		void disableAttribArray(const char *name);

		/**
		 * Set whether shaders are enabled
		 * This has no effect if shaders are not supported.
		 * @param b True to enable shaders, False to disable them.
		 */		
		static void enabled(bool);
		/**
		 * Get whether shaders are enabled by the user
		 * This does not return whether shaders are supported.
		 * @return enabled.
		 */		
		static bool enabled();

		BEGIN_EVENTS(Agent);
		EVENT(evt_reload, (*this)("reload"));
		END_EVENTS;
		
		private:
			
		static void initialise();
		static bool  s_available;
		static Shader *s_current;
		
		//get variable name
		GLint addVariable(const char *name);

		bool loadShader();
		char *readFile(cadence::File *);
		int logInfo(GLuint s, const char *name);
		
		GLuint m_vertexShader;
		GLuint m_fragmentShader;
		GLuint m_program;
		
		bool m_ready;
		bool m_loaded;
		
		//has a tangent attribute variable
		bool m_tangents;
			
		//maybe detect what kind of variable
		GLint getLocation(const char *name);
		
		class ShaderVar{
			public:
			ShaderVar(int t, GLint loc): type(t), location(loc){};
			int type; //1=uniform, 2=attribute
			GLint location;
		};
		
		ShaderVar *getVar(const char *name);
		cadence::doste::OID m_vars;
		
		
	};
};

#endif
