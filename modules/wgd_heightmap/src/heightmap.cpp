#include <wgd/heightmap.h>
#include <wgd/camera3d.h>

#include <GL/gl.h>
#include <wgd/extensions.h>
#include <wgd/math.h>

#ifdef LINUX
template <typename T>
T max(T a, T b) { return (a>b) ? a : b; }
#endif

using namespace wgd;

//Code for the height map regions


HMRegion::HMRegion(HMData *data, HMPadding *padding, int size) : m_VBO(0), m_patchSize(0), m_tree(0), m_patches(0), m_spatCoord(0) {
	
	//create vertex array
	m_size = size;
	m_vertices = new vertex[data->size];
	for(int i=0; i<data->size; i++){
		m_vertices[i].x = (float) (i % (size+1));
		m_vertices[i].y = data->heights[i];
		m_vertices[i].z = (float) (i / (size+1));
		
		//regionwide texture coordinates
		m_vertices[i].u = m_vertices[i].x / size;
		m_vertices[i].v = m_vertices[i].z / size;
	}
	delete [] data->heights;
	
	calculateNormals( padding );
	
	//Save textures
	m_baseMaterial = data->material;
	m_splatMaterial = data->materials;
	m_splatCount = data->nMat;
	
	//build VBO
	if (Extensions::hasVBO()) {
		WGDglGenBuffersARB( 1, &m_VBO);
		WGDglBindBufferARB( GL_ARRAY_BUFFER_ARB, m_VBO );
		WGDglBufferDataARB( GL_ARRAY_BUFFER_ARB, data->size * sizeof(vertex), m_vertices, GL_STATIC_DRAW_ARB );
		WGDglBindBufferARB( GL_ARRAY_BUFFER_ARB, 0);
	}
}

HMRegion::~HMRegion(){
	int count = m_size/m_patchSize;
	for(int i=0; i<count*count; i++) delete [] m_patches[i].indices;
	delete [] m_vertices;
	delete [] m_patches;
	delete [] m_tree;
	
	if(m_VBO) WGDglDeleteBuffersARB(1, &m_VBO);
}

void HMRegion::setup(int patchSize, const vector2d &splatScale){
	if(m_patchSize == patchSize) return; //already setup
	m_patchSize = patchSize;
	
	//create some of these patch thingys
	int count = (m_size/patchSize) * (m_size/patchSize);
	m_patches = new Patch[count];
	
	m_levels = levels(patchSize);
	
	for(int i=0; i<count; i++){
		m_patches[i].nIndices = 0;
		m_patches[i].level = 1;
		
		//position in vertex coordinates
		int x = (i * patchSize) % m_size;
		int y = (i * patchSize) / m_size * patchSize;
		//start position in vertex array
		m_patches[i].offset = x + y*(m_size+1);
		
		//setup index array as maximum size
		int max = patchSize * patchSize * 6;
		m_patches[i].indices = new unsigned int[max];
		
		m_patches[i].glue = 0;
		m_patches[i].level = 0;
		m_patches[i].changed = true; //make sure it builds the first time round
		
		//calculate error at each mip level
		m_patches[i].error = new float[m_levels];
		for(int l=0; l<m_levels; l++){
			if(l==0) m_patches[i].error[l] = 0;
			else m_patches[i].error[l] = calcError(patchSize, x, y, l);
		}
		

	}
	
	//initialise quadtree data
	int nodes = nodeCount(patchSize);
	m_tree = new HMNode[nodes];
	memset(m_tree, 0, nodes * sizeof(HMNode));
	
	//calculate Min and Max heights for AABB
	calcBoxR(0, 0, 0, m_size);
		
	//create texture splat coordinates if there are splat textures
	if(m_splatCount>0){
		int nVertices = (m_size+1)*(m_size+1);
		m_spatCoord = new GLTexCoord[nVertices];
		for(int i=0; i<nVertices; i++){
			m_spatCoord[i].u = m_vertices[i].x / splatScale.x;
			m_spatCoord[i].v = m_vertices[i].z / splatScale.y;
		}
		
		//process which splat textures are used in each quadtree node
	}
}

