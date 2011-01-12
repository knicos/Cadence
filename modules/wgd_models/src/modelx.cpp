#include <wgd/modelx.h>
#include <vector>
#include <wgd/bone.h>
#include <wgd/mesh.h>
#include <wgd/material.h>
#include <wgd/colour.h>
#include <wgd/vertex.h>
#include <wgd/animationset.h>

using namespace wgd;
using namespace cadence;
using namespace cadence::token;

static const cadence::token::Char<1> semi(';');
static const cadence::token::Char<1> comma(',');

bool ModelX::validate(File *file){
	if(file->parse(Keyword("xof"))){
		file->seek(5);
		bool valid = file->parse(Keyword("txt 0032"));
		if(!valid) Warning(0, "Invalid .x file");
		file->seek(0, Stream::BEG);
		return valid;
	} 
	return false;
}


void ModelX::load(){
	
	std::cout << "Loading Model " << file()->filename() << "\n";

	file()->open(File::READ);
	
	file()->seek(16);
	
	//main loop
	while(!file()->eof()){
		parseJunk();
		if(file()->eof()) break;
		
		if(parse(Keyword("template"))) skipBlock();
		else if(parse(Keyword("Frame"))) readFrame(createBone(ix::root));
		else if(parse(Keyword("Mesh"))) readMesh(Null);
		else if(parse(Keyword("AnimationSet"))) readAnimationSet();
		else if(parse(AlphaNumericX())) skipBlock();
		else Error(0, "ModelX: Some crazy error of doom");
	}
	
	file()->close();
}


void ModelX::skipBlock(){
	startBlock();
	endBlock();
}

OID ModelX::startBlock(){
	token::AlphaNumericX name;
	parse(WhiteSpace());
	parseValue(name);
	parse(WhiteSpace());
	parse(Char<1>('{'));
	return (name.value)? OID(name.value) : Null;
}

void ModelX::endBlock(){
	int count=1;
	while(count>0 && !file()->eof()) { 
		if(parse(Char<1>('{'))) count++;
		else if(parse(Char<1>('}'))) count--;
		else file()->seek(1);
	}
}

OID ModelX::readInline(){
	AlphaNumericX s;
	parseJunk();
	parseValue(s);
	parse(Block<-1>('}'));
	return (s.value)? OID(s.value) : Null;
}

void ModelX::readFrame(Bone *parent){
	OID name = startBlock();
	Bone *cBone;
	
	ALIGNVS float tMatrix[16] ALIGNED;
	
	//create bone
	cBone = createBone(name);
	parent->addChild(name);
	
	
	while(!file()->eof()){
	
		parseJunk();
	
		if(parse(Keyword("FrameTransformMatrix"))){ 
			startBlock();
			readMatrix(tMatrix);
			cBone->tMatrix(tMatrix); 
			endBlock();
		} else if(parse(Keyword("Frame"))) readFrame(cBone);
		else if(parse(Keyword("Mesh"))) readMesh(name);
		else if(parse(Char<1>('{'))) readInline(); //Mesh reference - ignore for now (and probably forever)
		else break;
	}
	
	endBlock();
}

void ModelX::readMatrix(float* m){
	token::Float f;
	for(int i=0; i<16; i++){
		parseJunk();
		parseValue(f);
		m[i] = f.value;
		parse(Char<1>(','));
		parse(WhiteSpace());
	}
}

vector3d ModelX::readVector(int n){ return parseVector(n, semi); }

void ModelX::readMesh(const OID& boneName){

	int nVertices=0;
	vector3d *vertices = NULL;
	vector3d *normals = NULL;
	vector2d *uvs = NULL;
	
	int nFaces=0;
	int *faces = NULL;
	int *matList = NULL;
	OID *materials = NULL;
	int *duplication = NULL;
	
	int weights = 0;
	MeshSkin *skin = NULL;
	
	//The actual parsing...
	OID name = startBlock();
	
	nVertices = readVertices(vertices);
	nFaces = readFaces(faces, duplication);
	
	//sub blocks
	while(!file()->eof()){
	
		parseJunk();
	
		if(parse(Keyword("MeshNormals"))) readNormals(normals);
		else if(parse(Keyword("MeshTextureCoords"))) readTexCoords(uvs);
		else if(parse(Keyword("MeshMaterialList")))  readMaterials(nFaces, matList, materials, duplication);
		else if(parse(Keyword("XSkinMeshHeader"))) {
			weights = readSkinHeader(skin);
			skin = new MeshSkin[weights * nVertices];
			memset(skin, 0, weights * nVertices * sizeof(MeshSkin));
		}else if(parse(Keyword("SkinWeights"))) readSkin(skin, weights);
		else if(parse(Keyword("VertexDuplicationIndices"))) skipBlock();
		else break;
	}
	
	//Process mesh (divide it into sub meshes per material - also fo any flipping stuff)
	processMesh(boneName, nFaces, faces, vertices, normals, uvs, matList, materials, weights, skin);
	
	endBlock();
}


