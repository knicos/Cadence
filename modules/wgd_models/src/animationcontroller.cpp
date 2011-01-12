#include <wgd/animationcontroller.h>
#include <cadence/doste/doste.h>
#include <wgd/math.h>
#include <GL/gl.h>

using namespace wgd;
using namespace cadence;

AnimationController::AnimationController(Instance3D* inst) : m_inst(inst) {
	 m_model = inst->get(ix::model);
	 
	//set up local memory
	int num = m_model->bones();
	//Manually aligh matrices
	m_transformA = new matrix[num+1];
	#ifdef X86_64
	m_transform = (matrix*)(((unsigned long long)m_transformA + 16) & 0xFFFFFFFFFFFFFFF0);
	#else
	m_transform = (matrix*)(((unsigned int)m_transformA + 16) & 0xFFFFFFF0);
	#endif
	
	//default fps
	if(m_model->get(ix::fps)==Null) m_model->fps(30.0f);
	
	m_cscale = new vector3d[num];
	m_cposition = new vector3d[num];
	m_corientation = new quaternion[num];
	
	//set up animations
	memset(m_blend, 0, MAX_ANIMATIONS * sizeof(Blend));
	memset(m_animation, 0, MAX_ANIMATIONS * sizeof(Animation));
	for(int i=0; i<MAX_ANIMATIONS; i++) {
		m_animation[i].m_parent = this;
		m_animation[i].m_index = i;
	}
	
}
AnimationController::~AnimationController() {
	//delete stuff
	delete [] m_transformA;
	//current
	delete [] m_cscale;
	delete [] m_cposition;
	delete [] m_corientation;
}

