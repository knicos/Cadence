#include <wgd/scene3d.h>
#include <wgd/index.h>
#include <cadence/doste/doste.h>
#include <wgd/iterator.h>
#include <cadence/dmsg.h>
#include <wgd/camera3d.h>
#include <wgd/render.h>

#include <GL/gl.h>


using namespace wgd;
using namespace cadence;
using namespace cadence::doste;


//Events
namespace wgd {
	OnEvent(Scene3D, evt_add){
		//instance added
		//add(value);
		//Iterate over all and add.
		//clear();
		m_doclear = true;
		//OID inst = get("instances");
		//for (OID::iterator i=inst.begin(); i!=inst.end(); i++) {
		//	add(inst.get(*i));
		//}
		//m_built=true;
		//build();
	}
	IMPLEMENT_EVENTS(Scene3D, Scene);
};



Scene3D::Scene3D(const cadence::doste::OID &id) : Scene(id), m_built(false), m_doclear(false) {
	registerEvents();
}

Scene3D::~Scene3D(){
	clear();
}

void Scene3D::build(){
	if(m_built) return;
	//wgd::cout << wgd::DEV << "Building Scene3D " << getID() << "\n";
	//std::cout << "Building Scene3D\n";
	//create root node
	Node *newNode = new Node;
	newNode->key = 1;
	newNode->hasChild = 0;
	m_octree[1] = newNode;
	//getID()[ix::instances].SetTriggerable(this);  //Set up trigger to add objects when modified
	
	//Can i just do this? - [if i need to set triggerable for child objects here, need to split it up]
	Iterator<Instance3D> iter;
	iter.recursive(true);
	for(iter=get(ix::instances).begin(); iter!=get(ix::instances).end(); iter++){ //begin has been redefined by OID?? need to do something about that.
		add(*iter);
	}
	
	m_built=true;
}

void Scene3D::clear(){
	m_doclear = false;
	if(!m_built) return;
	//std::cout << "Clear Scene3D\n";
	for(std::map<uint, Node*>::iterator i=m_octree.begin(); i!=m_octree.end(); i++){
		//delete instances
		for(std::list<Instance3D*>::iterator j = i->second->instances.begin(); j!=i->second->instances.end(); j++){
			//delete *j;
		}
		i->second->instances.clear();
	}
	m_octree.clear();
	m_location.clear();
	
	//Clear lights
	for(unsigned int i=0; i<m_lights.size(); i++){
		//delete m_lights[i];
	}
	m_lights.clear();
	//not built
	m_built = false;
}

void Scene3D::add(Instance3D* inst){
	//Add lights to vector
	if(inst->isa(ix::light)){
		m_lights.push_back(inst);
		return;
	}
	//Add 3D object to octree
	if(inst->isa(ix::instance3D)){
		insert(m_octree[1], inst, inst->boundingBoxLow(), inst->boundingBoxHigh());
		return;
	}
}

wgd::uint Scene3D::morton(const vector3d &v){
	//create a morton key from a point in 3d space
	
	//fractions
	float fx=0.5, fy=0.5, fz = 0.5;
	if(width()!=0)  fx = (v.x + width()/2)  / width();
	if(height()!=0) fy = (v.y + height()/2) / height();
	if(depth()!=0)  fz = (v.z + depth()/2)  / depth();
	
	//convert to fixed point number [0.0000000000]
	uint x = (uint) ((  fx)*0x400);
	uint y = (uint) ((  fy)*0x400);
	uint z = (uint) ((  fz)*0x400);
	
	//create Morton key
	x = (x | (x<<16)) & 0xff0000ff;
	x = (x | (x<< 8)) & 0x0300f00f;
	x = (x | (x<< 4)) & 0x030c30c3;
	x = (x | (x<< 2)) & 0x09249249;
	
	y = (y | (y<<16)) & 0xff0000ff;
	y = (y | (y<< 8)) & 0x0300f00f;
	y = (y | (y<< 4)) & 0x030c30c3;
	y = (y | (y<< 2)) & 0x09249249;
	
	z = (z | (z<<16)) & 0xff0000ff;
	z = (z | (z<< 8)) & 0x0300f00f;
	z = (z | (z<< 4)) & 0x030c30c3;
	z = (z | (z<< 2)) & 0x09249249;

	//add sentinel bit and combine key
	uint key =(1<<30) + (z<<2) + (y<<1) + x;
		
	return key;
}