int ModelX::readVertices(vector3d *& vertices){
	token::Integer<10> tint;
	
	parseJunk();
	int nVertices=0;
	//if(!parseValue(tint)) return 0;
	if(!tint.parse(*file())) return 0;
	
	nVertices = (int)tint.value;
	parse(semi);	
			
	
	vertices = new vector3d[nVertices];
	for(int i=0; i<nVertices; i++){
		vertices[i] = readVector();
		parse(comma);
	}
	parse(semi);
	return nVertices;
}

int ModelX::readFaces(int *& faces, int *& duplication){
	token::Integer<10> tint;
	std::vector<int> tfaces;
	std::vector<int> tduplication;
	
	int nFaces, sides;
	int f[2] = {-1, -1};
	
	parseJunk();
	
	//char a = file()->peek(); //no idea
	//char b = file()->peek(1);
	
	if(!parseValue(tint)) return 0;
	nFaces = (int)tint.value;		
	parse(semi);
	
	for(int i=0; i<nFaces; i++){
		parseJunk();
		parseValue(tint); parse(semi);
		sides = (int)tint.value;
				
		for(int j = 0; j<sides; j++){
			parseJunk();
			parseValue(tint); parse(comma);
			
			if(j<2) f[j] = (int)tint.value;
			else {
				tfaces.push_back(f[0]);
				tfaces.push_back(f[1]);
				tfaces.push_back((int)tint.value);
				f[1] = (int)tint.value;
			}
			
			//	Record if there are extra faces added to fix material list
			if(j>2) {
				tduplication.push_back(i);
			}
			
		}
		parse(semi);
		parse(comma);
	}
	parse(semi);
	
	//return data
	faces = new int[tfaces.size()];
	memcpy(faces, &tfaces[0], tfaces.size() * sizeof(int));
	
	//material duplication
	tduplication.push_back(-1);
	duplication = new int[tduplication.size()];
	memcpy(duplication, &tduplication[0], tduplication.size() * sizeof(int));	
	
	//return number of triangles
	return tfaces.size() / 3;
}

int ModelX::readNormals(wgd::vector3d *& normals){
	token::Integer<10> tint;
	
	startBlock();
	
	parseJunk();
	int nNormals=0;
	parseValue(tint); parse(semi);	nNormals = (int)tint.value;		
	
	normals = new vector3d[nNormals];
	for(int i=0; i<nNormals; i++){
		normals[i] = readVector();
		parse(comma);
	}
	parse(semi);
	endBlock();
	return nNormals;
}


int ModelX::readTexCoords(wgd::vector2d *& uvs){
	token::Integer<10> tint;
	
	startBlock();
	
	parseJunk();
	int nUVs=0;
	parseValue(tint); parse(semi);	nUVs = (int)tint.value;		
	
	uvs = new vector2d[nUVs];
	for(int i=0; i<nUVs; i++){
		uvs[i] = readVector(2);
		parse(comma);
	}
	parse(semi);
	endBlock();
	return nUVs;
}

