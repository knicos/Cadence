#include <wgd/imodel.h>
#include <wgd/material.h>
#include <wgd/extensions.h>
#include <wgd/drawable.h>
#include <GL/gl.h>

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

//Implement model loader base class
IMPLEMENT_LOADER(ModelLoader);

namespace wgd {
	OnEvent(IModel, evt_update){
		//update animationcontroler
		bool changed;
		if(m_anim) changed = m_anim->update(true);
		if(m_morph) m_built &= !m_morph->update();
		if(m_skin && changed) m_built &= !m_skin->update();
		m_forceUpdate = false;
	}
	OnEvent(IModel, evt_setup){
		//setup controllers
		if(m_anim) delete m_anim; m_anim=0;
		if(m_skin) delete m_skin; m_skin=0;
		if(m_morph) delete m_morph; m_morph=0;
		if((bool)get(ix::model).get("loaded")) {
			Model *mdl = model();
			if(mdl->bones() > 1){
				//create animation controler
				m_anim = new AnimationController(*this);
				
				//start initial animation
				OID startAnim = get(ix::animation);
				if(startAnim!=Null) {
					m_anim->animation(0).start(startAnim);
					m_anim->update(true); //force update
				}
				
				//check all meshes to see if we need the skincontroler
				for(int i=0; i<mdl->meshes(); i++) {
					if(mdl->mesh(i)->skin() != NULL) {
						m_skin = new SkinController(m_anim);
						break;
					}
				}
			}
			
			if(mdl->get(ix::morphs)!=Null) {
				m_morph = new MorphController(this);
				//initialise morphs
				for(OID::iterator i=get(ix::meshes).begin(); i!=get(ix::meshes).end(); i++){
					m_morph->changeMorph(*i, get(ix::morphs)[*i][ix::morph]);
				}
			}
		}
		
	}
	OnEvent(IModel, evt_bone) {
		//add dependancies to any bones
		OID bones = get(ix::bones);
		for(OID::iterator i = bones.begin(); i!=bones.end(); i++) {
			IBone *bone = bones.get(*i); //make object
			if(bone) DEPENDENCY(*bone, ix::changed);
		}
		//tell update loop that bones have changed
		m_forceUpdate = true;
	}
	
	OnEvent(IModel, evt_animation){
		if(m_anim) {
			m_anim->animation(0).start(animation());
		}
	}
	OnEvent(IModel, evt_morph){
		for(OID::iterator i=get(ix::meshes).begin(); i!=get(ix::meshes).end(); i++){
			addEvent(*i, get(ix::meshes).get(*i)(ix::morph));
		}
	}
	IMPLEMENT_EVENTS(IModel, Instance3D);
}

bool IModel::handler(int i){
	if(m_morph) m_morph->changeMorph(i);
	return false;
}

IModel::IModel() : Instance3D(), m_built(true), m_anim(0), m_morph(0), m_skin(0) {
	set(ix::type, type());
	if(scale() == vector3d()) scale(1.0);
	registerEvents();
}
IModel::IModel(Model *mdl) : Instance3D(), m_built(true), m_anim(0), m_morph(0), m_skin(0) {
	set(ix::type, type());
	if(scale() == vector3d()) scale(1.0);
	registerEvents();
	model(mdl);
}
IModel::IModel(const cadence::doste::OID &id) : Instance3D(id), m_built(true), m_anim(0), m_morph(0), m_skin(0) {
	if(scale() == vector3d()) scale(1.0);
	registerEvents();
}

IModel::~IModel(){
	if(m_anim) delete m_anim;
	if(m_morph) delete m_morph;
	if(m_skin) delete m_skin;
	
	///stop if no model
	if((*this)[ix::model]==Null) return;
}

/** Get a material by name */
Material* IModel::material(const cadence::doste::OID &name){
	//Look for material inside instance (for overridden textures)
	if(get(ix::materials)!=Null){
		if(get(ix::materials)[name]!=Null){
			return get(ix::materials).get(name);
		}
	}
	//look for the material inside the instance's model
	if(!model()) return NULL;
	return model()->material(name);
}

matrix IModel::boneMatrix(const OID &boneName) {
	if(m_anim) return localMatrix() * m_anim->boneMatrix(boneName);
	else return localMatrix();
}

IBone* IModel::bone(const OID& boneName) {
	//return if bone does not exist in model
	if(!model()->bone(boneName)) return 0;
	
	//get IBone
	IBone *b = get(ix::bones)[boneName];
	if(!b) {
		b = new IBone();
		get(ix::bones)[boneName] = b;
	}
	return b;
}

void IModel::draw(SceneGraph &graph, Camera3D *camera){

	Instance3D::draw(graph, camera);
	
	Model *mdl = model();
	if(!mdl) return;
	
	//the local matrix does not take scale into account!
	matrix scl; scl.scale(scale());
	matrix local = localMatrix() * scl;
	
	//Add all meshes to scene graph ======================
	Mesh* mesh = 0;
	int meshes = mdl->meshes();
	if(m_morph) meshes = m_morph->meshes();
	for(int i=0; i<meshes; i++){
		//get deformed mesh
		if(get(ix::meshes)[i]!=Null){
			mesh = get(ix::meshes).get(i);
		} else {
			if(m_morph) break;
			mesh = mdl->mesh(i);
		}
		
		//Send mesh to Scene graph
		Drawable<vertex_t> *d = new Drawable<vertex_t>;
		d->clearData = false;
		d->discard = true;
		d->data = mesh->data();
		d->size = mesh->vertices();
		Material *mat = material(mesh->material());
		if(mat) d->surface = *mat;
		
		//mesh local transformation
		if(m_anim) d->transform = local * m_anim->transformMesh(mesh);
		else d->transform = local;
		
		graph.addSolid(d);
	}
}
