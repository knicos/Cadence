#include <wgd/mesh.h>
#include <wgd/material.h>
#include <wgd/vector.h>
#include <wgd/extensions.h>

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

Mesh::Mesh()
	: Object(), m_weightsPerVertex(0), m_skin(NULL), m_drawable(0) {
	set("type",type());
}

Mesh::Mesh(const OID &id)
	: Object(id), m_weightsPerVertex(0), m_skin(NULL), m_drawable(0) {
	set("type",type());
}

Mesh::~Mesh(){
	if(m_skin) delete [] m_skin;
	if(m_drawable) m_drawable->destroy();
}

void Mesh::setup() {
	m_drawable = new Drawable<meshVertex>;
	m_drawable->mode = GL_TRIANGLES;
	m_drawable->clearData = false;
	m_drawable->discard = false;
}


void Mesh::vertices(int nVertices, meshVertex* vx) {
	if(!m_drawable) setup();
	m_drawable->deleteData();
	m_drawable->data = vx;
	m_drawable->size = nVertices;
}

void Mesh::skin(int wpv, MeshSkin *skin){
	if(m_skin) delete [] m_skin;
	m_weightsPerVertex = wpv;
	m_skin = skin;
}

void Mesh::create(int n){
	//setup blank mesh array
	if(!m_drawable || m_drawable->size != n){
		meshVertex *vx = new meshVertex[n];
		memset(vx, 0, n * sizeof(meshVertex));
		vertices(n, vx);
	} else {
		memset(m_drawable->data, 0, n * sizeof(meshVertex));
	}
}

void Mesh::calcTangents(){
	for(unsigned int i=0; i<m_drawable->size; i+=3){
		meshVertex *vx = m_drawable->data;
		vector3d tangent = calcTangent(vx[i], vx[i+1], vx[i+2]);
		
		for(int j=0; j<3; j++){
			vx[i+j].custom.var.x = tangent.x;
			vx[i+j].custom.var.y = tangent.y;
			vx[i+j].custom.var.z = tangent.z;
		}
	}
}


vector3d Mesh::calcTangent(const meshVertex &vA, const meshVertex &vB, const meshVertex &vC){
        vector3d vAB = vector3d(vB.position.x, vB.position.y, vB.position.z) - vector3d(vA.position.x, vA.position.y, vA.position.z);
        vector3d vAC = vector3d(vC.position.x, vC.position.y, vC.position.z) - vector3d(vA.position.x, vA.position.y, vA.position.z);
        vector3d nA  = vector3d(vA.normal.x, vA.normal.y, vA.normal.z);
       
        // Components of vectors to neighboring vertices that are orthogonal to the
        // vertex normal
        vector3d vProjAB = vAB - nA * ( vector3d::dotProduct( nA, vAB ));
        vector3d vProjAC = vAC - nA * ( vector3d::dotProduct( nA, vAC ));
       
        // tu texture coordinate differences
        float duAB = vB.texcoords[0].u - vA.texcoords[0].u;
        float duAC = vC.texcoords[0].u - vA.texcoords[0].u;
       
        // tv texture coordinate differences
        float dvAB = vB.texcoords[0].v - vA.texcoords[0].v;
        float dvAC = vC.texcoords[0].v - vA.texcoords[0].v;
       
        if( (dvAC * duAB) > (dvAB * duAC) ){
                dvAC = -dvAC;
                dvAB = -dvAB;
        }
       
        vector3d vTangent = (vProjAC * dvAB) - (vProjAB * dvAC);
        //vTangent.normalise();
        return vTangent.normalise();
}

void Mesh::calcNormals(){
        for(unsigned int i=0; i<m_drawable->size; i+=3){
               meshVertex *vx = m_drawable->data;
               
                //points
                vector3d p1 = vector3d(vx[i  ].position.x, vx[i  ].position.y, vx[i  ].position.z);
                vector3d p2 = vector3d(vx[i+1].position.x, vx[i+1].position.y, vx[i+1].position.z);
                vector3d p3 = vector3d(vx[i+2].position.x, vx[i+2].position.y, vx[i+2].position.z);
               
                //vectors
                vector3d a = p2 - p1;
                vector3d b = p3 - p1;
               
                //normal
                vector3d normal = vector3d::crossProduct(a, b);
                normal = normal.normalise();
               
                //save in the three points
                for(int j=0; j<3; j++){
                        vx[i+j].normal.x = normal.x;
                        vx[i+j].normal.y = normal.y;
                        vx[i+j].normal.z = normal.z;
                }
        }
}
