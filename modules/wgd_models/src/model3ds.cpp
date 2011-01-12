#include <wgd/model3ds.h>
#include <wgd/material.h>
#include <wgd/texture.h>
#include <wgd/colour.h>
#include <wgd/mesh.h>
#include <wgd/vector.h>

#define MAIN3DS			0x4d4d		//Main 3DS chunk, always the first chunk
#define EDIT3DS			0x3d3d		//Chunk whose subchunks contain objects
#define EDITOBJECT		0x4000		//An object which contains vertex and polygon data
#define OBJ_TRIMESH		0x4100		//The triangle mesh
#define TRI_VERTEXL		0x4110		//Vertices
#define TRI_FACEL1		0x4120		//triangles
#define FACE_MATERIAL	0x4130		//material for above polygons
#define MAPPING_COORDS	0x4140		//Texture mapping coordinates
#define MATERIAL		0xAFFF		//material
#define MATERIAL_NAME	0xA000		//material name
#define MAT_AMBIENT		0xA010		//material attributes...
#define MAT_DIFFUSE		0xA020
#define MAT_SPECULAR	0xA030
#define MAT_SHININESS	0xA040
#define MAT_TEXTURE		0xa300		//Material texture file name (limited to 12 characters aparently)

#define KEYFRAME		0xB000		//Beginning of KeyFrame stuff
#define FRAMEINFO		0xB008		//Key frame info
#define OBJECTINFO		0xB002		//Key frame object info
#define NAMEHIER		0xB010		//Object name and hierarchy
#define DUMMYNAME		0xB011		//Dummy object name
#define PIVOTPOINT		0xB020		//Objects pivot point.

#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

bool Model3DS::validate(cadence::File *file){
	unsigned short chunk_id;
	file->read(chunk_id);
	file->seek(0, File::BEG);
	return chunk_id==MAIN3DS;
}

