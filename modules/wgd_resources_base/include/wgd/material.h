/*
 * WGD Library
 *
 * Authors: 
 * Date: 17/9/2007
 *
 */

#ifndef _WGD_MATERIAL_
#define _WGD_MATERIAL_

#include <wgd/dll.h>
#include <cadence/agent.h>
#include <wgd/shader.h>
#include <wgd/index.h>
#include <wgd/colour.h>
#include <wgd/texture.h>

namespace wgd {
	struct colour;
	//class Texture;

	/**
	 * Surface material resource. A material specifies lighting properties of a
	 * surface as well as its texture. You can apply a material to a surface
	 * by calling bind() just before you draw that surface. It is also possible
	 * to add a shader to a material instead of a texture.<br/><br/>
	 * <code>
	 * //--- Initialise ---<br/>
	 * Material *mat = Resource::create\<Material\>("shiney_metal1");<br/>
	 * mat->shininess(200);<br/>
	 * mat->specular(Colour(1.0,1.0,1.0,1.0));<br/>
	 * mat->diffuse(Colour(1.0,1.0,1.0,1.0));<br/>
	 * mat->texture(Resource::get\<Texture\>("metal1"));<br/><br/>
	 * //--- Use ---<br/>
	 * Resource::get\<Material\>("shiney_metal1")->bind();<br/>
	 * //...<br/>
	 * Resource::get\<Material\>("shiney_metal1")->unbind();<br/>
	 * </code>
	 */
	class RESIMPORT Material : public cadence::Agent {

		public:

		OBJECT(Agent, Material);
		
		Material();
		/** Create a new material containing a texture */
		Material(cadence::File *texturefile);
		/** Create a material of a colour */
		Material(const wgd::colour &);
		Material(const cadence::doste::OID &);
		~Material();

		/**
		 * Size of highlight. Value between 0 and 128 where 128 is the smallest
		 * highlight and so appears shiniest.
		 * @param v Value between 0 and 128.
		 */
		PROPERTY_WF(float,shininess,ix::shininess);
		/**
		 * Get current shininess value.
		 * @return Number between 0 and 128.
		 */
		PROPERTY_RF(float,shininess,ix::shininess);

		/**
		 * Colour of specular highlight. Here you can specify its
		 * rgb value.
		 * @param c RGB Colour.
		 */
		PROPERTY_WF(colour,specular,ix::specular);
		/**
		 * Get specular colour.
		 * @return RGB colour object.
		 */
		PROPERTY_RF(colour,specular,ix::specular);

		/**
		 * The diffuse colour of the material. Specify its RGBA value
		 * so that you can make this material transparent.
		 * @param c RGBA Colour.
		 */
		PROPERTY_WF(colour,diffuse,ix::diffuse);
		/** Get diffuse colour */
		PROPERTY_RF(colour,diffuse,ix::diffuse);

		/**
		 * Set the ambient colour. Usually this is the same as diffuse colour.
		 */
		PROPERTY_WF(colour,ambient,ix::ambient);
		/** Get diffuse colour. */
		PROPERTY_RF(colour,ambient,ix::ambient);

		/**
		 * Set the ambient colour. Usually this is the same as diffuse colour.
		 */
		PROPERTY_WF(colour,emission,ix::emission);
		/** Get diffuse colour. */
		PROPERTY_RF(colour,emission,ix::emission);

		/** Set the blending effect to use. See BLEND_ */
		PROPERTY_WF(cadence::doste::OID, blending,ix::blending);
		/** Get the blending effect. */
		PROPERTY_RF(cadence::doste::OID, blending,ix::blending);
		
		/** set the alpha test value for this material. fragments with alpha less than this value will be ignored */
		PROPERTY_WF(float, alphaTest, "alphatest");
		/** get the alpha test value */
		PROPERTY_RF(float, alphaTest, "alphatest");
		
		/** Set this materials texture. */
		void texture(wgd::Texture *t) { texture(0, t); }

		/** Set multiple textures for this material. Specify the texture
		 * number between 0 and Texture::maxTextureUnits().
		 */
		void texture(int num, wgd::Texture *);

		/** Get one of the materials textures. */
		Texture *texture(int num=0);

		/** Assign a shader to this material. */
		PROPERTY_WF(wgd::Shader, shader,ix::shader);
		/** Get the shader for this material, may return 0. */
		PROPERTY_RF(wgd::Shader, shader,ix::shader);

		void variable(const char *name, const OID &v);
		OID variable(const char *name);

		/**
		 * Apply the material. Everything drawn after this will use this material until
		 * either it is unbound or another material is bound.
		 */
		void bind();
		/**
		 * Remove this material as the current one. Anything drawn after this will have no
		 * material unless another material is bound.
		 */
		void unbind();

		static cadence::doste::OID BLEND_NORMAL;
		static cadence::doste::OID BLEND_MULTIPLY;
		static cadence::doste::OID BLEND_ADD;
		static cadence::doste::OID BLEND_NONE;
		static cadence::doste::OID BLEND_ONE;

		static void initialise();
		static void finalise();

		private:
		static Material *s_current;
	};
};

#endif
