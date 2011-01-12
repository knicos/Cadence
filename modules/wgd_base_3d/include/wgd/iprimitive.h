/*
 * WGD Library
 *
 * Authors: 
 * Date: 19/9/2007
 *
 */

#ifndef _WGD_IPRIMATIVE_H_
#define _WGD_IPRIMATIVE_H_

#include <wgd/instance3d.h>
#include <wgd/drawable.h>
#include <wgd/material.h>
#include <wgd/vertex.h>
#include <wgd/index.h>

namespace wgd {
	
	/**
	 *	Primitives are basic shapes you can use as part of something greater
	 *	they are not database objects so it will be faster to use many of them
	 *	The vertex data is also re-used (eg one vertex array for all cubes);
	 */
	class BASE3DIMPORT Primitive {
		public:
		Primitive();
		~Primitive();
		
		/** get the drawable object */
		//Drawable<vertex_t> *drawable() { return m_drawable; }
		void draw(SceneGraph &graph, Camera3D *camera);
		
		private:
		
		
		float m_width, m_height, m_depth; // bounding box
		float m_tu, m_tv; // texture scale
		
		
		//cube: width, height, depth, tu, tv
		//cylinder: radius/width height, depth
		//sphere:
		//plane:
		
	};
	
	/**
	 * Draw simple shapes. Currently this class only provides a cube (rectangle).
	 */
	class BASE3DIMPORT IPrimitive3D : public wgd::Instance3D {
		public:
		IPrimitive3D();
		IPrimitive3D(const cadence::doste::OID &);
		
		/**
		 * Draw this primative.
		 */
		virtual void draw(SceneGraph &graph, Camera3D *camera);

		/**
		 * Select type of primative. Currently can only do a cube and is
		 * specified with IPrimative::CUBE. This is optional since it defaults
		 * to cube anyway.
		 */
		PROPERTY_WF(cadence::doste::OID,primitive,ix::primitive);
		/** Get the primative this will draw. */
		PROPERTY_RF(cadence::doste::OID,primitive,ix::primitive);

		/** Specify width of primative */
		PROPERTY_WF(float,width,ix::width);
		/** Current width. */
		PROPERTY_RF(float,width,ix::width);

		/** Specify height if primative. */
		PROPERTY_WF(float,height,ix::height);
		/** Current height. */
		PROPERTY_RF(float,height,ix::height);

		/** Specify depth of primative */
		PROPERTY_WF(float,depth,ix::depth);
		/** Current depth */
		PROPERTY_RF(float,depth,ix::depth);
		
		/**
		 * Set the size of the primative.
		 * @param w Width of primative
		 * @param h Height of primative
		 * @param d Depth of primative
		 */
		void size(float w, float h, float d);

		/**
		 * If applicable this sets radius. Currently unused.
		 */
		PROPERTY_WF(float,radius,ix::radius);
		/** Get radius. May not always have a value, in which case it is 0. */
		PROPERTY_RF(float,radius,ix::radius);
		
		/** Set number of sides in the sphere  and cylinder */
		PROPERTY_WF(int,sides, "sides");
		/** Get the number of sides of this primative */
		PROPERTY_RF(int,sides, "sides");
		
		/**
		 * Attach a material. This defines the appearance of the cube but giving it
		 * a texture, shader and lighting effects.
		 */
		PROPERTY_WF(Material, material,ix::material);
		/** Get current material. */
		PROPERTY_RF(Material, material,ix::material);

		
		OBJECT(Instance3D, IPrimitive3D);
		
		private:
	};

	
};


#endif 
