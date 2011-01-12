#include <wgd/clipmap.h>
#include "collision.h"
#include <vector>
#include <algorithm>

using namespace wgd;

namespace wgd {
	OnEvent(ClipMap, evt_build) {
		if((bool)model()->get("loaded")) build();
	}
	OnEvent(ClipMap, evt_scale) { calcLocal(); }
	OnEvent(ClipMap, evt_position) { calcLocal(); }
	OnEvent(ClipMap, evt_orientation) { calcLocal(); }
	OnEvent(ClipMap, evt_cache) { m_built = false; }
	IMPLEMENT_EVENTS(ClipMap, cadence::Agent);
};

ClipMap::ClipMap(const wgd::OID &id) : Agent(id), m_built(false), m_vertex(0), m_mesh(0) {
	if(get(ix::scale)==Null) scale(vector3d(1.0f, 1.0f, 1.0f));
	memset(m_tree.n, 0, 8 * sizeof(Node*));	//initialise tree
	registerEvents(); 
}

ClipMap::ClipMap(wgd::Model* mdl, int d, bool csh) : Agent(), m_built(false), m_vertex(0), m_mesh(0) {
	memset(m_tree.n, 0, 8 * sizeof(Node*));	//initialise tree
	scale(vector3d(1.0f, 1.0f, 1.0f));
	model(mdl);
	depth(d);
	cache(csh);
	registerEvents();
}

ClipMap::~ClipMap() {
	if(m_vertex) delete [] m_vertex;
	if(m_mesh) delete [] m_mesh;
}

void ClipMap::calcLocal() {
	matrix t, r, s, x;
	t.translate(position());
	r.rotate(orientation());
	s.scale(scale());
	matrix::fastMultiply(x, s, r);
	matrix::fastMultiply(m_localMatrix, t, x);
	m_built = false; //force rebuild
}

void ClipMap::build() {
	//build octree
	Model *mdl = model();
	if(mdl) mdl->load();
	if(!mdl || !(bool)mdl->loaded()) return;
	
	//cache transformed vertices
	if(m_vertex) delete [] m_vertex;
	m_vcount=0;
	if(cache()) {
		for(int i=0; i<mdl->meshes(); i++) m_vcount += mdl->mesh(i)->vertices();
		m_vertex = new vector3d[m_vcount];
		vector3d temp;
		m_vcount=0;
		for(int i=0; i<mdl->meshes(); i++) {
			for(int j=0; j<mdl->mesh(i)->vertices(); j++) {
				memcpy(&temp, &mdl->mesh(i)->data()[j].position.x, 3 * sizeof(float));
				matrix::fastMultiply(m_vertex[m_vcount++], m_localMatrix, temp);
			}
		}
		
		//first calculate AABB
		m_high = m_vertex[0];
		m_low  = m_vertex[0];
		for(int i=0; i<m_vcount; i++) {
			if(m_vertex[i].x > m_high.x) m_high.x = m_vertex[i].x;
			if(m_vertex[i].y > m_high.y) m_high.y = m_vertex[i].y;
			if(m_vertex[i].z > m_high.z) m_high.z = m_vertex[i].z;
			if(m_vertex[i].x < m_low.x)  m_low.x = m_vertex[i].x;
			if(m_vertex[i].y < m_low.y)  m_low.y = m_vertex[i].y;
			if(m_vertex[i].z < m_low.z)  m_low.z = m_vertex[i].z;
		}
	} else {
		//cache mesh pointers
		m_vertex = 0;
		m_meshes = mdl->meshes();
		if(m_mesh) delete [] m_mesh;
		m_mesh = new Mesh*[m_meshes];
		for(int i=0; i<m_meshes; i++) m_mesh[i] = mdl->mesh(i);
		
		//calculate AABB directly from model vertices
		for(int i=0; i<m_meshes; i++) {
			if(i==0) {
				memcpy(&m_high, &m_mesh[i]->data()[0].position.x, sizeof(float) * 3);
				memcpy(&m_low,  &m_mesh[i]->data()[0].position.x, sizeof(float) * 3);		
			}
			for(int j=0; j<m_mesh[i]->vertices(); j++) {
				vector_selector<3> &v = m_mesh[i]->data()[j].position;
				if(v.x > m_high.x) m_high.x = v.x;
				if(v.y > m_high.y) m_high.y = v.y;
				if(v.z > m_high.z) m_high.z = v.z;
				if(v.x < m_low.x) m_low.x = v.x;
				if(v.y < m_low.y) m_low.y = v.y;
				if(v.z < m_low.z) m_low.z = v.z;
			}
		}
		
		//save inverse transform matrix
		//this wont work properly with spheres if a non-uniform scale is used
		//as it transforms the test input, and sphere radius is fixed
		
		matrix::fastInverse(m_inverseMatrix, m_localMatrix);
	}
	
	//setup octree
	m_depth = depth();
	NodeData data;
	clearTree(m_tree);
	m_tree.centre = (m_high + m_low) / 2;
	m_tree.size = m_high - m_low;
	m_tree.depth = 0;
	
	
	//loop through cached vertices
	if(cache()) {
		for(int i=0; i<m_vcount; i+=3) {
			data.mesh=0;
			data.polygon = i;
			addTree(m_tree, data);
		}
	}

	m_built = true;
}