int ModelX::readMaterials(int count, int *&matID, OID *& material, int *& duplication){
	//count=number of triangles (may be different from faces), duplication=indices to be duplicated
	//This one is slightly different
	token::Integer<10> tint;
	
	startBlock();
	
	parseJunk();
	int nFaces=0;
	int nMaterials=0;
	
	parseValue(tint); parse(semi);	nMaterials = (int)tint.value;	
	
	parseJunk();
	parseValue(tint); parse(semi);	nFaces = (int)tint.value;
	
	int index=0, dix=0;
	matID = new int[count];
	for(int i=0; i<nFaces; i++){
		parseJunk();
		parseValue(tint); parse(comma);
		matID[index++] = (int)tint.value;
		
		//duplicated indices
		while(duplication[dix]==i) {
			matID[index++] = (int)tint.value;
			dix++;
		}
	}
	parse(semi);
	parse(semi); //coz blender is crap
	
	//Materials defined here
	material = new OID[nMaterials];
	for(int i=0; i<nMaterials; i++){
		parseJunk();
		if(parse(Keyword("Material"))) material[i] = readMaterial();
		else if(parse(Char<1>('{'))) material[i] = readInline();
	}
	
	endBlock();
	return nMaterials;
}

OID ModelX::readMaterial(){
	token::Float tfloat;
	vector3d v;
	
	OID name = startBlock();
	
	//Create the material resource
	Material *mat = createMaterial(name);
	
	//diffuse (ambient)
	parseJunk();
	v = readVector(4);
	if(mat->get(ix::diffuse)==Null) mat->diffuse(colour(v.x, v.y, v.z, v.w));
	if(mat->get(ix::ambient)==Null) mat->ambient(colour(v.x/3.0f, v.y/3.0f, v.z/3.0f, 1.0));
	parse(semi);
	
	//Shininess
	parseJunk();
	parseValue(tfloat);
	if(mat->get(ix::shininess)==Null) { 
		mat->shininess(tfloat.value);
		//Blender bug - this value is between 0-2 rather than 0-128:
		if(tfloat.value > 0.0 && tfloat.value < 1.0) mat->shininess(tfloat.value * 64.0f);
	}
	
	parse(semi);
	
	//specular
	parseJunk();
	v = readVector();
	if(mat->get(ix::specular)==Null) mat->specular(colour(v.x, v.y, v.z, 1.0));
	parse(semi);	
	
	//emmissive
	parseJunk();
	v = readVector();
	if(mat->get(ix::emission)==Null) mat->emission(colour(v.x, v.y, v.z, 1.0));
	parse(semi);

	//Texture
	parseJunk();
	if(parse(Keyword("TextureFilename"))){
		startBlock();
		
		parseJunk();
		
		//read string (append to directory(file()->localFile(), buf); and save)
		Block<-1> texFile('"', '"');
		parseValue(texFile);
		
		//create texture
		if(mat->texture()==NULL || mat->texture()->file()==NULL){
			char tdir[128];
			directory((const char*)file()->filename(), tdir);
			dstring tf(tdir); tf + texFile.value;		
		
			if(mat->texture()==NULL) mat->texture(new Texture);
			mat->texture()->file(new LocalFile((const char*)tf));
		}
		
		endBlock();
	}
	
	
	endBlock();
	return name;
}

int ModelX::readSkinHeader(wgd::MeshSkin *&){
	startBlock();
	parseJunk();
	
	Integer<10> wpv;
	parseValue(wpv);
	
	endBlock();
	return (int)wpv.value;
}

int ModelX::readSkin(wgd::MeshSkin *&skin, int weights){
	Block<-1> boneName('"', '"');
	Integer<10> count;
	Integer<10> tint;
	Float tfloat;
	
	startBlock();
	parseJunk();
	parseValue(boneName); parse(semi);
	
	Bone *bone = createBone(boneName.value);
	
	parseJunk();
	parseValue(count); parse(semi);
	
	int *index = new int[(int)count.value];
	for(int i=0; i<(int)count.value; i++){
		parseJunk();
		parseValue(tint);
		parse(comma);
		index[i] = (int)tint.value;
	}
	parse(semi);
	
	for(int i=0; i<(int)count.value; i++){
		parseJunk();
		parseValue(tfloat);
		parse(comma);
		
		for(int j=0; j<weights; j++){
			if(skin[index[i] * weights + j].weight==0.0){
				skin[index[i] * weights + j].weight = tfloat.value;
				skin[index[i] * weights + j].bone = bone;
				break;
			}
		}
	}
	parse(semi);
	
	ALIGNVS float mat[16] ALIGNED;
	readMatrix(mat);
	bone->sMatrix(mat);
	
	endBlock();
	return (int) count.value;
}