wgd::uint Scene3D::truncate(int depth, uint key){
	//get key depth
	int cdepth = calcDepth(key);
	
	uint k = key;
	
	if(cdepth > depth) k = key >> ((cdepth-depth) * 3);
	
	return k;
}

int Scene3D::calcDepth(uint key){
	int d=1;
	uint tmp = key;
	while(tmp > 1){
		tmp = tmp >> 3;
		d++;	
	}
	return d;
}

wgd::uint Scene3D::insert(Node* n, Instance3D* inst, const vector3d &low, const vector3d &high, int d){
	
	int index=0;
	bool straddle=false;
	uint key=0;
	
	//Calculate which octant the instance is in
	if(low.x<n->centre.x && high.x>=n->centre.x && width()!=0 || 
	   low.y<n->centre.y && high.y>=n->centre.y && height()!=0|| 
	   low.z<n->centre.z && high.z>=n->centre.z && depth()!=0 ){
			
		straddle=true;
	} else {
		if(low.x >= n->centre.x && width() !=0) index |= 1;
		if(low.y >= n->centre.y && height()!=0) index |= 2;
		if(low.z >= n->centre.z && depth() !=0) index |= 4;
	}
	
	if(!straddle && d<=levels()){
		//add to child
		
		key = (n->key << 3) | index;
		
		//check if child exists
		if(!(n->hasChild & (1<<index))){
			
			//create child
			//wgd::cout << "Creating child node [" << index << "] at depth " << d <<"\n";
			Node *newNode = new Node;
			newNode->key = key;
			newNode->hasChild = 0;
			
			//update this node
			n->hasChild |= (1<<index);
			
			//need to set the centre
			float rx = width()  / (1<<(d+2));
			float ry = height() / (1<<(d+2));
			float rz = depth()  / (1<<(d+2));
			
			newNode->centre.x = n->centre.x + (index&1? rx: -rx);
			newNode->centre.y = n->centre.y + (index&2? ry: -ry);
			newNode->centre.z = n->centre.z + (index&4? rz: -rz);
			
			//Create new node
			m_octree[key] = newNode;
			
		}
		
		//recurse to child
		insert(m_octree[key], inst, low, high, d+1);
		
	} else {
		//straddling, or at the bottom of the tree - add here
		n->instances.push_front(inst);
		key = n->key;
		
		//save the key in instance location map so the instance knows where it is in the tree
		m_location[inst] = key;
	}
	
	return key;	
}

wgd::uint Scene3D::remove(Instance3D *inst){
	//get the node
	if(m_location.find(inst)==m_location.end()) return 0;
	uint key = m_location[inst];
	//remove instance pointer
	m_octree[key]->instances.remove(inst);
	return key;
}


void Scene3D::draw(Camera *cam){
	//HACK: rebuild the scene if it has changed
	if (m_doclear) clear();
	
	//Build the scene graph if not already done
	if(!m_built) build();
	
	
	//Hack of doom.
	Camera3D *camera = (Camera3D*)(OID)*cam;
	
	//draw the skybox stuff

	glDisable(GL_LIGHTING); //HACK
	vector3d pos = camera->position();
	glTranslatef(pos.x, pos.y, pos.z);
	m_skyboxes.draw();
	glClear(GL_DEPTH_BUFFER_BIT);
	glTranslatef(-pos.x, -pos.y, -pos.z);
	glEnable(GL_LIGHTING); //HACK

	
	//Insanely simple draw function - from octree
	for(std::map<uint, Node*>::iterator i=m_octree.begin(); i!=m_octree.end(); i++){
		for(std::list<Instance3D*>::iterator j = i->second->instances.begin(); j!=i->second->instances.end(); j++){
			//dont draw if visible is false
			if( (*j)->get(ix::visible) != OID(false) ) {
				//TODO: automatic scaling for large and distant objects.
				if ((bool)(*j)->get("skybox")) {
					(*j)->draw(m_skyboxes, camera);
				} else {
					(*j)->draw(m_graph, camera);
				}
			}
		}
	}
	
	m_graph.draw();
	
	Render::end(); //depricated
	
}