void ClipMap::clearTree(Node &n) {
	n.data.clear();
	for(int i=0; i<8; i++) if(n.n[i]) clearTree(*n.n[i]);
	memset(n.n, 0, 8 * sizeof(Node*));	//clear child pointers
}

ClipMap::Node &ClipMap::addNode(Node &n, int index) {
	if(!n.n[index]) {
		vector3d s2 = n.size / 2;			//half size
		n.n[index] = new Node();	//create node
		memset(n.n[index]->n, 0, 8 * sizeof(Node*));	//clear child pointers
		n.n[index]->size = s2;				//node size
		n.n[index]->depth = n.depth + 1;			//node depth
		n.n[index]->centre.x = n.centre.x + (index&1)? s2.x: -s2.x;	//calculate centre
		n.n[index]->centre.y = n.centre.y + (index&2)? s2.y: -s2.y;	// ..
		n.n[index]->centre.z = n.centre.z + (index&4)? s2.z: -s2.z;	// ..
	}
	return *n.n[index];
}

void ClipMap::addTree(Node &n,  NodeData &data) {
	//calculate the octant each vertex is in
	int v1=0, v2=0, v3=0, straddle=0;
	if(m_vertex[data.polygon].x > n.centre.x) v1 |= 1;
	if(m_vertex[data.polygon].y > n.centre.y) v1 |= 2;
	if(m_vertex[data.polygon].z > n.centre.z) v1 |= 4;
	
	if(m_vertex[data.polygon+1].x > n.centre.x) v2 |= 1;
	if(m_vertex[data.polygon+1].y > n.centre.y) v2 |= 2;
	if(m_vertex[data.polygon+1].z > n.centre.z) v2 |= 4;
	
	if(m_vertex[data.polygon+2].x > n.centre.x) v3 |= 1;
	if(m_vertex[data.polygon+2].y > n.centre.y) v3 |= 2;
	if(m_vertex[data.polygon+2].z > n.centre.z) v3 |= 4;
	
	//calculate straddle value
	if((v1&1) != (v2&1) || (v2&1) != (v3&1)) straddle |= 1;
	if((v1&2) != (v2&2) || (v2&2) != (v3&2)) straddle |= 2;
	if((v1&4) != (v2&4) || (v2&4) != (v3&4)) straddle |= 4;
	
	
	//in a quadrant
	if(!straddle && n.depth<m_depth) {
		//recurse to child node - addNode creates it if it is null
		addTree(addNode(n, v1), data);
		
	} else {
		//do some clever stuff - add to two child nodes if only straddling them
		//this will cause the polygon to be tested multiple times, but the benefits are 
		//impressive with the level model
		int x = v1 | straddle;
		if(n.depth >= 3) straddle=0; //dont do this further down the tree
		if(straddle==1) {
			addTree(addNode(n, x), data);
			addTree(addNode(n, x-1), data);
		} else if(straddle==2) {
			addTree(addNode(n, x), data);
			addTree(addNode(n, x-2), data);
		} else if(straddle==4) {
			addTree(addNode(n, x), data);
			addTree(addNode(n, x-4), data);
		} else {
			//stick it here
			n.data.push_back(data);
		}
	}
}