float HMRegion::height(int x, int y) const {
	if(x>m_size) x=m_size;
	if(y>m_size) y=m_size;
	int loc = x + y*(m_size+1);
	return m_vertices[loc].y;
}

vector3d HMRegion::normal(int x, int y) const {
	int loc = x + y*(m_size+1);
	return vector3d(&m_vertices[loc].nx);
}

int HMRegion::nodeCount(int patchSize){
	int i=1, count=1;
	int size = m_size;
	while(size > patchSize) {
		i <<= 2;	//multiply by 4 for each level
		count += i;
		size >>= 1;	//divide by 2 for next level
	}
	return count;
}

int HMRegion::levels(int patchSize){
	int i=0;
	while(patchSize > 0) {
		i++;
		patchSize >>= 1;
	}
	return i;
}

float HMRegion::calcError(int patchSize, int x, int y, int level){
	int step = 1<<level;
	float h[4], leftStep, rightStep, finalStep;
	float leftHeight, rightHeight, levelHeight, actualHeight;
	float error, maxError=0;
	
	for(int j=0; j<patchSize; j+=step){

		for(int i=0; i<patchSize; i+=step){
			//heightmap y coordinates for this level
			h[0] = height(x+i,			y+j);
			h[1] = height(x+i,			y+j + step);
			h[2] = height(x+i + step,	y+j);
			h[3] = height(x+i + step,	y+j + step);
			
			//left and right interpolation steps
			leftStep = (h[1] - h[0]) / step;
			rightStep = (h[2] - h[3]) / step;

			for(int jj=0; jj<step; jj++){

				//left and right interpolated height values
				leftHeight  = h[0] + jj * leftStep;
				rightHeight = h[3] + jj * rightStep;

				//actual interpolation step
				finalStep = (rightHeight - leftHeight) / step;
				for(int ii=0; ii<step; ii++){

					//final point
					levelHeight = leftHeight + ii * finalStep;
					actualHeight = height(x + i+ii, y + j+jj);

					//error at this vertex
					error = fabs(levelHeight - actualHeight);

					//maximum error
					maxError = max(maxError, error);
				}
			}
		}
	}
	return maxError;
}

void HMRegion::calculateNormals( HMPadding *padding ){
	vector3d v, v1, v2, v3, v4;
	int width = m_size+1;
	for(int i=0; i<width*width; i++){
		//get surrounding points
		v  = vector3d(0.0, m_vertices[i].y, 0.0);
		
		//left and right 
		if(i % width == 0) {
			if(padding && padding->left)  v1 = vector3d(-1.0, padding->left[i / width], 0.0);
			v2 = vector3d(-1.0, m_vertices[i+1].y, 0.0);
		} else if(i % width >= m_size) {
			v1 = vector3d(-1.0, m_vertices[i-1].y, 0.0);
			if(padding && padding->right) v2 = vector3d( 1.0, padding->right[i / width], 0.0);
		} else {
			v1 = vector3d(-1.0, m_vertices[i-1].y, 0.0);
			v2 = vector3d( 1.0, m_vertices[i+1].y, 0.0);
		}
		
		//up and down
		if(i / width == 0) {
			if(padding && padding->top)  v3 = vector3d(0.0, padding->top[i % width], -1.0);
			v4 = vector3d(0.0, m_vertices[i+width].y, 1.0);
		} else if(i / width >= m_size) {
			v3 = vector3d(0.0, m_vertices[i-width].y, -1.0);
			if(padding && padding->bottom) v4 = vector3d(0.0, padding->bottom[i % width], 1.0);
		} else {
			v3 = vector3d(0.0, m_vertices[i-width].y, -1.0);
			v4 = vector3d(0.0, m_vertices[i+width].y, 1.0);
		}
		
		//calculate four normals
		vector3d n1, n2, n3, n4;
		n1 = vector3d::crossProduct(v1-v, v2-v);
		n2 = vector3d::crossProduct(v2-v, v3-v);
		n3 = vector3d::crossProduct(v3-v, v4-v);
		n4 = vector3d::crossProduct(v4-v, v1-v);
		
		//clear data if a vertex is undefined
		if(n1.y>=0.0){ n1.x=0; n1.y=0; n1.z=0; }
		if(n2.y>=0.0){ n2.x=0; n2.y=0; n2.z=0; }
		if(n3.y>=0.0){ n3.x=0; n3.y=0; n3.z=0; }
		if(n4.y>=0.0){ n4.x=0; n4.y=0; n4.z=0; }
		
		//get average normal
		v = (n1+n2+n3+n4).normalise();
		
		m_vertices[i].nx = -v.x;
		m_vertices[i].ny = -v.y;
		m_vertices[i].nz = -v.z;
		
		//Tangents are relatively simple (may be backwards)
		vector3d tangent = vector3d::crossProduct(vector3d(0.0, 0.0, 1.0), v) ;
		m_vertices[i].tx = tangent.x;
		m_vertices[i].ty = tangent.y;
		m_vertices[i].tz = tangent.z;
	}
}

