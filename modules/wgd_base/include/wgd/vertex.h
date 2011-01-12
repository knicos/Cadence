#ifndef _WGD_VERTEX_
#define _WGD_VERTEX_

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <wgd/vector.h>
#include <wgd/colour.h>

namespace wgd {
	#ifdef _MSC_VER
	#pragma pack(push)
	#pragma pack(1)
	#endif
	struct vertex{				
		float x;	//Position
		float y;	
		float z;	
		float nx;	//Normal
		float ny;
		float nz;		
		float u;	//Texture coords
		float v;
		float tx;	//tangent
		float ty;
		float tz;
	#ifdef _MSC_VER
	};
	#else	
	} __attribute__((packed));
	#endif
	
	
	/**	Null structure. Used to fill in
	 *	the non-existant patrs of the vertex structure.
	 */
	struct null_type { /*null_type(){}*/ };
	
	/**
	 *	Test to see if a structure is null_type, 
	 *	use: bool isNullType<T>::type;
	 */
	template <typename T> class is_nulltype  : public cadence::doste::false_type {};
	template <> class is_nulltype<null_type> : public cadence::doste::true_type {};
	
	/**
	 *	Defauld vector selector class used by custom_vertex.
	 *	Vertors with 2, 3 and 4 elements are specialised, anything else
	 *	becomes the null type. <br>
	 *	Template <NV> is the number of elements in the
	 */
	template <int NV>
	struct vector_selector {
		typedef null_type type;
	};
	
	/**
	 *	Specialised vector with four elements
	 *	Can either use x,y,z,w or r,g,b,a to access the elements
	 */
	template <>
	struct vector_selector<4> {
		union {
			struct { float x,y,z,w; } PACKED;
			struct { float r,g,b,a; } PACKED;
		};
		void operator=(const vector3d &v) { x=v.x; y=v.y; z=v.z; w=1.0f; }
		void operator=(const colour &c) { r=c.r; g=c.g; b=c.b; a=c.a; }
	};	

	/**
	 *	Specialised vector with three elements
	 *	Can either use x,y,z or r,g,b to access the elements
	 */
	template <>
	struct vector_selector<3> {
		union {
			struct { float x,y,z; } PACKED;
			struct { float r,g,b; } PACKED;
		};
		void operator=(const vector3d &v) { x=v.x; y=v.y; z=v.z; }
		void operator=(const colour &c) { r=c.r; g=c.g; b=c.b; }
	};

	/**
	 *	Specialised vector with two elements
	 *	Can either use x,y or u,v or s,t to access the elements
	 */
	template <>
	struct vector_selector<2> {
		union {
			struct { float x,y; } PACKED;
			struct { float u,v; } PACKED;
			struct { float s,t; } PACKED;
		}; 
		void operator=(const vector2d &v) { x=v.x; y=v.y; }
	};
	
	/**
	 *	Optional structure for texture coordinates consisting
	 *	of an array of two element vectors.
	 *	Access these elements with x,y or u,v or s,t.<br>
	 *	The array is of length TC.<br>
	 */
	template <int TC>
	struct texcoord_selector {
		typedef struct {
			union {
				struct { float x,y; } PACKED;
				struct { float u,v; } PACKED;
				struct { float s,t; } PACKED;
			};
			void operator=(const vector2d &v) { x=v.x; y=v.y; }
		} PACKED type[TC];
	};
	/**
	 *	Specialised texture coordinate structure when there are no
	 *	coordinates. This becomes null_type.
	 */
	template <>
	struct texcoord_selector<0> {
		typedef null_type type;
	};
	
	//Crazy custom vertex stuff
	#ifdef _MSC_VER
	#pragma pack(1)
	#endif
	/**
	 *	Custom vertex template. Use this class to create your own custom vertex structures. <br>
	 *	The template paramerters are used to define what data the structure contains.<br>
	 *	NV - Number of elements for the vertex position, Must be 2,3 or 4. <br>
	 *	NN - Number of elements for the vertex normal. can be 0, or 3. zero is used when there are no normals.<br>
	 *	TC - Number of Texture coordinate sets. Each set contains two elements. Generally use 0-8. <br>
	 *	NC - Number of vertex colour elements, can be 0, 3 or 4, corresponding to r,g,b,a. <br>
	 *	T  - Custom vertex data for use with shaders. This must be type_null if there is no extra data
	 *	to be added to the vertex. If there are, then T must be a tuple containing a custom structure
	 *	and the next custom format or type_null. <br>
	 *	Use typedef to make it easier to use a custom structure.
	 *	Example:<br>
	 *	<code>
	 *	//Create a custom vertex 'myVertex' containing 3 position elements, 3 normal elements and a set of texture coordinates:<br>
	 *	typedef vertex_template<3, 3, 1, 0, null_type> myVertex;
	 *	</code>
	 *	To use extra custom variables:
	 *	<code>
	 *	//Create a custom vertex structure 'blah' containing 3 position elements, and a set of per vertex shader attribute variables:<br>
	 *	typedef vertex_template<3, 0, 0, 0, tuple< customClass, null_type> > blah;
	 *	</code>
	 *	The customClass structure must be a packed structure consisting of the variables to send.
	 *	All the variables in the structure MUST be of the same type as they are sent to the shader as an array.
	 *	It also needs to contain static variables to define the structure: <br>
	 *	<code>
	 *		static const int size = x; //where x is the number of variables in the structure<br>
	 *		static const GLenum type = x; //where x is the type of the variables, GL_FLOAT, GL_INT, etc.
	 *		static char* name;	//Name is the name of the shader variable we are writing to. it must be set in a cpp file somewhere.
	 *	</code>
	 *	For an example of this, see vertex_tangent structure below.
	 */
	template <int NV, int NN, int TC, int NC, typename T> 
	struct vertex_template {
		vertex_template() {};

		union {
			typename wgd::vector_selector<NV> position;
			struct { char _d1[sizeof(float)*NV - (!NN? 1:0)];			typename wgd::vector_selector<NN> normal; };
			struct { char _d2[sizeof(float)*(NV+NN) - (!TC? 1:0)];		typename wgd::texcoord_selector<TC>::type texcoords; };
			struct { char _d3[sizeof(float)*(NV+NN+TC*2) - (!NC? 1:0)]; typename wgd::vector_selector<NC> colour; };
			
			struct { char _d4[sizeof(float)*(NV+NN+TC*2+NC) - (is_nulltype<T>::value? 1:0)]; T custom; };
		};


		typedef T type_custom;
		
		static const int nv = NV;
		static const int nn = NN;
		static const int nc = NC;
		static const int tc = TC;
	} PACKED;
	
	/**
	 *	Tuple structure - used to add extra structures to a custom vertex structure.
	 *	This data is used only for shader vertex attribute variables.<br>
	 *	template A is the type of the custom variable structure<br>
	 *	template B is either another tuple, or type_null specifting the end of a chain.
	 *	You can chain as many custom classes as you like using nested tuples.<br>
	 *	Example:
	 *	<code>
	 *	//create a chain containing two custom classes:<br>
	 *	tuple< myClass1, tuple< myclass2, null_type > >;
	 *	</code>
	 *	This is used fot the T template parameter in custom_vertex.
	 */
	template <typename A, typename B>
	struct tuple {
		A var;
		B next;
		typedef A type_A;
		typedef B type_B;
	} PACKED;
	
	/**
	 *	Tuple specialisation for the terminating tuple in a chain.
	 *	This version does not have the 'next' element. It occurs when 
	 *	the second template parameter is null_type.
	 */
	template <typename A>
	struct tuple<A, null_type> {
		A var;
		typedef A type_A;
		typedef null_type type_B;
	} PACKED;
	
	/**
	 *	Vertex tangents are a commonly used shader attribute variable, 
	 *	This is an extension for the custom_vertex class use with the tuples.
	 *	It contains three floats, and the information the shader needs to apply them
	 *	as static elements. <br>
	 *	size must be number of elements in the structure, <br>
	 *	type must be the data type used for the elements, <br>
	 *	name must be the variable name used in the shader that these values correspond to. <br>
	 *	All data elements MUST be of the same type in a structure.
	 */
	 struct vertex_tangent {
		float x,y,z;
		static const int size = 3;
		static const GLenum type = GL_FLOAT;
		static BASEIMPORT char* name; //"tangent";
	} PACKED;
	
	/**
	 *	A standard vertex format consisting of 3 position elements (x,y,z), 3 normal elements, 
	 *	and one set of texture coordinates.
	 */
	typedef vertex_template<3,3,1,0, null_type> vertex1;
	/**
	 *	A standard vertex format consisting of 3 position elements (x,y,z), 3 normal elements, 
	 *	one set of texture coordinates, and a set of 'tangent' shader attribute variables.
	 */
	typedef vertex_template<3,3,1,0, tuple<vertex_tangent, null_type> > vertex_t; 
 	
 	/** vertex structure used for sprites */
 	typedef vertex_template<3,3,1,4,null_type> SpriteVertex;
	
	/** vetrex structure used for lines */
	typedef vertex_template<3,0,0,4,null_type> LineVertex;
	
	/**
	 * Texture Coordinate structure. Contains uv texture data for a vertex	
	 */
	#ifdef _MSC_VER
	#pragma pack(1)
	#endif	
	struct GLTexCoord{
		float u;
		float v;
	#ifdef _MSC_VER
	};
	#else	
	} __attribute__((packed));
	#endif
	
	
	/**
	 * Vertex structure, used for position, normals and tangents
	 * Generally all vertex data needing three components
	 */
	#ifdef _MSC_VER
	#pragma pack(1)
	#endif
	struct GLVertex{
		float x;
		float y;
		float z;
	#ifdef _MSC_VER
	};
	#else	
	} __attribute__((packed));
	#endif
	
	/**
	 * Vertex Colour information using four components (rgba).
	 */
	#ifdef _MSC_VER
	#pragma pack(1)
	#endif
	struct GLColour{
		float r;
		float g;
		float b;
		float a;
	#ifdef _MSC_VER
	};
	#pragma pack(pop)
	#else	
	} __attribute__((packed));
	#endif
};

#endif
