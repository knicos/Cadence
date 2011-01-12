#include <wgd/morphcontroller.h>

using namespace wgd;

MorphController::MorphController(wgd::Instance3D* inst) : m_meshes(0), m_inst(inst), m_time(0), m_duration(0) {
	m_model = inst->get(ix::model);
	
}
MorphController::~MorphController() {
	if(m_meshes){
		delete [] m_time;
		delete [] m_duration;
	}
}

void MorphController::resize(int n){
	//resize arrays
	float* ttime = new float[n];
	float* tdur = new float[n];
	//copy any existing data
	if(m_meshes){
		memcpy(ttime, m_time, m_meshes * sizeof(float));
		memcpy(tdur, m_duration, m_meshes * sizeof(float));
		delete [] m_time;
		delete [] m_duration;
	}
	//set member array pointers to point to new arrays
	m_time = ttime;
	m_duration = tdur;
	
	m_meshes = n;
}

bool MorphController::update() {
	float time = (float)WGD.get(ix::time);
	float mtime, value;
	for(int i=0; i<m_meshes; i++){
		
		//current morph
		OID cmorph = m_inst->get(ix::meshes)[i][ix::morph];
		if(cmorph!=Null){
			
			//morph time
			mtime = time - m_time[i];
			
			//get morph interpolation value
			if(m_duration[i]==0.0) value=1.0f;
			else value = mtime / m_duration[i];
			
			//next morph?
			if(value >= 1.0f){
				OID next = m_model->get(ix::morphs)[cmorph][ix::next];
				if(next!=Null) m_inst->get(ix::meshes).get(i).set(ix::morph, next);
				value = 1.0;
			}
			
			//get meshes
			Mesh *a = (OID)m_model->get(ix::morphs)[cmorph][ix::start];
			Mesh *b = (OID)m_model->get(ix::morphs)[cmorph][ix::end];
			
			//_ASSERT(a && b);
			
			if(m_inst->get(ix::meshes).get(i)==Null) m_inst->get(ix::meshes).set(i, *new Mesh);
			Mesh *deformed = m_inst->get(ix::meshes).get(i);
			
			//deform mesh
			deform(deformed, a, b, value);
		}
	}
	
	return m_meshes>0;
}

void MorphController::changeMorph(int mesh, float transition) {
	if(mesh >= m_meshes) resize(mesh+1);
	
	OID morph = m_inst->get(ix::meshes)[mesh][ix::morph];
	
	m_time[mesh] = (float)WGD.get(ix::time);
	m_duration[mesh] = m_model->get(ix::morphs)[morph][ix::time];
}

void MorphController::deform(Mesh* dest, Mesh *a, Mesh* b, float pcnt) {
	//copy material
	dest->set(ix::material, a->material());
	
	//Allocate and clear memory in destination mesh
	dest->create(a->vertices());
	
	meshVertex *vdest, *va, *vb;
	vector3d pos, start, end;
	
	for(int i=0; i<a->vertices(); ++i){
		vdest = &dest->data()[i];
		va = &a->data()[i];
		vb = &b->data()[i];
		
		//position
		start.fromArray(&va->position.x);
		end.fromArray  (&vb->position.x);
		pos = vector3d::lerp(start, end, pcnt);
		memcpy(&vdest->position.x, &pos.x, 3*sizeof(float));
		
		//Normals
		start.fromArray(&va->normal.x);
		end.fromArray  (&vb->normal.x);
		pos = vector3d::lerp(start, end, pcnt);
		memcpy(&vdest->normal.x, &pos.x, 3*sizeof(float));
		
		//Texture coordinates
		start.fromArray(&va->texcoords[0].u);
		end.fromArray  (&vb->texcoords[0].u);
		pos = vector2d::lerp(start, end, pcnt);
		memcpy(&vdest->texcoords[0].u, &pos.x, 2*sizeof(float));
		
		//Tangents
		start.fromArray(&va->custom.var.x);
		end.fromArray(&vb->custom.var.x);
		pos = vector3d::lerp(start, end, pcnt);
		memcpy(&vdest->custom.var.x, &pos.x, 3*sizeof(float));
	}
}
