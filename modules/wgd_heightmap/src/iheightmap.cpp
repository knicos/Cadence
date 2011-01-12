#include <wgd/iheightmap.h>
#include <wgd/heightmapsource.h>
#include <wgd/hmimagesource.h>
#include <wgd/math.h>
#include <wgd/extensions.h>
#include <wgd/render.h>

using namespace cadence;

extern "C" void HEIGHTMAPIMPORT initialise(const cadence::doste::OID &base) {
	Object::registerType<wgd::IHeightmap>();
	Object::registerType<wgd::HeightmapSource>();
	Object::registerType<wgd::HMImageSource>();
}

extern "C" void HEIGHTMAPIMPORT finalise() {
}




using namespace wgd;

IHeightmap::IHeightmap(HeightmapSource* src) : Instance3D(), m_regionSize(0) {
	set(ix::type, type());
	if(get(ix::scale)==Null) scale(1.0f);
	if(get("splatscale")==Null) splatScale(8.0f);
	if(get("fade")==Null) fade(3.0f * patchSize());
	source(src);
}

IHeightmap::IHeightmap(const OID& id) : Instance3D(id), m_regionSize(0) {
	if(get(ix::scale)==Null) scale(1.0f);
	if(get("splatscale")==Null) splatScale(8.0f);
	if(get("fade")==Null) fade(3.0f * patchSize());
}

IHeightmap::~IHeightmap(){}

void IHeightmap::errorMetric(float fovY, int viewHeight){
	float halfFov;
	float vlod = lod();
	if(vlod<=0) vlod = 4.0f;

	if(fovY >= 180.0f)	fovY = 179.99f;
	else if(fovY < 0.0) fovY = 0.0;

	halfFov = fovY * (float)PI / 360.0f;
	m_errorMetric = (float)(tan(halfFov) / (float)viewHeight) * vlod;
}

