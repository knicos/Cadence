#include <cadence/doste/doste.h>
#include <wgd/model.h>
#include <wgd/material.h>
#include <wgd/imodel.h>
#include <wgd/loader.h>
#include <wgd/model3ds.h>
#include <wgd/modelx.h>
#include <wgd/clipmap.h>


using namespace cadence;
using namespace cadence::doste;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

extern "C" void MODELSIMPORT initialise(const cadence::doste::OID &base) {
	Object::registerType<wgd::Model>();
	Object::registerType<wgd::Mesh>();
	Object::registerType<wgd::IModel>();
	Object::registerType<wgd::IBone>();
	Object::registerType<wgd::ClipMap>();
	
	//Model loaders
	wgd::Loader::regFileType<wgd::Model3DS>();
	wgd::Loader::regFileType<wgd::ModelX>();
}

extern "C" void MODELSIMPORT finalise() {

}


using namespace wgd;

namespace wgd {
	OnEvent(Model, evt_file) {
		if(m_loader) delete m_loader;
		m_loader = Loader::create<ModelLoader>(file());
	}
	
	IMPLEMENT_EVENTS(Model,Agent);
}

Model::Model(const char* filename) : Agent(), m_nMeshes(0), m_radius(0), m_loaded(false), m_loader(NULL) {
	registerEvents();
	file(new LocalFile(filename));
}
Model::Model(cadence::File *f) : Agent(), m_nMeshes(0), m_radius(0), m_loaded(false), m_loader(NULL) {
	registerEvents();
	file(f);
}
Model::Model(const cadence::doste::OID &id) : Agent(id), m_nMeshes(0), m_radius(0), m_loaded(false), m_loader(NULL) {
	registerEvents();
}
	
Model::~Model(){
	//delete bones
	for(unsigned int i=0; i<m_bones.size(); i++) delete m_bones[i];
}


void Model::load(){
	//if(!m_loader && get("file")!=Null) m_loader = Loader::create<ModelLoader>(file());
	if(!m_loaded && m_loader) {
		m_loader->parent(this);
		m_loader->load();
		m_loaded=true;
		set("loaded", true);
	}
}


int Model::meshes() {
	load(); 
	return m_nMeshes; 
}

Mesh *Model::mesh(int index) const {
	if(get(ix::meshes)[index]==Null) return NULL;
	return get(ix::meshes).get(index);
}

Bone *Model::bone(const cadence::doste::OID &name) {
	const OID &id = m_boneMap[name];
	if(id==Null) return NULL;
	else return boneID((int)id);
}

Bone *Model::boneID(int bid) const {
	if(bid>=(int)m_bones.size() || bid<0) return NULL;
	return m_bones[bid];
}

/** Get a material from teh model */
Material *Model::material(const cadence::doste::OID &mat) const{
	if(get(ix::materials)[mat]==Null) return NULL;
	return get(ix::materials).get(mat);
}

AnimationSet *Model::animationSet(const cadence::doste::OID &name){
	OID a = m_animationMap[name];
	if(a==Null) return NULL;
	else return (AnimationSet*)(void*)a;
}

/////////	LOADER FUNCTIONS	/////////


//texture directory - Model Loader
char* ModelLoader::directory(dstring s, char* d) const {
	strcpy(d, (const char*)s);
	for(int i=s.size(); i>0; i--){
		if(d[i]=='/' || d[i] == '\\') break;
		d[i]=0;
	}
	return d;
}

Bone *ModelLoader::createBone(const cadence::doste::OID &name){
	//check if bone already exists
	Bone* b = m_model->bone(name);
	if(b) return b;
	
	//create new bone
	int id = m_model->m_bones.size();
	m_model->m_bones.push_back(new Bone(id));
	m_model->m_boneMap.add(name, id);
	
	if(m_model->get(ix::bones)==Null) m_model->set(ix::bones, OID::create());
	m_model->get(ix::bones).set(id, name);
	
	return m_model->m_bones.back();
}

Mesh *ModelLoader::createMesh(){
	if(m_model->get(ix::meshes)==Null) m_model->set(ix::meshes, OID::create());
	Mesh *m = m_model->mesh(m_meshCounter);
	if(m) return m;
	
	//create a mesh
	m = new Mesh;
	m_model->get(ix::meshes).set(m_meshCounter++, *m);
	m_model->m_nMeshes=m_meshCounter;
	return m;
}

Material *ModelLoader::createMaterial(cadence::doste::OID &name){
	if(m_model->get(ix::materials)==Null) m_model->set(ix::materials, OID::create());
	
	//Generate material name
	if(name==Null){
		char buf[10]; sprintf(buf, "mat%d", ++m_materialCounter);
		name = OID(buf);
	}
	
	//check prior existance
	Material *mat = m_model->material(name);
	if(mat) return mat;
	
	//create new material	
	mat = new Material(); //Material::create();
	m_model->get(ix::materials).set(name, *mat);
	return mat;
}

AnimationSet *ModelLoader::createAnimation(const cadence::doste::OID &name){
	AnimationSet *anim = new AnimationSet(m_model->bones());
	m_model->m_animationMap.add(name, (void*)anim);
	return anim;
}