vector2d HMRegion::calcBoxR(int node, int x, int y, int size){
	vector2d box, tmp; //	x = min, y = max;
	
	int halfSize = size >> 1;
	if(size > m_patchSize){
		//recurse through quadtree if not a leaf node to get higher level boxes
		box = calcBoxR((node << 2) + 1, x,		y + halfSize,	halfSize); //bottom left

		tmp = calcBoxR((node << 2) + 2, x+halfSize,	y + halfSize,	halfSize); //bottom right
		if(tmp.x<box.x) box.x=tmp.x;
		if(tmp.y>box.y) box.y=tmp.y;

		tmp = calcBoxR((node << 2) + 3, x,			y,				halfSize); //top left
		if(tmp.x<box.x) box.x=tmp.x;
		if(tmp.y>box.y) box.y=tmp.y;

		tmp = calcBoxR((node << 2) + 4, x+halfSize,	y,				halfSize); //top right
		if(tmp.x<box.x) box.x=tmp.x;
		if(tmp.y>box.y) box.y=tmp.y;

	} else {
		
		//Calculate min and max from vertex data
		box.x = box.y = height(x, y);
		for(int i=0; i<=size; i++){
			for(int j=0; j<=size; j++){
				float h = height(x+i, y+j);
				if(h<box.x) box.x = h;
				if(h>box.y) box.y = h;
			}
		}
		
		//save node index in this patch
		patch(x / size, y / size)->node = node;
	}
	
	m_tree[node].min = box.x;
	m_tree[node].max = box.y;
	
	return box;
}

void HMRegion::clip(Camera3D *cam, const vector3d &scale, const vector3d &pos, int rx, int ry){
	m_rx = rx;
	m_ry = ry;
	m_scale = scale;
	m_pos = pos;
	clipR(cam, 0, 0, 0, m_size, 0x7e);
}

void HMRegion::clipR(Camera3D *cam, int node, int x, int y, int size, int parent){
	
	//Do clipping tests here
	int flags = m_tree[node].clipFlags;
	//int lastFlags = flags; - for stopping if no change - causes problems
	
	//if parent was all in, so are its children
	if(parent & 0x1) flags = 0x7f; 
	//if parent all out, so are children
	else if(parent==0) flags = 0;
	//if parent intersects, need to retest
	else {
		vector3d low, high;
	
		low.x = (m_rx*m_size + x) * m_scale.x;
		low.y = m_tree[node].min * m_scale.y;
		low.z = (m_ry*m_size + y) * m_scale.z;
		
		high.x = (m_rx*m_size + x + size) * m_scale.x;
		high.y = m_tree[node].max * m_scale.y;
		high.z = (m_ry*m_size + y + size) * m_scale.z;
		
		flags = cam->insideFrustum(low+m_pos, high+m_pos);
	}
	
	//save flags
	m_tree[node].clipFlags = flags;
	
	//stop here if node is all out
	if(flags==0) return;
	
	//we can also stop if all in and no change
	//if(flags==lastFlags && (flags & 0x1)) return;
	
	
	//recurse through quadtree if not a leaf node
	int halfSize = size >> 1;
	if(size > m_patchSize){
		clipR(cam, (node << 2) + 1, x,			y + halfSize,	halfSize, flags); //bottom left
		clipR(cam, (node << 2) + 2, x+halfSize,	y + halfSize,	halfSize, flags); //bottom right
		clipR(cam, (node << 2) + 3, x,			y,				halfSize, flags); //top left
		clipR(cam, (node << 2) + 4, x+halfSize,	y,				halfSize, flags); //top right
	}
}

