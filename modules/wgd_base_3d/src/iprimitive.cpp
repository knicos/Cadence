
#include <wgd/iprimitive.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <wgd/texture.h>
#include <wgd/shader.h>
#include <wgd/math.h>

#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#endif

using namespace wgd;
using namespace cadence;
using namespace cadence::doste;

wgd::IPrimitive3D::IPrimitive3D() : Instance3D(){
	set(ix::type, type());
	if (get(ix::width) == Null)
		width(1.0);
	if (get(ix::height) == Null)
		height(1.0);
	if (get(ix::depth) == Null)
		depth(1.0);
	if (get(ix::primitive) == Null)
		primitive(ix::cube);
}
wgd::IPrimitive3D::IPrimitive3D(const cadence::doste::OID &inst) : Instance3D(inst){
	if (get(ix::width) == Null)
		width(1.0);
	if (get(ix::height) == Null)
		height(1.0);
	if (get(ix::depth) == Null)
		depth(1.0);
	if (get(ix::primitive) == Null)
		primitive(ix::cube);
}


void wgd::IPrimitive3D::size(float w, float h, float d){
	width(w);
	height(h);
	depth(d);
}

void wgd::IPrimitive3D::draw(SceneGraph &graph, Camera3D *camera) {
	Instance3D::draw(graph, camera);
	localMatrix().apply();

	
	
	Material *mat = material();
	if (mat != 0) mat->bind();
		
	//test for shader - to add tangent only
	//Shader *s = Shader::current();
	//if(s!=NULL && !s->tangents()) s=NULL;
	Shader *s = 0;
 
	if (primitive() == OID("cube")) {

		float w2 = width()/2.0f;
		float h2 = height()/2.0f;
		float d2 = depth()/2.0f;

		glBegin(GL_QUADS);
			
			glNormal3f(0.0, 0.0, -1.0);
			if(s) s->setVariable("tangent", -1.0, 0.0, 0.0);
			glTexCoord2f(1.0f, 0.0f);   glVertex3f(-w2, -h2, -d2);
			glTexCoord2f(1.0f, 1.0f);   glVertex3f(-w2, h2, -d2);
			glTexCoord2f(0.0f, 1.0f);   glVertex3f( w2, h2, -d2);
			glTexCoord2f(0.0f, 0.0f);   glVertex3f( w2, -h2, -d2);
			
			if(s) s->setVariable("tangent", 1.0, 0.0, 0.0);
			glNormal3f(0.0, 0.0, 1.0);
			glTexCoord2f(1.0f, 0.0f);   glVertex3f( w2, -h2, d2);
			glTexCoord2f(1.0f, 1.0f);   glVertex3f( w2, h2, d2);
			glTexCoord2f(0.0f, 1.0f);   glVertex3f(-w2, h2, d2);
			glTexCoord2f(0.0f, 0.0f);   glVertex3f(-w2, -h2, d2);			
			
			if(s) s->setVariable("tangent", 0.0, 0.0, -1.0);
			glNormal3f(1.0, 0.0, 0.0);
			glTexCoord2f(1.0f, 0.0f);   glVertex3f( w2, -h2, -d2);
			glTexCoord2f(1.0f, 1.0f);   glVertex3f( w2, h2, -d2);
			glTexCoord2f(0.0f, 1.0f);   glVertex3f( w2, h2, d2);
			glTexCoord2f(0.0f, 0.0f);   glVertex3f( w2, -h2, d2);
			
			if(s) s->setVariable("tangent", 0.0, 0.0, 1.0);
			glNormal3f(-1.0, 0.0, 0.0);
			glTexCoord2f(1.0f, 0.0f);   glVertex3f( -w2, -h2, d2);
			glTexCoord2f(1.0f, 1.0f);   glVertex3f( -w2, h2, d2);
			glTexCoord2f(0.0f, 1.0f);   glVertex3f( -w2, h2, -d2);
			glTexCoord2f(0.0f, 0.0f);   glVertex3f( -w2, -h2, -d2);
			
			glNormal3f(0.0, 1.0, 0.0);
			if(s) s->setVariable("tangent", 1.0, 0.0, 0.0);
			glTexCoord2f(1.0f, 0.0f);   glVertex3f( w2, h2, d2);
			glTexCoord2f(1.0f, 1.0f);   glVertex3f( w2, h2, -d2);
			glTexCoord2f(0.0f, 1.0f);   glVertex3f( -w2, h2, -d2);
			glTexCoord2f(0.0f, 0.0f);   glVertex3f( -w2, h2, d2);
			
			glNormal3f(0.0, -1.0, 0.0);
			glTexCoord2f(1.0f, 0.0f);   glVertex3f( w2, -h2, -d2);
			glTexCoord2f(1.0f, 1.0f);   glVertex3f( w2, -h2, d2);
			glTexCoord2f(0.0f, 1.0f);   glVertex3f(-w2, -h2, d2);
			glTexCoord2f(0.0f, 0.0f);   glVertex3f(-w2, -h2, -d2);			
		glEnd();
		
	}
	
	if (primitive() == OID("sphere")) {
		
		float r=radius();
		//calculate radius from MIN(width,height,depth) if not set
		if(r==0.0){
			r=width()/2.0f;
			if(height()/2.0<r) r=height()/2.0f;
			if(depth()/2.0<r) r=depth()/2.0f;
			radius(r);
		};
		
		//minimum slices = 1, minimum sides = 3
		if(sides()<3) sides(3);
		int nSlices = sides()-2;
		int nSides = sides();	

		float step = 2*wgd::PI / nSides;
		float ystep = wgd::PI / (nSlices+1);
		
		vector3d *ring1 = new vector3d[nSides];
		vector3d *ring2 = new vector3d[nSides];

		float py = sin(ystep+wgd::PI/2)*r;
		float dist = cos(ystep+wgd::PI/2) * r;

		//Top
		for(int i=0; i<nSides; i++){
			ring2[i].y = py;
			ring2[i].x = dist*cos(-i*step);
			ring2[i].z = dist*sin(-i*step);
		}
		
		//Draw Triangle Fan
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5, 0.0);
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, r, 0.0);
		for(int i=0; i<nSides; i++){
			glTexCoord2f((float)i/nSides, 1/(nSlices+1));
			glNormal3f(ring2[i].x/r, ring2[i].y/r, ring2[i].z/r);
			glVertex3f(ring2[i].x, ring2[i].y, ring2[i].z);
		}
		glTexCoord2f(1.0, 1/(nSlices+1));
		glNormal3f(ring2[0].x/r, ring2[0].y/r, ring2[0].z/r);
		glVertex3f(ring2[0].x,   ring2[0].y,   ring2[0].z);
		glEnd();
		
		
		//Middle
		for(int j=2; j<=nSlices; j++){
			
			//copy ring2 to ring1
			memcpy(ring1, ring2, nSides*sizeof(vector3d));
			
			//y position
			py = sin(j*ystep+wgd::PI/2)*r;
			
			//distance from center XZ
			dist = cos(j*ystep+wgd::PI/2) *r;
			
			for(int i=0; i<nSides; i++){
				ring2[i].y = py;
				ring2[i].x = dist*cos(-i*step);
				ring2[i].z = dist*sin(-i*step);
			}
			
			//draw line loop for now
			glBegin(GL_TRIANGLE_STRIP);
			for(int i=0; i<nSides; i++){
				glTexCoord2f((float)i/nSides, (float)(j-1)/(nSlices+1));
				glNormal3f(ring1[i].x/r, ring1[i].y/r, ring1[i].z/r);
				glVertex3f(ring1[i].x,   ring1[i].y,   ring1[i].z);
				
				glTexCoord2f((float)i/nSides, (float)j/(nSlices+1));
				glNormal3f(ring2[i].x/r, ring2[i].y/r, ring2[i].z/r);
				glVertex3f(ring2[i].x,   ring2[i].y,   ring2[i].z);
			}
			glTexCoord2f(1.0, (float)(j-1)/(nSlices+1));
			glNormal3f(ring1[0].x/r, ring1[0].y/r, ring1[0].z/r);
			glVertex3f(ring1[0].x,   ring1[0].y,   ring1[0].z);
			
			glTexCoord2f(1.0, (float)j/(nSlices+1));
			glNormal3f(ring2[0].x/r, ring2[0].y/r, ring2[0].z/r);
			glVertex3f(ring2[0].x,   ring2[0].y,   ring2[0].z);
			glEnd();	
		
		}

		//Bottom
		//Draw Triangle Fan
		glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5, 1.0);
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(0.0, -r, 0.0);
		for(int i=nSides-1; i>=0; i--){
			glTexCoord2f((float)i/nSides, 1-1/(nSlices+1));
			glNormal3f(ring2[i].x/r, ring2[i].y/r, ring2[i].z/r);
			glVertex3f(ring2[i].x,   ring2[i].y,   ring2[i].z);
		}
		glTexCoord2f(0.0, 1.0);
		glNormal3f(ring2[nSides-1].x/r, ring2[nSides-1].y/r, ring2[nSides-1].z/r);
		glVertex3f(ring2[nSides-1].x,   ring2[nSides-1].y,   ring2[nSides-1].z);
		glEnd();

		delete [] ring1;
		delete [] ring2;
		
		
		
	}
	
	if (primitive() == OID("cylinder")) {
		
		float rx = width()/2;
		float rz = depth()/2;
		float ht = height()/2;
		
		if(sides()<3) sides(3); //minimum 3 sides
		int nSlices = sides();
		
		vector3d *ring = new vector3d[nSlices];
		
		float step = 2*wgd::PI / nSlices;
		
		for(int i=0; i<nSlices; i++){
			ring[i].x = cos(i*step) * rx;
			ring[i].z = sin(i*step) * rz;
		}
		
		//Cylinder
		glBegin(GL_TRIANGLE_STRIP);
		for(int i=0; i<nSlices; i++){
			glTexCoord2f((float)i/nSlices, 1.0);
			glNormal3f(ring[i].x, 0.0, ring[i].z);
			glVertex3f(ring[i].x, -ht, ring[i].z);
			glTexCoord2f((float)i/nSlices, 0.0);
			glVertex3f(ring[i].x,  ht, ring[i].z);
		}
		glTexCoord2f(1.0, 1.0);
		glNormal3f(ring[0].x, 0.0, ring[0].z);
		glVertex3f(ring[0].x, -ht, ring[0].z);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(ring[0].x,  ht, ring[0].z);
		glEnd();
		
		//Bottom
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0, -1.0, 0.0);
		for(int i=0; i<nSlices; i++){
			glTexCoord2f(ring[i].x / (rx*2), ring[i].z / (rz*2));
			glVertex3f(ring[i].x, -ht, ring[i].z);
		}		
		glEnd();
		
		//Top
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0.0, 1.0, 0.0);
		for(int i=nSlices-1; i>=0; i--){
			glTexCoord2f(ring[i].x / (rx*2), ring[i].z / (rz*2));
			glVertex3f(ring[i].x, ht, ring[i].z);
		}		
		glEnd();
			
		delete [] ring;
	}

	if (mat != 0) mat->unbind();
	
	//glPopMatrix(); 
	localMatrix().unapply();
}
 