//ray collision
int ClipMap::test(const vector3d &pa, const vector3d &pb, vector3d &hit, vector3d &normal) {
	if(!m_built) build();
	if(!m_built) return 0;
	
	//AABB test - need to know if it intersects, not where exactly...
	if(!TestSegmentAABB(pa, pb, m_low, m_high)) return 0;
	
	if(m_vcount) {
		/*
		//test with octree
		std::vector<CData> data;
		m_tests = testTree(m_tree, pa, pb, data, vector3d::distanceSquared(pa, pb));
		if(data.empty()) return 0;
		
		//extract hits - return closest one for now
		float clen = data[0].d+1;
		for(unsigned int i=0; i<data.size(); i++) {
			if(data[i].d < clen) {
				memcpy(&hit.x, data[i].p, 3 * sizeof(float));
				memcpy(&normal.x, data[i].n, 3 * sizeof(float));
				clen = data[i].d;
			}
		}
		return data.size();
		*/
		
		
		//Test with all triangles
		int hits = 0;
		float dist = 9999999;
		vector3d r,n;
		for(int i=0; i<m_vcount; i+=3) {
			if( intersectLineTriangle(pa,pb, m_vertex[i],m_vertex[i+1],m_vertex[i+2], r,n) ) {
				//check if it is the closest hit
				float d = pa.distanceSquared(r);
				if(d < dist) {
					hit = r;
					//get normal from polygon
					normal = n;
					dist = d;
				}
				hits++;
			}
		}
		m_tests = m_vcount / 3; //testing all polys
		return hits;
	} else {
		//transform input
		vector3d ta, tb;
		matrix::fastMultiply(ta, m_inverseMatrix, pa);
		matrix::fastMultiply(tb, m_inverseMatrix, pb);
	
		//non cached mode - use model vertex data
		int hits = 0;
		float dist = 9999999;
		vector3d r,n;
		for(int j = 0; j<m_meshes; j++) {
		
			vector3d a,b,c,r,n;
			for(int i=0; i<m_mesh[j]->vertices(); i+=3) {
				memcpy(&a.x, &m_mesh[j]->data()[i+0].position.x, sizeof(float) * 3);
				memcpy(&b.x, &m_mesh[j]->data()[i+1].position.x, sizeof(float) * 3);
				memcpy(&c.x, &m_mesh[j]->data()[i+2].position.x, sizeof(float) * 3);
				
				if( intersectLineTriangle(ta,tb, a,b,c, r,n) ) {
					//check if it is the closest hit
					float d = pa.distanceSquared(r);
					if(d < dist) {
						matrix::fastMultiply(hit, m_localMatrix, r); //hit = r;
						//get normal from polygon
						matrix::fastMultiply(normal, m_localMatrix, n); //normal = n;
						dist = d;
					}
					hits++;
				}
				
			}
		}
		m_tests = m_vcount / 3; //testing all polys
		return hits;
	}
}

int ClipMap::testTree(Node &n, const wgd::vector3d &a, const wgd::vector3d &b, std::vector<CData> &data, float length) {
	//test AABB, then call ALL its children
	if(!TestSegmentAABB(a, b, m_low, m_high)) return 0;
	
	//do tests on this node's data
	if(!n.data.empty()) {
		int p;
		vector3d r,nm;
		for(unsigned int i=0; i<n.data.size(); i++) {
			p = n.data[i].polygon;
			if( intersectLineTriangle(a,b, m_vertex[p],m_vertex[p+1],m_vertex[p+2], r,nm) ) {
				float d = a.distanceSquared(r);
				if(d <= length) data.push_back(CData(r, nm, d));
			}
		}
	}
	
	int tests = n.data.size();
	
	//recurse to children
	for(int i=0; i<8; i++) {
		if(n.n[i]) tests += testTree(*n.n[i], a, b, data, length);
	}
	
	return tests;
}