void HMRegion::setLevels(Camera3D *cam, const wgd::vector3d &scale, float errorMetric){
	m_errorMetric = errorMetric;
	m_scale = scale;
	m_camFar = cam->farclip();
	m_camPos = cam->position();
	setLevelsR(0, 0, 0, m_size);
}

void HMRegion::setLevelsR(int node, int x, int y, int size){
	
	if(m_tree[node].clipFlags==0) return;
	
	int halfSize = size >> 1;
	
	//calculate distance from camera (of the node centre)
	vector3d centre;
	centre.x = (m_rx*m_size + x + halfSize) * m_scale.x;
	centre.y = ((m_tree[node].min + m_tree[node].max) / 2) * m_scale.y;
	centre.z = (m_ry*m_size + y + halfSize) * m_scale.z;
	m_tree[node].distance = m_camPos.distance(centre);
	
	
	if(size > m_patchSize){
		//recurse through quadtree if not a leaf node
		setLevelsR((node << 2) + 1, x,			y + halfSize,	halfSize); //bottom left
		setLevelsR((node << 2) + 2, x+halfSize,	y + halfSize,	halfSize); //bottom right
		setLevelsR((node << 2) + 3, x,			y,				halfSize); //top left
		setLevelsR((node << 2) + 4, x+halfSize,	y,				halfSize); //top right
	} else {
		
		//get patch at this leaf node
		Patch *p = patch(x / m_patchSize, y / m_patchSize);
		
		//initialise mip level
		int level = m_levels-1;
		
		//calculate mip level from the log of the distance from the camera
		float farclip = m_camFar * 0.8f;
		float dist = m_tree[node].distance;
		int distanceScale = (m_patchSize * (int)(farclip - dist)) / (int)farclip;
		while(distanceScale>0){
			level--;
			distanceScale >>= 1;
		}

		//calculate mip level from the screen distortion error metric
		//use this if it allows a higher mip level
		dist *= m_errorMetric;
		for(int i=m_levels-1; i>=0; i--) {
			if(p->error[i] * m_scale.y <= dist){
				if(i>level) level = i;
				break;
			}
		}
	
		//Set the new level
		if(level<0) level=0;
		if(level!=p->level){
			p->level=level;
			p->changed = true;
		}
	}
}





//	Height and normals of a point	//

float HMRegion::height(float x, float y) const {
	int lx = (int)floor(x);
	int lz = (int)floor(y);
	//fractions
	float dx = x - lx;
	float dz = y - lz;
	
	//four spot heights
	float h1 = height(lx,   lz);
	float h2 = height(lx,   lz+1);
	float h3 = height(lx+1, lz);
	float h4 = height(lx+1, lz+1);
	
	//heights on z plane
	h1 = h1 + (h2-h1) * dz;
	h2 = h3 + (h4-h3) * dz;
	
	//final point plus heightmap position
	return h1 + (h2-h1) * dx;
}

vector3d HMRegion::normal(float x, float y) const {
	int lx = (int)floor(x);
	int lz = (int)floor(y);
	//fractions
	float dx = x - lx;
	float dz = y - lz;

	//four vertex normals
	vector3d n1 = normal(lx,   lz);
	vector3d n2 = normal(lx,   lz+1);
	vector3d n3 = normal(lx+1, lz);
	vector3d n4 = normal(lx+1, lz+1);
	
	///normals on z plane
	n1 = n1 + (n2-n1) * dz;
	n2 = n3 + (n4-n3) * dz;

	//final normal
	return n1 + (n2-n1) * dx;
}