void IHeightmap::draw(SceneGraph &graph, Camera3D *camera) {
	//update fps
	WGD[ix::widgets][ix::root][ix::children][ix::height][ix::children]["fps"]["draw"].set(doste::Void);
	
	//begin heightmap operations
	m_source = source();
	m_source->begin();
	
	//info
	m_polys = 0;
	m_maxError = 0.0;
	m_count=0;
	
	//local data for this draw loop
	m_regionSize = m_source->regionSize();
	m_patchSize = patchSize();
	if(m_patchSize==0) m_patchSize = 16;
	m_patches = m_regionSize / m_patchSize;
	m_splatScale = splatScale();
	errorMetric(camera->fov(), camera->height());
	m_farClip = camera->farclip();
	m_fade = fade();
	m_scale = scale();
	
	if(wireframe()) glPolygonMode(GL_FRONT, GL_LINE);
	
	//Setup gl client state
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glEnable(GL_NORMALIZE);
	
	//draw infinite regions
	vector3d ctr = camera->position() / m_scale;
	int rx = (int)floor(ctr.x / m_regionSize); //the region containing the camera
	int ry = (int)floor(ctr.z / m_regionSize);
	int visx = (int)ceil(m_farClip / (m_regionSize * m_scale.x));  //maximum number of visible patches in any direction
	int visz = (int)ceil(m_farClip / (m_regionSize * m_scale.z));
	
	//draw regions
	for(int i=rx-visx; i<=rx+visx; i++) {
		for(int j=ry-visz; j<=ry+visz; j++) {
			drawRegion(camera, i, j);
		}
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	if(wireframe()) glPolygonMode(GL_FRONT, GL_FILL);
	
	//end heightmap operations
	m_source->end();
	
	//output info
	OID l = WGD[ix::widgets][ix::root][ix::children][ix::height][ix::children]["label"][ix::caption].get();
	l.set(8,  m_polys);
	l.set(22, m_maxError * m_scale.y);
	l.set(35, m_count);
	l.set(37, m_patches * m_patches * 9);
}

void IHeightmap::drawRegion(Camera3D *camera, int rx, int ry) {
	//Will asynchronously load the region if it is not in memory
	HMRegion *region = source()->region(rx, ry);
	
	if(!region) return;
	
	//setup patches
	region->setup(m_patchSize, m_splatScale);
	
	vector3d pos = position();
	
	//clip patches		(HMRegion)
	region->clip(camera, m_scale, pos, rx, ry); 
	
	//exit here if region is completely off sreen
	if(region->node(0).clipFlags==0) return;
	
	//set mip levels	(HMRegion)
	region->setLevels(camera, m_scale, m_errorMetric);
	
	//draw patches		(IHeightmap)
	glPushMatrix();

	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(m_scale.x, m_scale.y, m_scale.z);
	glTranslatef((float)rx * m_regionSize, 0.0, (float)ry * m_regionSize);
	
	
	//Set up pointers
	if(region->vbo()) {
		WGDglBindBufferARB( GL_ARRAY_BUFFER_ARB, region->vbo() );
		glVertexPointer( 3, GL_FLOAT, sizeof(vertex), (void*) 0);
		glNormalPointer(    GL_FLOAT, sizeof(vertex), (void*) (3 * sizeof(float)));
		glTexCoordPointer(2,    GL_FLOAT, sizeof(vertex), (void*) (6 * sizeof(float)));
		WGDglBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
	} else {
		float *p = &region->vertices()[0].x;	//p is 0 if using VBO
		glVertexPointer(3, GL_FLOAT, sizeof(vertex), p);
		glNormalPointer(   GL_FLOAT, sizeof(vertex), p+3);
		glTexCoordPointer(2,   GL_FLOAT, sizeof(vertex), p+6);
	}
	
	//recursively draw all visible patches with base material
	Material *mbase = region->baseMaterial();
	if(mbase) mbase->bind();
	drawPatchR(region, rx, ry, 0, 0, 0, m_regionSize);
	if(mbase) mbase->unbind();
	
	
	//Set up openGL for texture splatting
	if(m_fade > 0.0) {
		WGDglClientActiveTextureARB(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,   GL_FLOAT, sizeof(GLTexCoord), region->splatCoord());
		
		//recursively draw any texture splatting
		for(int i=0; i<region->splatCount(); i++) {
			drawSplatR(region, i, rx, ry, 0, 0, 0, m_regionSize);
			region->splatMaterial(i)->unbind();
		}
		
		//restore openGL stuff
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		WGDglClientActiveTextureARB(GL_TEXTURE0_ARB);
	}
	
	
	/*/Debug output to test ray intersection
	vector3d p; bool hit=false;
	if(rx==0 && ry==0){
		if((bool)WGD[ix::input][ix::keyboard][ix::keys]['o'].get()) {
			vs = camera->position();
			ve = vs + vector3d().translate(camera->quaternion().conjugate(), vector3d(0.0, 0.0, -512.0));
		}
		hit = intersect(p, vs, ve);
	}//*///
	
	glPopMatrix();
	
	//if(hit && get("sprite")!=Null) Render::sprite(get("sprite"), 0, p);
}


//get a patch using absolute patch coordinates (from any region)
wgd::Patch *IHeightmap::patch(int x, int y) {
	//ger region of patch
	int rx = (int)floor((float)x / m_patches);
	int ry = (int)floor((float)y / m_patches);
	
	//This should load the region asynchronously if not already loaded
	HMRegion *region = m_source->region(rx, ry);
	region->setup(m_patchSize, m_splatScale);
	
	//get patch
	int px = x - rx * m_patches;
	int py = y - ry * m_patches;
	return region->patch(px, py);
}

unsigned int IHeightmap::glue(Patch *p, int x, int y){
	int l[4];
	//do null checking here if patch can return null
	l[0] = patch(x, y-1)->level;	//top glue
	l[1] = patch(x, y+1)->level;	//bottom glue
	l[2] = patch(x-1, y)->level;	//left glue
	l[3] = patch(x+1, y)->level;	//right glue
	
	unsigned int g = 0;
	if(p->level < l[0]) g |= l[0];
	if(p->level < l[1]) g |= l[1]<<8;
	if(p->level < l[2]) g |= l[2]<<16;
	if(p->level < l[3]) g |= l[3]<<24;
	return g;
}

void IHeightmap::drawPatchR(HMRegion *region, int rx, int ry, int node, int x, int y, int size){
	
	if(region->node(node).clipFlags==0) return;
	
	if(size > m_patchSize){
		//recurse through quadtree if not a leaf node
		int halfSize = size >> 1;
		drawPatchR(region, rx, ry, (node << 2) + 1, x,			y + halfSize,	halfSize); //bottom left
		drawPatchR(region, rx, ry, (node << 2) + 2, x+halfSize,	y + halfSize,	halfSize); //bottom right
		drawPatchR(region, rx, ry, (node << 2) + 3, x,			y,				halfSize); //top left
		drawPatchR(region, rx, ry, (node << 2) + 4, x+halfSize,	y,				halfSize); //top right
	} else {
		
		//get the patch object
		int px = x / m_patchSize;
		int py = y / m_patchSize;
		Patch *p = region->patch(px, py);
		
		//See if glue has changed
		unsigned int g = glue(p, px + rx*m_patches, py + ry*m_patches);
		
		//rebuld patch if nesesary
		if(p->glue != g || p->changed)	buildPatch(p, g);
		
		m_polys += p->nIndices / 3;
		m_maxError = max( p->error[p->level], m_maxError );
		
		//Draw patch
		glDrawElements(GL_TRIANGLES, p->nIndices, GL_UNSIGNED_INT, p->indices);
		
		m_count++;
	}
}

void IHeightmap::drawSplatR(HMRegion *region, int splat, int rx, int ry, int node, int x, int y, int size){
	
	if(region->node(node).clipFlags==0) return;
	
	//work out splat based on distance
	float d = region->node(node).distance;
	float scl = vector2d(m_scale.x, m_scale.z).length();
	if(d - size*scl > m_fade) return;
	
	if(size > m_patchSize){
		//recurse through quadtree if not a leaf node
		int halfSize = size >> 1;
		drawSplatR(region, splat, rx, ry, (node << 2) + 1, x,			y + halfSize,	halfSize); //bottom left
		drawSplatR(region, splat, rx, ry, (node << 2) + 2, x+halfSize,	y + halfSize,	halfSize); //bottom right
		drawSplatR(region, splat, rx, ry, (node << 2) + 3, x,			y,				halfSize); //top left
		drawSplatR(region, splat, rx, ry, (node << 2) + 4, x+halfSize,	y,				halfSize); //top right
	} else {
		//get the patch object
		int px = x / m_patchSize;
		int py = y / m_patchSize;
		Patch *p = region->patch(px, py);
		
		//drawing the splat texture
		region->splatMaterial(splat)->bind();
		
		//aplha fro some nice fadey stuff
		float a = 1.0;
		//float c12 = m_farClip / 12, c8 = m_farClip / 8;
		//if(d > c12) a = 1.0f - (d-c12) / (c8-c12);
		float sfade = m_fade / 2; //fade start
		if(d > sfade) a = 1.0f - (d-sfade) / (m_fade - sfade); //fade amount
		GLfloat mat[4] = { 1.0, 1.0, 1.0, a };	//apply fade
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
		
		//draw heightmap patch
		glDrawElements(GL_TRIANGLES, p->nIndices, GL_UNSIGNED_INT, p->indices);
	}
}

void IHeightmap::buildPatch(Patch *p, unsigned int g){
	
	int step = 0x1 << p->level;
	p->nIndices = 0;
	p->changed = false;
	p->glue = g;
	
	if(!g){
		//no glue - create entire patch with triangle strips
		for (int i = 0; i < m_patchSize; i+=step){
			acrossStrip(p, i*(m_regionSize+1), m_patchSize, step);
		}
	} else {
		//draw central block using triangle strips
		for (int i = step; i < m_patchSize-step; i += step){
			acrossStrip(p, i*(m_regionSize+1) + step, m_patchSize - 2*step, step);
		}
		
		//fill in borders with glue

		//top glue
		int tg = (g>>0) & 0xff;
		topGlue(p, (tg>0? tg: p->level));

		//bottom glue
		int bg = (g>>8) & 0xff;
		bottomGlue(p, (bg>0? bg: p->level));

		//left glue
		int lg = (g>>16) & 0xff;
		leftGlue(p, (lg>0? lg: p->level));

		//Right glue
		int rg = (g>>24) & 0xff;
		rightGlue(p, (rg>0? rg: p->level));
	}
}

void IHeightmap::acrossStrip(Patch *p, int start, int length, int step){
	int &k = p->nIndices;
	int offset = p->offset;
  	for(int i = start; i < start+length; i += step) {
		p->indices[k  ] = offset + i;
		p->indices[k+1] = offset + i + (m_regionSize+1) * step;
		p->indices[k+2] = offset + i + step;	//get the starting region
		p->indices[k+3] = offset + i + (m_regionSize+1) * step + step;
		p->indices[k+4] = p->indices[k+2];
		p->indices[k+5] = p->indices[k+1];
		k += 6;
	}
}
void IHeightmap::leftGlue(Patch *p, int nextLevel) {
	int &k = p->nIndices;
	int offset = p->offset;
	int min = 0x1 << p->level;
	int max = 0x1 << nextLevel;
	int p0=min, p1=2*min;

	for(int i=max; i<=m_patchSize; i+=max){
		p->indices[k++] = offset + min + p0*(m_regionSize+1);
		p->indices[k++] = offset + (i-max)*(m_regionSize+1);
		p->indices[k++] = offset + i*(m_regionSize+1);

		for(; p0<(i) && p0<(m_patchSize-min); p1+=min){
			p->indices[k++] = offset + i*(m_regionSize+1);
			p->indices[k++] = offset + min + p1*(m_regionSize+1);
			p->indices[k++] = offset + min + p0*(m_regionSize+1);
			p0=p1;
		}	
	}
}
void IHeightmap::rightGlue  (Patch *p, int nextLevel){
	int &k = p->nIndices;
	int offset = p->offset;
	int min = 0x1 << p->level;
	int max = 0x1 << nextLevel;
	int p0=min, p1=2*min;//, n=0;
	
	for(int i=0; i<m_patchSize; i+=max){
		for(; p0<(i+max) && p0<(m_patchSize-min); p1+=min){
			p->indices[k++] = offset + m_patchSize + i*(m_regionSize+1);
			p->indices[k++] = offset + m_patchSize - min + p0*(m_regionSize+1);
			p->indices[k++] = offset + m_patchSize - min + p1*(m_regionSize+1);
			p0=p1;
		}

		p->indices[k++] = offset + m_patchSize + i*(m_regionSize+1);
		p->indices[k++] = offset + m_patchSize - min + p0*(m_regionSize+1);
		p->indices[k++] = offset + m_patchSize + (i+max)*(m_regionSize+1);
	}
}
void IHeightmap::topGlue(Patch *p, int nextLevel){
	int &k = p->nIndices;
	int offset = p->offset;
	int min = 0x1 << p->level;
	int max = 0x1 << nextLevel;
	int p0=min, p1=2*min;//, n=0;

	for(int i=max; i<=m_patchSize; i+=max){
		p->indices[k++] = offset + min*(m_regionSize+1)+p0;
		p->indices[k++] = offset + i;
		p->indices[k++] = offset + i-max;

		for(; p0<(i) && p0<(m_patchSize-min); p1+=min){
			p->indices[k++] = offset + i;
			p->indices[k++] = offset + min*(m_regionSize+1)+p0;
			p->indices[k++] = offset + min*(m_regionSize+1)+p1;
			p0=p1;
		}
	}
}
void IHeightmap::bottomGlue (Patch *p, int nextLevel){
	int &k = p->nIndices;
	int offset = p->offset;
	int min = 0x1 << p->level;
	int max = 0x1 << nextLevel;
	int p0=min, p1=2*min;
	int patchsq = m_patchSize*m_regionSize;
	
	for(int i=0; i<m_patchSize; i+=max){    
		for(; p0<(i+max) && p0<(m_patchSize-min); p1+=min){
			p->indices[k++] = offset + patchsq + m_patchSize + i;
			p->indices[k++] = offset + (m_patchSize-min)*(m_regionSize+1)+p1;
			p->indices[k++] = offset + (m_patchSize-min)*(m_regionSize+1)+p0;
			p0=p1;
		}

		p->indices[k++] = offset + (m_patchSize-min)*(m_regionSize+1)+p0;
		p->indices[k++] = offset + patchsq + m_patchSize + i;
		p->indices[k++] = offset + patchsq + m_patchSize + i+max;
	}
}


float IHeightmap::height(const wgd::vector3d &v){
	//in case local data has not been initialised yet
	if(!m_regionSize) m_regionSize = source()->regionSize();
	
	//adjust for scale and offset
	vector3d pos = (v - position()) / scale();
	
	//get the region
	int rx = (int)floor(pos.x / m_regionSize); 
	int ry = (int)floor(pos.z / m_regionSize);
	HMRegion *r = source()->region(rx, ry);
	
	//Invalid region
	if(!r) return 0; 
	
	//Get point in region coordinates
	pos.x -= rx * m_regionSize;
	pos.z -= ry * m_regionSize;
	
	//get spot height
	float h = r->height(pos.x, pos.z);
	
	//redo scale and offset
	h = h * scale().y + position().y;
	return h;
}

float IHeightmap::height(float x, float z) {
	return height(vector3d(x, 0.0, z));
}

vector3d IHeightmap::normal(const wgd::vector3d &v){
	//in case local data has not been initialised yet
	if(!m_regionSize) m_regionSize = source()->regionSize();
	
	//adjust for scale and offset
	vector3d pos = (v - position()) / scale();
	
	//get the region
	int rx = (int)floor(pos.x / m_regionSize);
	int ry = (int)floor(pos.z / m_regionSize);
	HMRegion *r = source()->region(rx, ry);
	
	//Invalid region - return zero value
	if(!r) return vector3d(); 
	
	//Get point in region coordinates
	pos.x -= rx * m_regionSize;
	pos.z -= ry * m_regionSize;
	
	//get spot normal
	vector3d n = r->normal(pos.x, pos.z);
	return (n * scale()).normalise();
}

bool IHeightmap::intersect(wgd::vector3d &result, const wgd::vector3d &start, const wgd::vector3d &end) {
	//in case local data has not been initialised yet
	//if(!m_regionSize) {
		m_scale = scale();
		m_source = source();
		m_regionSize = m_source->regionSize();
	//}
	//work out which regions the line intersects then clip the line
	//to each region to test them individually
	
	//adjust for scale and offset
	vector3d tstart = (start - position()) / m_scale;
	vector3d tend =   (end   - position()) / m_scale;
	vector3d direction = (tend - tstart).normalise();
	vector3d point = tstart;
	vector3d hit; //result temporary
	
	//get the ending region
	int ex = (int)floor(tend.x / m_regionSize); 
	int ez = (int)floor(tend.z / m_regionSize);
	tend.x -= ex * m_regionSize;
	tend.z -= ez * m_regionSize;
	int rx, rz;
	
	//loop until we get to the ending region
	int count=0;
	do {
		//get current region
		rx = (int)floor(tstart.x / m_regionSize);
		rz = (int)floor(tstart.z / m_regionSize);		
		
		//std::cout << "Testing region: (" << rx << ", " << rz << ")\n";
		
		//translate to region coordinates
		vector3d rstart, rend;
		rstart.x = tstart.x - rx * m_regionSize;
		rstart.y = tstart.y;
		rstart.z = tstart.z - rz * m_regionSize;
		
		
		//get distance to edge of region
		float dx, dz;
		if(direction.x > 0) dx = fabs((m_regionSize - rstart.x) / direction.x);
		else if(direction.x < 0) dx = fabs(rstart.x / direction.x);
		else dx = 999999;
		
		if(direction.z > 0) dz = fabs((m_regionSize - rstart.z) / direction.z);
		else if(direction.z < 0) dz = fabs(rstart.z / direction.z);
		else dz = 999999;	
		
		//end point
		if(rx==ex && rz==ez) rend = tend;
		else rend = rstart + direction * (dx<dz ? dx : dz);
		
		//std::cout << "Segment: (" << rstart.x << ", " << rstart.y << ", " << rstart.z << ") - (";
		//std::cout << rend.x << ", " << rend.y << ", " << rend.z << ")\n";
		
		//get region object to test against
		HMRegion *r = m_source->region(rx, rz);
		
		//Invalid region - return no intersection
		if(!r) return false;
		
		//test to see if the line segment is inside region bounding box to do the region intersect test
		HMRegion::HMNode &node = r->node(0);
		if(!(rstart.y > node.max && rend.y > node.max) && !(rstart.y < node.min && rend.y < node.min)) {
			if(r->intersect(hit, rstart, rend)) {
				//convert back to world coordinates
				hit.x += rx*m_regionSize;
				hit.z += rz*m_regionSize;
				result = hit * m_scale + position();
				return true;
			}
		}
		
		//next start point - hopefully it will be in the next region...
		tstart += direction * ((dx<dz ? dx : dz) + 0.01f);
		
		//debug: quit if more that 3 regions
		if(count++ > 3) break;
		
	} while(rx!=ex || rz!=ez);
	//no intersection
	return false;
}

bool IHeightmap::ray(wgd::vector3d &result, const wgd::vector3d &point, const wgd::vector3d &direction) {
	//create a really long line segment - may be able to do this better
	return intersect(result, point, point + direction*5000);
	
}
