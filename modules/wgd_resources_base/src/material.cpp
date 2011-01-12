/*
 * WGD Library
 *
 * Authors: 
 * Date: 17/9/2007
 *
 */

#include <wgd/material.h>
#include <cadence/doste/oid.h>
#include <wgd/texture.h>
//#include <wgd/lighting.h>

using namespace cadence;
using namespace cadence::doste;
using namespace wgd;

Material *Material::s_current = 0;
wgd::OID Material::BLEND_NORMAL;
wgd::OID Material::BLEND_MULTIPLY;
wgd::OID Material::BLEND_ADD;
wgd::OID Material::BLEND_NONE;
wgd::OID Material::BLEND_ONE;

void wgd::Material::initialise() {
	BLEND_NORMAL = "normal";
	BLEND_ADD = "add";
	BLEND_NONE = "none";
	BLEND_MULTIPLY = "multiply";
	BLEND_ONE = "one";
}

void wgd::Material::finalise() {
}

wgd::Material::Material()
	: Agent() {
	set("type", type());
}

wgd::Material::Material(File* file) : Agent() {
	set(Type, type());
	Texture *tex = new Texture();
	tex->file(file);
	set(ix::texture, tex);
}

wgd::Material::Material(const colour &colour) : Agent() {
	set(Type, type());
	diffuse(colour);
}

wgd::Material::Material(const OID &res)
	: Agent(res) {
}

wgd::Material::~Material() {
}

void wgd::Material::variable(const char *name, const wgd::OID &v) {
	if (get(ix::variables) == Null) {
		set(ix::variables, wgd::OID::create());
	}

	get(ix::variables).set(name, v);
}

wgd::OID wgd::Material::variable(const char *name) {
	return get(ix::variables)[name];
}

void wgd::Material::bind() {
	if (s_current != this) {
		GLfloat mat[4];
		colour c = specular();
		c.toArray(mat);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
		c = diffuse();
		
		//if diffuse is null, use white (rather than transparent)
		if(get(ix::diffuse)==Null) { c.r=1.0; c.g=1.0; c.b=1.0; c.a=1.0; }
		
		//if (Lighting::enabled() == false) {
		//	glColor4f(c.r,c.g,c.b,c.a);
		//}

		//std::cout << "Colours: " << c.r << " " << c.g << " " << c.b << " " << c.a << "\n";

		c.toArray(mat);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
		c = ambient();
		c.toArray(mat);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
		mat[0] = shininess();
		glMaterialfv(GL_FRONT, GL_SHININESS, mat);
		c = emission();
		c.toArray(mat);
		glMaterialfv(GL_FRONT, GL_EMISSION, mat);
	
	
		s_current = this;

		Texture *tex;

		//if (get(ix::textures) != Null) {
			for (int i=0; i<Texture::maxTextureUnits(); i++) {
				tex = texture(i);
				if (tex == 0) continue;
				tex->bind(i);
			}
		//} else {
		//	if (get(ix::texture) != Null) {
		//		tex = texture();
		//		if(tex!=NULL)
		//			tex->bind();
		//	}
		//}

		if (get(ix::shader) != Null) {
			Shader *shade = shader();
			shade->bind();

			if (Shader::current() == shade) {
				//Set shader variables.
				OID vars = get(ix::variables);
				OID val;
				char sbuf[50];

				if (vars != Null) {
					for (OID::iterator i=vars.begin(); i!=vars.end(); i++) {
						(*i).toString(sbuf, 50);
						val = vars[*i];
						
						if (val.isDouble()) shade->setVariable(sbuf, (float)val);
						if (val.isLongLong()) shade->setVariable(sbuf, (int)val);
						
						//also need arrays
						/*if (val.isObject()) {
							//val is an array of n values
							//either need to send them individually, or
							//create an array and send them all at once
							strcat(sbuf, "[0]");
							
							//get number of values
							int n=0;
							for(n=0; val[n]!=Null; n++);
							
							//floats or ints?
							if (val[0].IsFloat()){
								float *array = new float[n];
								for(int i=0; i<n; i++) array[i] = (float)val[i];
								shade->setVariable(sbuf, n, array);
								delete [] array;
								//cout << "Array " << sbuf << " = ";
								//for(int i=0; i<n; i++) cout << array[i] << ", ";
								//cout <<  " (" << n << " values)\n"; 
								
							}
							if (val.IsInt()){
								int *array = new int[n];
								for(int i=0; i<n; i++) array[i] = (int)val[i];
								shade->setVariable(sbuf, n, array);
								delete [] array;
							}	
						}*/
					}
				}
			}
		}

		OID blnd = blending();
		if (blnd == BLEND_MULTIPLY) {
			glBlendFunc (GL_DST_COLOR, GL_ZERO);
		} else if (blnd == BLEND_ONE) {
			glBlendFunc(GL_ONE, GL_ONE);
		} else if (blnd == BLEND_ADD) {
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		} else {
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		//} else {
		//	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
		//}
	}
}

void wgd::Material::unbind() {

	Texture *tex;

	//if (get(ix::textures) != Null) {
		for (int i=Texture::maxTextureUnits()-1; i>=0; i--) {
			tex = texture(i);
			if (tex == 0) continue;
			tex->unbind();
		}
	//} else {
	//	if (get(ix::texture) != Null) {
	//		tex = texture();
	//		if (tex != 0)
	//			tex->unbind();
	//	}
	//}

	if (get(ix::shader) != Null) {
		Shader *shade = shader();
		shade->unbind();
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//reset diffuse
	GLfloat mat[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
	
	if(s_current==this) s_current=NULL;
}

void wgd::Material::texture(int num, Texture *tex) {
	if (get(ix::textures) == Null) {
		if (num == 0) {
			set(ix::texture, *tex);
		} else {
			set(ix::textures, OID::create());
			get(ix::textures).set(0, get(ix::texture));
			get(ix::textures).set(num, *tex);
		}
	} else {
		get(ix::textures).set(num, *tex);
	}
}

Texture *wgd::Material::texture(int num) {
	if (get(ix::textures) == Null) {
		if (num == 0 ) {
			return (Texture*)get(ix::texture);
		} else {
			return 0;
		}
	} else {
		return (Texture*)(get(ix::textures).get(num));
	}
}
