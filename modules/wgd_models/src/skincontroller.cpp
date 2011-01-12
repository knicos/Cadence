#include <wgd/skincontroller.h>
#include <cadence/messages.h>

using namespace wgd;
using namespace cadence;

SkinController::SkinController(wgd::AnimationController *c) : m_anim(c) {
	//place for deformed meshes to go
	if(m_anim->m_inst->get(ix::meshes)==Null) m_anim->m_inst->set(ix::meshes, OID::create());
	
	//setup local memory
	int num = m_anim->m_model->bones();
	m_offset = new vector3d[num];
	
	
	//Manually aligh matrices
	m_finalA = new matrix[num+1];
	#ifdef X86_64
	m_final = (matrix*)(((unsigned long long)m_finalA + 16) & 0xFFFFFFFFFFFFFFF0);
	#else
	m_final = (matrix*)(((unsigned int)m_finalA + 16) & 0xFFFFFFF0);
	#endif
}
SkinController::~SkinController(){
	delete [] m_offset;
	delete [] m_finalA;
}

bool SkinController::update(){
	OID dest = m_anim->m_inst->get(ix::meshes);
	Mesh *mesh, *deformed;
	calculateFinal();
	for(int i=0; i<m_anim->m_model->meshes(); i++){
		mesh = m_anim->m_model->mesh(i);
		if(mesh->weights()>0){
			//the mesh is skinned - create/get destination mesh
			if(dest.get(i)==Null){
				deformed = new Mesh;
				dest.set(i, *deformed);
			} else deformed = dest.get(i);
			
			if(!deformed) Error(0, "There be something wrong with deformed meshes"); 
			else deform(deformed, mesh);
		}
	}
	return true;
}

void SkinController::calculateFinal(){
	for(int i=0; i<m_anim->m_model->bones(); i++){
		matrix::fastMultiply(m_final[i], m_anim->m_transform[i], m_anim->m_model->boneID(i)->sMatrix());
		m_offset[i] = m_final[i] * vector2d();
	}
}

void SkinController::deform(wgd::Mesh* dest, wgd::Mesh *src) {
	//copy material
	dest->set(ix::material, src->material());
	
	//Allocate and clear memory in destination mesh
	dest->create(src->vertices());
	
	//local vars
	meshVertex *vsrc, *vdest;
	vector3d position, normal, tpos, tnorm;
	Bone *bone=0;
	float weight;
	MeshSkin *skin = src->m_skin;
	int tskin;
	
	int i, j;
	for(i=0; i<src->vertices(); ++i){

		vsrc  = &src->data()[i];
		vdest = &dest->data()[i];

		tskin = i*src->m_weightsPerVertex;

		//Copy UVs
		memcpy(&vdest->texcoords[0].u, &vsrc->texcoords[0].u, sizeof(float)*2);

		//transform vertex
		for(j=0; j<src->m_weightsPerVertex; ++j){
			weight = skin[tskin + j].weight;
			if(weight){
				position.fromArray(&(vsrc->position.x));
				normal.fromArray(&(vsrc->normal.x));
				
				//get transform matrix
				if(skin[tskin + j].bone){
					bone = skin[tskin + j].bone;
					matrix::fastMultiply(tpos, m_final[bone->bid()], position);
					matrix::fastMultiply(tnorm, m_final[bone->bid()], normal);
				}
				
				vector3d::fastScale(tpos, tpos, weight);
				
				vdest->position.x += tpos.x;
				vdest->position.y += tpos.y;
				vdest->position.z += tpos.z;
				
				vector3d::fastScale(tnorm, (tnorm - m_offset[bone->bid()]), weight);
				
				vdest->normal.x += tnorm.x;
				vdest->normal.y += tnorm.y;
				vdest->normal.z += tnorm.z;
				
				//ToDo: Tangents
			}
		}
	}
}