bool HMRegion::intersect(vector3d& point, const wgd::vector3d &start, const wgd::vector3d &end) {
	/*//draw full line
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(start.x, start.y, start.z);
	glVertex3f(end.x, end.y, end.z);
	glColor3f(0.0, 0.8f, 0.8f);
	glEnd();//*/

	bool r = intersectPatches(point, start, end);
	
	//glColor3f(1.0, 1.0, 1.0);
	//glEnable(GL_LIGHTING);
	return r;
}

bool HMRegion::intersectPatches(wgd::vector3d &point, const wgd::vector3d &start, const wgd::vector3d &end) {
	//re-written version - to try and fix a problem!
	vector3d d = end - start;	//direction
	vector3d a, b;			//per-test start and end points
	vector3d inc;			//increment
	int patchCount = m_size / m_patchSize;
	
	//The algorithm is different depending on line orientation
	if (fabs(d.x) < fabs(d.z)) { //major axis: Z ===========================
		inc.x = d.x / d.z;
		inc.y = d.y / d.z;
		inc.z = 1.0;
		if(d.z < 0) inc = inc * -1;
		
		//adjust for start position
		if(inc.z < 0) a = start - inc * ( ceil(start.z / m_patchSize) * m_patchSize - start.z );
		else a = start - inc * ( start.z - floor(start.z / m_patchSize) * m_patchSize );
		inc = inc * (float)m_patchSize;
		
		int limit = (int)( d.length() / inc.length() + 1 );
		for(int i=0; i<limit; i++) {
			b = a + inc;
			
			//get the starting patch
			int px = (int)a.x / m_patchSize;
			int pz = (int)a.z / m_patchSize;
			//negative directions get the wrong AABBs
			if(inc.z < 0.0 && pz>0) pz--;
			//dont go off the edge
			if(px >= patchCount) px = patchCount-1;
			if(pz >= patchCount) pz = patchCount-1;
			//get the node of this patch
			int n = patch(px, pz)->node;
			int n2 = n;
			
			//may need to also use adjacent node x+-1
			if(inc.x>0) {
				if(px < patchCount-1 && a.x - px*m_patchSize > m_patchSize-inc.x) n2 = patch(px+1, pz)->node;
			} else {
				if(px > 0 && px*m_patchSize - a.x > inc.x) n2 = patch(px-1, pz)->node;
			}
			
			//outer limits of patches n and n2
			float min = (m_tree[n].min < m_tree[n2].min)? m_tree[n].min : m_tree[n2].min;
			float max = (m_tree[n].max > m_tree[n2].max)? m_tree[n].max : m_tree[n2].max;
			
			/*/more debug lines
			glBegin(GL_LINES);
			glVertex3f(a.x, max, a.z);
			glVertex3f(a.x, min, a.z);
			glVertex3f(b.x, max, b.z);
			glVertex3f(b.x, min, b.z);
			//
			glVertex3f(a.x, max, a.z);
			glVertex3f(b.x, max, b.z);
			glEnd();//*/
			
			//test with geometry if inside patch bounds
			if(!(a.y > max && b.y > max) && !(a.y < min && b.y < min)) {
				if(i==0 && intersectPolygons(point, start, b)) return true;
				else if(intersectPolygons(point, a, b)) return true;
			}
			
			//next test
			a = b;
		}
		
	} else { //Major axis: X =========================================
		inc.x = 1.0;
		inc.y = d.y / d.x;
		inc.z = d.z / d.x;
		if (d.x < 0) inc = inc * -1;
		
		//adjust for start position
		if(inc.x < 0) a = start - inc * ( ceil(start.x / m_patchSize) * m_patchSize - start.x );
		else a = start - inc * ( start.x - floor(start.x / m_patchSize) * m_patchSize );
		inc = inc * (float)m_patchSize;
		
		int limit = (int)( d.length() / inc.length() + 1 );
		for(int i=0; i<limit; i++) {
			b = a + inc;
			
			//get start patch
			int px = (int)a.x / m_patchSize;
			int pz = (int)a.z / m_patchSize;
			//negative directions get the wrong AABBs
			if(inc.x < 0.0 && px>0) px--;
			//dont go off the edge
			if(px >= patchCount) px = patchCount-1;
			if(pz >= patchCount) pz = patchCount-1;
			//patch node
			int n = patch(px, pz)->node;
			int n2 = n;
			
			//may need to also use adjacent node x+-1
			if(inc.z>0) {
				if(pz < patchCount-1 && a.z - pz*m_patchSize > m_patchSize-inc.z) n2 = patch(px, pz+1)->node;
			} else {
				if(pz > 0 && pz*m_patchSize-a.z > inc.z) n2 = patch(px, pz-1)->node;
			}
			
			//outer limits of patches n and n2
			float min = (m_tree[n].min < m_tree[n2].min)? m_tree[n].min : m_tree[n2].min;
			float max = (m_tree[n].max > m_tree[n2].max)? m_tree[n].max : m_tree[n2].max;
			
			/*/more debug lines
			glBegin(GL_LINES);
			glVertex3f(a.x, max, a.z);
			glVertex3f(a.x, min, a.z);
			glVertex3f(b.x, max, b.z);
			glVertex3f(b.x, min, b.z);
			//
			glVertex3f(a.x, max, a.z);
			glVertex3f(b.x, max, b.z);
			glEnd();//*/
			
			//test with geometry if inside patch bounds
			if(!(a.y > max && b.y > max) && !(a.y < min && b.y < min)) {
				if(i==0 && intersectPolygons(point, start, b)) return true;
				else if(intersectPolygons(point, a, b)) return true;
			}
			
			//next test
			a = b;
		}
	}
	
	//no intersection
	return false;
}

