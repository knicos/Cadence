//I'll stick this in base3d for now, but it really needs to be used by scene2d and scene3d.

#include <wgd/scenegraph.h>
#include <wgd/material.h>
#include <wgd/extensions.h>
#include <algorithm>

using namespace wgd;

DrawStatus DrawableBase::status;

bool SceneGraph::draw() {
	DrawableBase::begin();
	
	//sort lists
	std::sort(m_solid.begin(), m_solid.end(), DrawableBase::compare);
	std::sort(m_alpha.begin(), m_alpha.end(), DrawableBase::compare);
	//draw lists
	for(std::vector<DrawableBase*>::iterator i= m_solid.begin(); i!=m_solid.end(); i++) {
		(*i)->draw();
		//delete data
		(*i)->queued--;
		if((*i)->clearData) (*i)->deleteData();
		if((*i)->discard) delete *i;
	}
	for(std::vector<DrawableBase*>::iterator i=m_alpha.begin(); i!=m_alpha.end(); i++) {
		(*i)->draw();
		//delete data
		(*i)->queued--;
		if((*i)->clearData) (*i)->deleteData();
		if((*i)->discard) delete *i;
	}
	//clear lists
	m_solid.clear();
	m_alpha.clear();
	
	DrawableBase::end();
	return true;
}


DrawableBase* SceneGraph::add(DrawableBase *d) {
		//detect if there is any alpha (d->material/texture (from surface) + vertex colour )
		return addSolid(d); 
}

DrawableBase* SceneGraph::addSolid(DrawableBase *d) {
		m_solid.push_back(d); 
		d->queued++;
		return d; 
}

DrawableBase* SceneGraph::addAlpha(DrawableBase *d) {
		m_alpha.push_back(d); 
		d->queued++;
		return d; 
}


// Some drawable functions //
DrawStatus::DrawStatus() : active(false), normals(0), colours(0), texcoords(0), material(0), texture(0), shader(0) {}

void DrawStatus::begin() {
	glEnableClientState(GL_VERTEX_ARRAY);	//All drawables need this
	glAlphaFunc(GL_GREATER, 0.5);
	active = true;			//drawstatus is on
	flags = 7;	//defaults for stuff
	normals = false;
	colours = false;
	texcoords = 0;
	surface = Null;
	glEnable(GL_NORMALIZE);
}
void DrawStatus::end() {
	setState(0, 0, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	setFlags(7);
	setSurface(Null);
	active = false;
}

void DrawStatus::setState(bool n, bool c, unsigned int tx) {
	//Normal array
	if(n && !normals) glEnableClientState (GL_NORMAL_ARRAY);
	if(!n && normals) glDisableClientState(GL_NORMAL_ARRAY);
	normals = n;
	//Colour array
	if(c && !colours) glEnableClientState (GL_COLOR_ARRAY);
	if(!c && colours) glDisableClientState(GL_COLOR_ARRAY);
	colours = c;
	
	//enable texture arrays
	unsigned int i;
	for(i=0; i<tx || i<texcoords; i++) {
		if(i>0) WGDglClientActiveTextureARB(GL_TEXTURE0_ARB+i);
		if(i<tx && i>=texcoords) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if(i>=tx) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	if(i>0) WGDglClientActiveTextureARB(GL_TEXTURE0_ARB);
	texcoords = tx;
}
void DrawStatus::setSurface(const OID& surf) {
	if(surf==surface) return;	//No change
	//unbind any existing stuff
	if(material) material->unbind();
	if(texture)  texture->unbind();
	
	//disable any previous shader stuff
	if(last) last->disableCustom(shader);
	
	//bind new surfce
	shader = 0;
	material = 0;
	texture = 0;
	if(surf!=Null) {
		//warn user if they haven't dereferenced their material/texture pointers
		if(surf.isPointer()) {
			std::cout << "Warning: Pointer found for drawable surface\n";
			return;
		}
		if(surf.get(ix::type) == OID("Material")) {
			material = surf;
			shader = material->shader();
			material->bind();
		} else if(surf.get(ix::type) == OID("Texture")) {
			texture = surf;
			texture->bind();
		}
	}
	//store new surface
	surface = surf;
}
void DrawStatus::setFlags(unsigned char f) {
	//Lighting
	if((f & LIGHTING) && !(flags & LIGHTING)) glEnable(GL_LIGHTING);
	if(!(f & LIGHTING) && (flags & LIGHTING)) glDisable(GL_LIGHTING);

	//Depth Test
	if((f & DEPTH_TEST) && !(flags & DEPTH_TEST)) glEnable(GL_DEPTH_TEST);
	if(!(f & DEPTH_TEST) && (flags & DEPTH_TEST)) glDisable(GL_DEPTH_TEST);
	
	//Depth mask (write)
	if((f & DEPTH_WRITE) && !(flags & DEPTH_WRITE)) glDepthMask(1);
	if(!(f & DEPTH_WRITE) && (flags & DEPTH_WRITE)) glDepthMask(0);
	
	//Alpha testing
	if((f & ALPHA_TEST) && !(flags & ALPHA_TEST)) glEnable(GL_ALPHA_TEST);
	if(!(f & ALPHA_TEST) && (flags & ALPHA_TEST)) glDisable(GL_ALPHA_TEST);
	
	//Wireframe
	if((f & WIREFRAME) && !(flags & WIREFRAME)) glPolygonMode(GL_FRONT, GL_LINE);
	if(!(f & WIREFRAME) && (flags & WIREFRAME)) glPolygonMode(GL_FRONT, GL_FILL);
	
	//Double sided polygons
	if((f & DOUBLE_SIDED) && !(flags & DOUBLE_SIDED)) glDisable(GL_CULL_FACE);
	if(!(f & DOUBLE_SIDED) && (flags & DOUBLE_SIDED)) glEnable(GL_CULL_FACE);
	
	flags = f;
}

void DrawStatus::reset() {
	setState(0,0,0);
	glDisableClientState(GL_VERTEX_ARRAY);
	setSurface(Null);
	flags = (DrawFlags)7;
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glDisable(GL_ALPHA_TEST);
	glPolygonMode(GL_FRONT, GL_FILL);
	active = false;
}

