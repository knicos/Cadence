#ifndef _DRAWABLE_
#define _DRAWABLE_

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <wgd/extensions.h>

#include <wgd/vertex.h>
#include <wgd/matrix.h>
#include <wgd/shader.h>
#include <wgd/material.h>

namespace wgd {
	/** Draw flags for use with drawable items */
	enum DrawFlags { LIGHTING=1, DEPTH_TEST=2, DEPTH_WRITE=4, ALPHA_TEST=8, WIREFRAME=16, DOUBLE_SIDED=32 };
	class DrawableBase;
	//The curent openGL client state - for Drawable optimisation stuff
	//This will remember the client state and only change it when nessesary.
	class RESIMPORT DrawStatus {
		public:
		DrawStatus();
		void begin();	//Activate draw status control
		void end();		//stop draw status control
		void reset();	//set OpenGL states to default
		void setState(bool n, bool c, unsigned int tx);	//set client states
		void setSurface(const OID& surf);		//bind a surface
		void setFlags(unsigned char f);				//set draw flags to change OpenGL states
		
		bool active;		//Drawstatus is in use
		Shader* currentShader() { return shader; }
		private:
		
		unsigned char flags;	//Last flags active
		bool normals;		//normal array active
		bool colours;		//colour array active
		unsigned int texcoords;	//number of active texture arrays
		OID  surface;		//last used surface
		Material* material;	//currently bound material
		Texture *texture;	//currently bound texture
		Shader* shader;		//currently bound shader
		
		DrawableBase* last;	//The last drawable that was drawn (for deactivating shaders)
		//...
		
	};
	
	/**
	 *	Abstract Drawable base class. This contains the control data used to specify
	 *	how an item is drawn.
	 */
	class RESIMPORT DrawableBase {
		friend class DrawStatus;
		public:
		DrawableBase() : VBO(0), distance(0), mode(GL_TRIANGLES), discard(false), clearData(false), alpha(1.0f), flags(7), queued(0) {};
		virtual ~DrawableBase() {}
		
		matrix 	transform;		///< Final transformation m atrix of the mesh
		GLuint 	VBO;			///< Vertex buffer object containing vertex data, 0=none
		float 	distance;		///< Distance from camera, for alpha sorting
		GLenum	mode;			///< Drawing mode, GL_LINE, GL_TRIANGLES, etc..
		OID	surface;		///< Matrial or texture to use, if Null, leave as white. This takes an OID not a pointer so dont forget to dereference material/texture pointers.
		bool	discard;		///< Will delete object after drawing
		bool	clearData;		///< Deletes vertex data after use
		bool	useVBO;			///< Will build a VBO to use - only if discard = false
		float	alpha;			///< Drawable alpha - defaults to 1.0 - used to fade out objects that do not have a colour array
		
		unsigned char flags;		///< Draw flags =- was type Drawflags but it got anoying to cast everything
		
		int queued;			//Drawable is in the drawing queue
		
		/** Draw the item! */
		virtual void draw() = 0;
		
		/** delete vertex data */
		virtual void deleteData() = 0;
		
		/**	delete drawable object, this will wait until it has been drawn if in the drawing queue. 
		 *	Use this instead of delete as it will break if you call deleto on a drawable that is queued for drawing.
		 */
		void destroy() {
			if(queued) {
				discard = true; clearData = true;
			} else {
				deleteData();
				delete this;
			}
		}
		
		//begin and end drawstatus
		static void begin() { status.begin(); }
		static void end() { status.end(); }
		
		//Comparison for sorting, by distance, then by material
		static bool compare(DrawableBase* a, DrawableBase* b) {
			if(a->distance==b->distance) return a->surface < b->surface;
			return a->distance > b->distance;
		}
			
		protected:
		//Disable custom shader attribute variable client states
		virtual void disableCustom(wgd::Shader*) {};
		
		static DrawStatus status;
	};

	//Recursively enable, disable and set custom vertex data - A and B are each side of the tuple
	template <typename A, typename B>
	struct CustomVarRecurse {
		//Recursively enable shader variables
		static void enable(wgd::Shader *shader) {
			shader->enableAttribArray(A::name);
			CustomVarRecurse<typename B::type_A, typename B::type_B>::enable(shader);
		}
		
		//recursively disable them
		static void disable(wgd::Shader *shader) {
			shader->disableAttribArray(A::name);
			CustomVarRecurse<typename B::type_A, typename B::type_B>::disable(shader);
		}
		