void Model3DS::load(){
	
	File *f = file();
	f->open(File::READ);

	DMsg msg(DMsg::INFO);
	msg << "Loading 3DS model '" << (const char*)f->filename() << "'\n";

	char tdir[128];
	directory((const char*)f->filename(), tdir);
	
	unsigned short chunk_id;
	unsigned int chunk_length;
	unsigned short qty;
	unsigned short face_flags;
	unsigned int hier;
	int i;
	unsigned char lchar;
	
	//geometry data
	char name[50] = "";
	char mName[50] = "";
	vector3d *vertices=NULL;
	vector2d *texCoords=NULL;
	unsigned short *faces=NULL;
	unsigned short nFaces=0;
	
	//material data
	char matName[50] = "";
	char texFile[50] = "";
	Texture *tex = NULL;
	float rgb[3];
	Material *mat = NULL;
	
	int meshindex = 0;

	char buffer[50];
	
	while(!f->eof()){
		f->read(chunk_id);
		f->read(chunk_length);
		
		//std::cout << "chunk " << std::hex << chunk_id << std::dec << " length " << chunk_length << "\n";
		
		switch(chunk_id){
			case KEYFRAME: break;	//Beginning of KeyFrame stuff
			case OBJECTINFO: break;	//Key frame object info
			case NAMEHIER:		//Object name and hierarchy
				i = 0;
				do {
					//f->read((char*)&lchar, 1);
					f->read(lchar);
					if(lchar==32) lchar='_';
					buffer[i] = lchar;
					i++;
				} while (lchar != 0 && i < 50);

				f->read(hier);
				f->read(hier);
				f->read(hier);

				//std::cout << " - Hierarchy: " << buffer << " = " << hier << std::endl;
				break;
			
			case MAIN3DS: break;	//Main 3DS chunk, always the first chunk
			case MATERIAL: break;
			case MATERIAL_NAME:
				i = 0;
				do {
					f->read(lchar);
					if(lchar==32) lchar='_';
					matName[i] = lchar;
					i++;
				} while (lchar != 0 && i < 20);
				
				if(m_model->get(ix::materials)==Null) m_model->set(ix::materials, OID::create());

				if (m_model->get(ix::materials).get(matName) == Null) {
					mat = Material::create();
					m_model->get(ix::materials).set(matName, *mat);
				} else {
					mat = m_model->get(ix::materials).get(matName);
				}
			break;
			
			case MAT_AMBIENT:
				//use only the last 3 bytes
				f->seek(chunk_length - 9);
				
				rgb[0] = (float) f->read<unsigned char>();
				rgb[1] = (float) f->read<unsigned char>();
				rgb[2] = (float) f->read<unsigned char>();

				if (mat->get(ix::ambient) == Null)
				mat->ambient(colour(rgb[0]/256.0f, rgb[1]/256.0f, rgb[2]/256.0f, 1.0f));
				
				break;
			case MAT_DIFFUSE:
				//use only the last 3 bytes
				f->seek(chunk_length - 9);
				
				rgb[0] = (float) f->read<unsigned char>();
				rgb[1] = (float) f->read<unsigned char>();
				rgb[2] = (float) f->read<unsigned char>();

				if (mat->get(ix::diffuse) == Null)
				mat->diffuse(colour(rgb[0]/256.0f, rgb[1]/256.0f, rgb[2]/256.0f, 1.0f));
				
				break;
				
			case MAT_SPECULAR:
				//use only the last 3 bytes
				f->seek(chunk_length - 9);
				
				rgb[0] = (float) f->read<unsigned char>();
				rgb[1] = (float) f->read<unsigned char>();
				rgb[2] = (float) f->read<unsigned char>();

				if (mat->get(ix::specular) == Null)
				mat->specular(colour(rgb[0]/256.0f, rgb[1]/256.0f, rgb[2]/256.0f, 1.0f));

				break;				
				
			//case MAT_SHININESS:
			//no idea which values to use so ill leave this for now	
			
			case 0xa200: break;
			case MAT_TEXTURE:
				i = 0;
				do {
					//f->read((char*)&lchar, 1);
					f->read(lchar);
					buffer[i] = lchar;
					i++;
				} while (lchar != 0 && i < 20);	
				
				sprintf(texFile, "%s%s", tdir, buffer);
				
				//wgd::cout << "Texture found: " << texFile << "\n";
				tex = mat->texture();
				if (tex == 0) {
					tex = new Texture();
					mat->texture(tex);
					tex->file(new LocalFile(texFile));
				}
				
				break;			
				
				
			case EDIT3DS: break;	//Chunk whose subchunks contain objects
			case EDITOBJECT:	//An object which contains vertex and polygon data
				
				//Submit current mesh here - if it contains anything
				if(nFaces>0) {
					processMesh(meshindex++, mName, nFaces, faces, vertices, texCoords);
					texCoords = NULL;
					vertices = NULL;
				}
				
				i = 0;
				do {
					f->read(lchar);
					if(lchar==32) lchar='_';
					name[i] = lchar;
					i++;
				} while (lchar != 0 && i < 50);
				
				break;
			
			case OBJ_TRIMESH: break;//The triangle mesh
			case TRI_VERTEXL:	//List of vertices
				f->read(qty);
				vertices = new vector3d[qty];
				for (i=0; i<qty; i++) {
					f->read(vertices[i].x);
					f->read(vertices[i].y);
					f->read(vertices[i].z);
				}
				break;
					
			case TRI_FACEL1:	//List of triangle faces
				f->read(nFaces);
				faces = new unsigned short[nFaces*3];
				
				for (i = 0; i<nFaces*3; i+=3)	{
					f->read(faces[i]);
					f->read(faces[i+1]);
					f->read(faces[i+2]);
					f->read(face_flags);
				}
				break;
			
			case FACE_MATERIAL:
				
				//get material name
				i = 0;
				do {
					//f->read((char*)&lchar, 1);
					f->read(lchar);
					if(lchar==32) lchar='_';
					mName[i] = lchar;
					i++;
				} while (lchar != 0 && i < 50);				
				
				
				//skip the rest of this block
				f->seek(chunk_length - 6 - i);
				
				break;
				
					
			case MAPPING_COORDS:	//Texture mapping coordinates
				f->read(qty);
				texCoords = new vector2d[qty];
				
				for (i=0; i<qty; i++) {
					f->read(texCoords[i].x);
					f->read(texCoords[i].y);
				}
				
				break;
				

			default: //skip chunk
				f->seek(chunk_length - 6);		
									
		}
		
		f->peek();
	}
	f->close();	
	if(nFaces>0) processMesh(meshindex++, mName, nFaces, faces, vertices, texCoords);
	return;
}


void Model3DS::processMesh(int index, char *material, int nFaces, unsigned short *faces, vector3d *vertices, vector2d *tex) {
	
	//make vertex arrays
	meshVertex *vx = new meshVertex[nFaces*3];
	
	for(int i=0; i<nFaces*3; i++){
		vx[i].position.x = vertices[faces[i]].x;
		vx[i].position.y = vertices[faces[i]].y;
		vx[i].position.z = vertices[faces[i]].z;
		
		if(tex!=NULL) {
			vx[i].texcoords[0].u = tex[faces[i]].x;
			vx[i].texcoords[0].v = tex[faces[i]].y;
		} else {
			vx[i].texcoords[0].u=0;
			vx[i].texcoords[0].v=0;
		}
	}
	
	//create mesh
	Mesh *m = createMesh();
	
	m->vertices(nFaces * 3, vx);
	m->material(material);
	m->calcNormals();
	m->calcTangents();
	
	
	if(tex!=NULL) delete [] tex; tex=NULL;
	delete [] vertices; vertices=NULL;
}