void AnimationController::drawSkeleton(){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	for(int i=0; i<m_model->bones(); i++){
		vector3d start = m_transform[i] * vector3d();
		vector3d end = m_transform[i] * vector3d(0.0, 0.0, 1.0f);
		vector3d norm = m_transform[i] * vector3d(0.0, 0.2f, 0.0);
		IBone *ibone = m_inst->get(ix::bones)[m_model->get(ix::bones)[i]];
		
		glBegin(GL_LINES);
		if(ibone && ibone->active() && ibone->get("target")!=Null){
			matrix inv; matrix::fastInverse(inv, m_inst->localMatrix());
			vector3d target =  inv * (vector3d)ibone->get("target");
			glColor3f(1.0, 0.5f, 0.0f);
			glVertex3f(start.x, start.y, start.z);
			glVertex3f(target.x, target.y, target.z);
		}
		
		glColor3f(0.0, 1.0f, 1.0f);
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(end.x, end.y, end.z);

		glColor3f(0.0, 0.0, 1.0f);
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(norm.x, norm.y, norm.z);
		
		
		glEnd();
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

bool AnimationController::update(bool force) {
	//update frame and calculate matrices
	
	float time = WGD[ix::time];
	float itime = WGD["itime"];
	
	bool changed=false;
	for(int i=0; i<MAX_ANIMATIONS; i++) {
		if(m_animation[i].animation()!=Null) changed |= update(i, itime);
	}
	
	if(force || changed) {
		calculateBone(ix::root, matrix());
		return true;
	}
	return false;
}

bool AnimationController::update(int b, float itime) {
	
	//calculate frame (loop if nessesary)
	float lastFrame = m_blend[b].frame;
	float dt = itime * m_animation[b].speed;
	if(dt!=0 && m_blend[b].frames>0) {
		float t = m_animation[b].time + dt;
		
		//calculate frame
		if(m_animation[b].loop) {
			float pcnt = t / m_animation[b].length(); //normalised time
			pcnt -= floor(pcnt);	//wrap pcnt between 0 and 1
			float f = pcnt * m_blend[b].frames; //get current frame
			
			//translate this frame to animation start/end frames
			if(m_blend[b].start > m_blend[b].end) f = -f;
			m_blend[b].frame = m_blend[b].start + f;
			
		} else {
			//off the ends of the animation
			if(t<=0) m_blend[b].frame = m_blend[b].start;
			else if(t>=m_animation[b].length()) {
				m_blend[b].frame = m_blend[b].end;
				if(m_blend[b].next !=Null) startAnimation(b);
			} else {
				//get frame from time since start of animation
				float f = t / m_animation[b].length() * m_blend[b].frames;
				
				//translate this frame to animation start/end frames
				if(m_blend[b].start > m_blend[b].end) f = -f;
				m_blend[b].frame = m_blend[b].start + f;
			}
		}
		
		m_animation[b].time = t;
	}
	
	//calculate transition value - later
	
	//return true if something has changed
	return lastFrame!=m_blend[b].frame;
}

matrix &AnimationController::transformMesh(Mesh *mesh) {
	//local matrices for each mesh if not skeletally deformed
	return boneMatrix(mesh->bone());
}

void Animation::start(const OID &name, float wt, const OID &bone, float spd) {
	m_animation = name;
	weight = wt;
	speed = spd;
	m_parent->startAnimation(m_index, bone);
}

void AnimationController::startAnimation(int anim, const OID &bone) {
	//change the current animation
	OID an = m_animation[anim].animation();
	OID animation = m_model->get(ix::animations)[an];
	
	//cache local animation data
	m_blend[anim].set		= m_model->animationSet(animation[ix::animset]);
	m_blend[anim].next		= animation[ix::next];
	m_blend[anim].start		= animation[ix::start];
	m_blend[anim].end		= animation[ix::end];
	m_blend[anim].frames		= fabs(m_blend[anim].end-m_blend[anim].start);
	m_blend[anim].frame		= m_blend[anim].start;
	
	//set bone
	if(bone!=Null) {
		Bone *sbone = m_model->bone(bone);
		if(sbone) m_blend[anim].root = sbone->bid(); 
		else m_blend[anim].root=0;
	}
	
	//change public data
	m_animation[anim].loop		= animation[ix::loop];
	m_animation[anim].m_length = m_blend[anim].frames / m_model->fps();
	m_animation[anim].time = 0;
}

void AnimationController::transform(int bid, int mask, vector3d &position, vector3d &scale, quaternion &orientation) {
	//get base ?? no idea what this will do... - perhaps use frame 0 ??
	scale = m_cscale[bid];
	position = m_cposition[bid];
	orientation = m_corientation[bid];
	
	//loop through all blended animations
	for(unsigned int i=0; i<MAX_ANIMATIONS; i++) {
		//animation applies to this bone
		if(mask & (1<<i)) {
			//scale
			vector3d tscale = m_blend[i].set->scale(bid, m_blend[i].frame);
			scale = vector3d::lerp(scale, tscale, m_animation[i].weight);
			//position
			vector3d tposition = m_blend[i].set->position(bid, m_blend[i].frame);
			position = vector3d::lerp(position, tposition, m_animation[i].weight);
			//orientation
			quaternion torientation = m_blend[i].set->orientation(bid, m_blend[i].frame);
			orientation = quaternion::slerp(orientation, torientation, m_animation[i].weight);
		}
	}
}

void AnimationController::calculateBone(const OID &boneName, const matrix &parent, int mask) {
	//calculate matrix for each bone recursively
	
	Bone *bone = m_model->bone(boneName);
	IBone *ibone = m_inst->get(ix::bones)[boneName];
	int bid = bone->bid();
	
	//activate animations
	for(int i=0; i<9; i++) {
		if(m_blend[i].root==bid && m_animation[i].weight>0 && m_blend[i].set) mask |= (1<<i);
	}
	
	matrix tmp;
	matrix &combined = m_transform[bone->bid()];
	
	//user defined transformation
	if( ibone && ibone->active() ) {
		matrix::fastMultiply(tmp, parent, bone->tMatrix());
		matrix scale; scale.scale(m_inst->get(ix::scale)); //this should really be outside the loop
		const matrix &user = ibone->transform(m_inst->localMatrix() * scale, tmp);
		matrix::fastMultiply(combined, tmp, user);
		
	} else if(mask==0) {
		//use base matrix if no animations active on this bone
		matrix::fastMultiply(combined, parent, bone->tMatrix());
		
	} else { 
		//apply animations to this bone
		matrix rot, pos, scl;
		quaternion quat;
		
		//get transformatrions from animation for this bone
		transform(bid, mask, m_cposition[bid], m_cscale[bid], m_corientation[bid]);
		
		//create matrices
		scl.scale(m_cscale[bid]);
		pos.translate(m_cposition[bid]);
		m_corientation[bid].createMatrix(rot);
		
		//mash all matrices together in a big pile of numbers
		matrix::fastMultiply(combined, pos, scl);
		matrix::fastMultiply(tmp, combined, rot);
		matrix::fastMultiply(combined, parent, tmp);
	}
	
	//recursively process all child bones
	for(int i=0; i<bone->children(); i++){
		calculateBone(bone->child(i), combined, mask);
	}
	
	//deactivate animations
	for(int i=0; i<9; i++) {
		if(m_blend[i].root==bid) mask &= ~(1<<i);
	}
}

matrix &AnimationController::boneMatrix(const wgd::OID &boneName) {
	Bone *bone = m_model->bone(boneName);
	if(bone) return m_transform[bone->bid()];
	else return m_transform[0];
}


///////	IBone stuff ////////////

namespace wgd {
	OnEvent(IBone, evt_target) {
		if(get("target")!=Null) {
			m_target = get("target");
			m_useTarget=true;
			set(ix::changed, doste::Void);
		} else {
			calcPYR();
			m_useTarget = false;
		}
	}
	OnEvent(IBone, evt_pitchyawRoll) { calcPYR(); }
	IMPLEMENT_EVENTS(IBone, Agent);
};

IBone::IBone() {
	registerEvents();
}

IBone::IBone(const cadence::doste::OID &id) : Agent(id), m_useTarget(false) {
	registerEvents();
}

const matrix &IBone::transform(const matrix& p, const matrix& b){
	if(m_useTarget) calcTarget(p, b);
	return m_matrix;
}

void IBone::calcPYR(){
	m_matrix.rotate(orientation());
	if(get(ix::scale)!=Null) {
		matrix scl; scl.scale(scale());
		m_matrix = m_matrix * scl;
	}
	set(ix::changed, doste::Void);
}

void IBone::calcTarget(const matrix &inst, const matrix &parent){
	//get target in bone local coodinates
	matrix mat, inv;
	matrix::fastMultiply(mat, inst, parent);
	matrix::fastInverse(inv, mat);
	vector3d target = inv * m_target;
	
	//calculate pitch and yaw
	float yaw	= atan2(target.x, target.z);
	float pitch	= atan2( sqrt(target.x*target.x + target.z*target.z), target.y) - 1.5707963f;
	float roll	= get(ix::orientation).get(ix::z);
	
	//are there limits?
	//this has to be done here rather than in cadence as triggeting may happen at the wrong time
	//i may be wrong here but it can be taken out if it is a problem.
	
	vector3d vmin, vmax;
	bool limits = (get("max") != Null);
	if(limits) {
		vmin = minimum();
		vmax = maximum();
		
		if(pitch < vmin.x) pitch = vmin.x;
		else if(pitch > vmax.x) pitch = vmax.x;
		
		if(yaw < vmin.y) yaw = vmin.y;
		else if(yaw > vmax.y) yaw = vmax.y;
		
		if(roll < vmin.z) roll = vmin.z;
		else if(roll > vmax.z) roll = vmax.z;
	}
	
	
	//if speed is set, the bone must rotate smoothly at that speed
	float spd = speed();
	if(spd>0.0) {
		vector3d last = orientation();
		
		//get directions
		float pd = pitch - last.x;
		float yd = yaw - last.y;
		
		float pi = (float)PI;
		
		//wrap -> the other direction
		if(pd < -pi && (!limits || vmin.x < -pi)) pd = pi * 2 + pd;
		else if(pd > pi && (!limits || vmax.x > pi)) pd = pd - pi * 2;
		
		if(yd < -pi && (!limits || vmin.y < -PI)) yd = pi * 2 + yd;
		else if(yd > pi && (!limits || vmax.y > PI)) yd = yd - pi * 2;
		
		//rotate
		if(pd > spd) pitch = last.x + spd;
		else if(pd < -spd) pitch = last.x - spd;
		if(yd > spd) yaw = last.y + spd;
		else if(yd < -spd) yaw = last.y - spd;
		
		//wrap values to { -PI, PI }
		if(pitch < -pi) pitch += pi*2;
		else if(pitch > PI) pitch -= pi*2;
		if(yaw < -pi) yaw += pi*2;
		else if(yaw > pi) yaw -= pi*2;
		if(roll < -pi) roll += pi*2;
		else if(roll > pi) roll -= pi*2;
	}
	
	//set orientation to cause the matrix to be recalculated
	orientation(vector3d(pitch, yaw, roll));
}