		//Recursive template function which will loop through all custom parameters
		//and will set the vertex pointer for each one.
		static void setPointer(wgd::Shader *shader, const char* offset, int stride) {
			shader->attribPointer(A::name, A::size, A::type, true, stride, offset); //set data
			offset += sizeof(A);	//increment data offset
			CustomVarRecurse<typename B::type_A, typename B::type_B>::setPointer(shader, offset, stride);
		}
	};
	//Specialised recursive functions that are used to terminate the recursion - when 'next' does not exist
	template <typename A>
	struct CustomVarRecurse<A, null_type> {
		static void enable(wgd::Shader *shader) {
			shader->enableAttribArray(A::name);
		}
		static void disable(wgd::Shader *shader) {
			shader->disableAttribArray(A::name);
		}
		static void setPointer(wgd::Shader *shader, const char* offset, int stride) {
			shader->attribPointer(A::name, A::size, A::type, true, stride, offset);
			offset += sizeof(A);
		}
	};


	//Wrapper for calling the recursive functions to enable, disable and set custom shader variables
	//E is the type of the fist tuple in the extended vertex format.
	template <typename E>
	struct CustomVarWrap {
		//Call recursive enable function
		static void enable(wgd::Shader *shader) {
			CustomVarRecurse<typename E::type_A, typename E::type_B>::enable(shader);
		}
		//call recursive function to set vertex data pointers
		static void setPointer(wgd::Shader *shader, const char* offset, int stride) {
			CustomVarRecurse<typename E::type_A, typename E::type_B>::setPointer(shader, offset, stride);
		}
		//call recursive disable function
		static void disable(wgd::Shader *shader) {
			CustomVarRecurse<typename E::type_A, typename E::type_B>::disable(shader);

		}
	};
	
	//Specialised wrapper for when there are no custom shader variables
	template <>
	struct CustomVarWrap<null_type> {
		static void enable(wgd::Shader *shader) {}
		static void setPointer(wgd::Shader *shader, const char* offset, int stride) {}
		static void disable(wgd::Shader *shader) {}
	};
	
	
	/**
	 *	A Drawable item is a mesh consisting of vertices, materials and the 
	 *	final transformation matrix needed to render the item.
	 *	Drawable items are alpha-sorted and material sorted.
	 *	May need other flags such as Lighting, depth testing, depth masking etc. <br>
	 *	The template parameter is the custom vertex type used by this item.
	 */
	template <typename T>
	class Drawable : public DrawableBase {
		public:
		Drawable() : data(0), size(0), indices(0), nIndices(0), sData(0), sIndices(0), dataPointer(0), indicesPointer(0) {};
		
		T* data;	//vertex data
		unsigned int	size;	//How many vertices
		unsigned char* indices; //Indices
		unsigned int nIndices;	//number of indices
		
		/**
		 *	Draw the item
		 */
		void draw();
		
		/** Add stuff to drawable like a vector */
		T& add();				//Add a blank vertex, return it so the user can give it data
		void add(const T&);		//Add a vertex
		void add(unsigned char);//Add an index
		void add(Drawable<T> *);//Add data from another drawable
		
		/** delete vertex data */
		void deleteData() { 
			if(data) delete [] data; data=0; size=0;  
			if(indices) delete [] indices; indices=0; nIndices=0;
		};
		
		protected:
		/// Set data pointers to vertex array;
		void bindArray(T* data);
		
		/// set data pointers to VBO
		void bindVBO(GLuint vbo);
		
		//enable custom vertex arrays (for shaders)
		void enableCustom(wgd::Shader* shader) { CustomVarWrap<typename T::type_custom>::enable(shader); }
		
		private:
		
		//array sizes for adding stuff
		void resizeData(unsigned int s);
		void resizeIndices(unsigned int s);
		unsigned int sData;
		unsigned int sIndices;
		T* dataPointer; //sdata will be invalid if the pointer changes
		unsigned char* indicesPointer; //same with indices
		
		void setPointers(const char* p);
		void setCustomPointer(wgd::Shader* shader, const char* offset, int stride) {CustomVarWrap<typename T::type_custom>::setPointer(shader, offset, stride); }
		void disableCustom(wgd::Shader* shader) { CustomVarWrap<typename T::type_custom>::disable(shader); }
	};
};