bool HMRegion::intersectPolygons(wgd::vector3d &point, const wgd::vector3d &start, const wgd::vector3d &end) {
	/*//DEBUG: Draw the line segment
	glColor3f(1.0, 1.0, 0.0);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glVertex3f(start.x, start.y, start.z);
	glVertex3f(end.x, end.y, end.z);
	glEnd();
	glLineWidth(1.0f);
	//*/
	
	//Temporary information
	vector3d d = end - start;
	vector3d p = start;
	vector3d inc;
	
	//Is the start point above or below the heightmap?
	if(start.x < 0 || start.x >= m_size || start.z < 0 || start.z >= m_size) {
		//std::cout << "Invalid test\n";
		return false;
	}
	bool above = height(start.x, start.z) >= start.y;
	
	//Get increment in each dimension
	if (fabs(d.x) < fabs(d.z)) {
		inc.x = d.x / d.z;
		inc.y = d.y / d.z;
		inc.z = 1.0;
		if(d.z < 0) inc = inc * -1;
		
		//find the first edge
		if(p.z != floor(p.z)){
			if(inc.z > 0) p += inc * (ceil(p.z) - p.z);
			else p+= inc * (p.z - floor(p.z));
		}
		
	} else {
		inc.x = 1.0;
		inc.y = d.y / d.x;
		inc.z = d.z / d.x;
		if (d.x < 0) inc = inc * -1;
		
		//find the first edge
		if(p.x != floor(p.x)){
			if(inc.x > 0) p += inc * (ceil(p.x) - p.x);
			else p+= inc * (p.x - floor(p.x));
		}		
	}
	
	//number of steps until the end of the line segment
	int limit = (int)( d.length() / inc.length() + 2 );

	//Trace along the ray
	while(limit-- > 0) {
		//make sure the point is inside the region (it should  be)
		if(p.x < 0 || p.x >= m_size || p.z < 0 || p.z >= m_size) {
			//std::cout << "Test outside region\n";
		} else {
			//Stop if the intersection is between this point and the last one!
			if((height(p.x, p.z) >= p.y) != above) break;
		}
		//next test position
		p += inc;
	}
	
	//if we reached the end of the segment
	if(limit<0) return false;
	
	//get the actual intersection point
	float h1 = (p.y-inc.y) - height(p.x-inc.x, p.z-inc.z);
	float h2 = height(p.x, p.z) - p.y;
	point = p - inc * (1 / (1 + h1/h2));
	
	return true;
}