void ModelX::processMesh(const OID& boneName, int nfaces, int *f, wgd::vector3d *v, wgd::vector3d *n, wgd::vector2d *uv, int *m, OID *mat, int w, MeshSkin *skin){
	int offset=0;
	while(offset < nfaces){
		
		//see how many faces in this material
		int num = 0, cmat = m[offset];
		for(int i=offset; i<=nfaces; i++){
			if(m[i]!=cmat || i==nfaces) { num = i-offset; break; }
		}
		
		//create mesh
		if(num>0){
			
			//copy vertex data
			MeshSkin *s=0;
			vertex_t *vx = new vertex_t[num * 3];
			if(w) s = new MeshSkin[num * 3 * w];
			for(int i=0; i<num * 3; i++){
				int ix = f[i + offset*3];
				vx[i].position.x = v[ix].x;
				vx[i].position.y = v[ix].y;
				vx[i].position.z = v[ix].z;
				
				if(n){
					vx[i].normal.x = n[ix].x;
					vx[i].normal.y = n[ix].y;
					vx[i].normal.z = n[ix].z;
				}
				
				if(uv){
					vx[i].texcoords[0].u = uv[ix].x;
					vx[i].texcoords[0].v = 1.0f - uv[ix].y;
				}
				
				if(w){
					for(int j=0; j<w; j++){
						s[i*w + j].bone = skin[ix*w + j].bone;
						s[i*w + j].weight = skin[ix*w + j].weight;
						
						//check for non-skin
						if(j==0 && s[i*w].weight==0.0){
							s[i*w].weight = 1.0;
							if(boneName==Null) s[i*w].bone = createBone(ix::root);
							else s[i*w].bone = createBone(boneName);
						}
					} 
				}
				
			}
	
			//Save mesh to the model
			Mesh *m = createMesh();
			m->vertices(num * 3, vx);
			m->skin(w, s);
			//std::cout << "CMAT = " << cmat << "\n";
			OID mate = mat[cmat];
			m->material(mate);
			if(boneName!=Null) m->bone(boneName);
			
			offset += num;
		}
	}
}


void ModelX::readAnimationSet(){ 
	OID name = startBlock();
	//create animation set
	AnimationSet *set = createAnimation(name);
	while(!file()->eof()){
		parseJunk();
		if(parse(Keyword("Animation"))) readAnimation(set);
		else break;
	}
	endBlock();
}

void ModelX::readAnimation(AnimationSet *set){ 
	OID name = startBlock();
	OID boneName;
	
	//get bone it applies to
	parseJunk();
	if(parse(Char<1>('{'))) boneName = readInline();
	
	if(boneName==Null){
		Error(0, "ModelX: No frame defined for animation");
		endBlock();
		return;
	}
	
	int bID = createBone(boneName)->bid();
	
	//Read Keyframes
	while(!file()->eof()){
		parseJunk();
		if(parse(Keyword("AnimationKey"))) readAnimationKey(set, bID);
		else break;
	}	
	
	endBlock();
}
void ModelX::readAnimationKey(AnimationSet*set, int bone){
	token::Integer<10> tint;
	int type, keys, frame, size;
	vector3d v;
	
	startBlock();
	
	//type
	parseJunk();	parseValue(tint); 	parse(semi);
	type = (int)tint.value;
	
	//Invalid type
	if(type==4){
		Error(0, "ModelX: Cannot use matrix keys for animation");
		endBlock();
		return;
	}
	
	//number of keyframes
	parseJunk();	parseValue(tint); 	parse(semi);
	keys = (int)tint.value;
	
	for(int i=0; i<keys; i++){
		//keyframe
		parseJunk();	parseValue(tint); 	parse(semi);
		frame = (int)tint.value;
		
		//num values
		parseJunk();	parseValue(tint); 	parse(semi);
		size = (int)tint.value;
	
		//data
		v = parseVector(size, comma);
		
		parse(semi); parse(semi); parse(comma);
		
		switch(type){
		// Note: quaternions are in a strange order - see version 2
		case 0: set->keyOrientation(bone, frame, quaternion(v.y, v.z, v.w, -v.x)); break;	//Rotation Keys
		case 1: set->keyScale(bone, frame, v); break;		//Scale keys
		case 2: set->keyPosition(bone, frame, v); break;	//position keys
		}
	}
	parse(semi);
	
	endBlock();
}