//sphere collision
int ClipMap::test(const wgd::vector3d &centre, float radius, wgd::vector3d *point, wgd::vector3d *normal, int max) {
	if(!m_built) build();
	if(!m_built) return 0; //build failed
	
	//first, quickly test AABBs
	if(centre.x+radius < m_low.x || centre.y+radius < m_low.y || centre.z+radius < m_low.z) return 0;
	if(centre.x-radius > m_high.x || centre.y-radius > m_high.y || centre.z-radius > m_high.z) return 0;
	
	//temportaty collision data
	std::vector<CData> data;
	
	//recursively test octree
	if(m_vcount) {
		m_tests =  testTree(m_tree, centre, radius, data);
		
	} else if(m_mesh) {
		//transform input
		vector3d ctr = m_inverseMatrix * centre;
		float rd = radius; //scale().length() * radius;
		
		//if no cache, test all meshes
		m_tests = 0;
		for(int j = 0; j<m_meshes; j++) {
			vector3d a,b,c,r, r2;
			for(int i=0; i<m_mesh[j]->vertices(); i+=3) {
				memcpy(&a.x, &m_mesh[j]->data()[i+0].position.x, sizeof(float) * 3);
				memcpy(&b.x, &m_mesh[j]->data()[i+1].position.x, sizeof(float) * 3);
				memcpy(&c.x, &m_mesh[j]->data()[i+2].position.x, sizeof(float) * 3);
				
				r = ClosectPointTriangle(ctr, a,b,c);
				//check if it within the collision radius
				float d = ctr.distanceSquared(r);
				if(d < rd*rd) {
					//convert back to world coordinates
					matrix::fastMultiply(r2, m_localMatrix, r);
					//insert into array in order of decreasing d
					data.push_back(CData(r2, (centre - r2).normalise(), d));
				}
			}
			m_tests += m_mesh[j]->vertices() / 3;
		}
	}
	
	//sort list
	std::sort(data.begin(), data.end());
	
	//copy data to return variables
	int i=0;
	for(std::vector<CData>::iterator iter=data.begin(); iter!=data.end(); iter++, i++) {
		if(i==max) return max; //stop if we are over the array size
		memcpy(&point[i].x, iter->p, 3 * sizeof(float));
		memcpy(&normal[i].x, iter->n, 3 * sizeof(float));
	}
	
	return data.size();
}

int ClipMap::testTree(Node &n, const wgd::vector3d &centre, float radius, std::vector<CData> &data) {
	int tests=0; //debug - count tests
	
	//chose child nodes to recurse to
	int index = 0, straddle=0;
	if(centre.x > n.centre.x) index |= 1;
	if(centre.y > n.centre.y) index |= 2;
	if(centre.z > n.centre.z) index |= 4;
	
	//are we in multiple child nodes?
	if(centre.x-radius <= n.centre.x && centre.x+radius > n.centre.x) straddle |= 1;
	if(centre.y-radius <= n.centre.y && centre.y+radius > n.centre.y) straddle |= 2;
	if(centre.z-radius <= n.centre.z && centre.z+radius > n.centre.z) straddle |= 4;
	
	if(straddle) {
		//is there a nice way of limiting the straddle testing - meh - do them all for now
		for(int i=0; i<8; i++) {
			if(n.n[i]) tests += testTree(*n.n[i], centre, radius, data);
		}
	} else {
		if(n.n[index]) tests += testTree(*n.n[index], centre, radius, data);
	}
	
	//do the actual testing
	if(!n.data.empty()) {
		vector3d a,b,c,r;
		for(unsigned int i=0; i<n.data.size(); i++) {
			int p = n.data[i].polygon;
			r = ClosectPointTriangle(centre, m_vertex[p], m_vertex[p+1], m_vertex[p+2]);
			
			//check if it within the collision radius
			float d = centre.distanceSquared(r);
			if(d < radius*radius) {
				//insert into array in order of decreasing d
				data.push_back(CData(r, (centre - r).normalise(), d));
			}
		}
		tests += n.data.size();
	}
	
	return tests;
}