template <typename T>
void wgd::Drawable<T>::setPointers(const char* p) {
	int stride = sizeof(T);
	glVertexPointer(T::nv, GL_FLOAT, stride, p);
	p += T::nv * sizeof(float);

	//set normal pointer (if normals are used)
	if(T::nn==3) {
		glNormalPointer(GL_FLOAT, stride, p);
		p += T::nn * sizeof(float);
	}
	
	//set texture coordinate pointers
	int i;
	for(i = 0; i<T::tc; i++) {
		if(i>0) WGDglClientActiveTextureARB(GL_TEXTURE0_ARB+i);
		glTexCoordPointer(2, GL_FLOAT, stride, p);
		p += 2 * sizeof(float);
	}
	if(i) WGDglClientActiveTextureARB(GL_TEXTURE0_ARB);
	
	//set colour pointer
	if(T::nc!=0) {
		glColorPointer(T::nc, GL_FLOAT, stride, p);
		p += T::nc * sizeof(float);
	}
	
	if(status.currentShader()) setCustomPointer(status.currentShader(), p, stride);
}

template <typename T>
void wgd::Drawable<T>::bindArray(T* data) {
	setPointers((char*)data);
}

template <typename T>
void wgd::Drawable<T>::bindVBO(GLuint vbo) {
	WGDglBindBufferARB( GL_ARRAY_BUFFER_ARB, vbo );
	setPointers(0);
	WGDglBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
}


template <typename T>
void wgd::Drawable<T>::draw() {

	//transform matrix
	transform.apply();
	
	//Enable client states
	if(!status.active) glEnableClientState(GL_VERTEX_ARRAY);
	status.setState(T::nn, T::nc, T::tc);
	status.setFlags(flags);
	status.setSurface(surface);
	
	//drawable alpha
	if(alpha!=1.0f && T::nc==0) {
		if(flags & LIGHTING) {
			GLfloat mat[4] = { 1.0f, 1.0f, 1.0f, alpha };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
		} else glColor4f(1.0f, 1.0f, 1.0f, alpha);
	}
	
	//shader has to be done here
	if(status.currentShader()) enableCustom(status.currentShader());
	
	if(VBO) bindVBO(VBO);
	else bindArray(data);
	
	if(nIndices==0) {
		glDrawArrays(mode, 0, size);
	} else {
		glDrawElements(mode, nIndices, GL_UNSIGNED_BYTE, indices);
	}
	 
	transform.unapply();
	
	if(!status.active) status.end();
	
	//may need to reset active colour if it has changed
	if(T::nc>0 || alpha!=1.0f) {
		GLfloat mat[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

/** Add stuff to drawable like a vector */
template <typename T>
void wgd::Drawable<T>::resizeData(unsigned int s) {
	if(dataPointer!=data) sData = size; //validate pointer
	
	if(s<=sData) return;	//nothing to do if s is too small
	if(s < sData + 4) s = sData + 4; //always add at least 4 vertices
	if(sData==0) {
		deleteData();
		data = new T[s];
	} else {
		T* tmp = new T[s];
		memcpy(tmp, data, size * sizeof(T));
		delete [] data;
		data = tmp;
	}
	sData = s;
	dataPointer=data;
}

template <typename T>
void wgd::Drawable<T>::resizeIndices(unsigned int s) {
	if(indicesPointer!=indices) sIndices = nIndices; //validate pointer
	
	//always add at least 4 indices
	if(s < nIndices + 4) s = nIndices + 4;
	
	if(s<=sIndices) return;	//nothing to do if s is too small
	if(sIndices==0) {
		indices = new unsigned char[s];
	} else {
		unsigned char* tmp = new unsigned char[s];
		memcpy(tmp, indices, size*sizeof(unsigned char));
		delete [] indices;
		indices = tmp;
	}
	sIndices = s;
	indicesPointer=indices;
}

template <typename T>
T& wgd::Drawable<T>::add() {
	resizeData(size+1);
	return data[size++];
}

template <typename T>
void wgd::Drawable<T>::add(const T& vx) {
	resizeData(size+1);
	memcpy(&data[size], &vx, sizeof(T));
	size++;
}

template <typename T>
void wgd::Drawable<T>::add(unsigned char i) {
	if(indicesPointer!=indices) { sIndices = nIndices; indicesPointer=indices; }
	if(sIndices == nIndices) resizeIndices(nIndices+4);
	indices[size++] = i;
}

template <typename T>
void wgd::Drawable<T>::add(Drawable<T> *d) {
	//this one is slightly more complex
	//also needs to transform everything by the matrices
	//to use homogeneous coordinates - need to transform vertices and normals 
	
	
	unsigned int offset = size;
	resizeData(size + d->size);
	memcpy(&data[size], d->data, d->size * sizeof(T));
	size += d->size;
	
	//copy indices if they exist and adjust for offset.
	if(d->nIndices) {
		resizeIndices(nIndices + d->nIndices);
		for(unsigned int i=0; i<d->nIndices; i++) indices[nIndices+i] = d->indices[i] + offset;
		nIndices += d->nIndices;
	}
}


#endif
