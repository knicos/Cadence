#include <wgd/animationset.h>

using namespace wgd;

AnimationSet::AnimationSet(int size) : m_size(size) {
	// create arrays of vectors
	m_scale = new std::vector<KeyScale*> [ size ];
	m_position = new std::vector<KeyPosition*> [ size ];
	m_orientation = new std::vector<KeyOrientation*> [ size ];
}

AnimationSet::~AnimationSet() {
	for(int j=0; j<m_size; j++) {
		for(unsigned int i=0; i<m_scale[j].size(); i++) delete m_scale[j][i];
		for(unsigned int i=0; i<m_position[j].size(); i++) delete m_position[j][i];
		for(unsigned int i=0; i<m_orientation[j].size(); i++) delete m_orientation[j][i];
	}
	delete [] m_scale;
	delete [] m_position;
	delete [] m_orientation;
}

// add keyframes //
void AnimationSet::keyScale(int id, int frame, const vector3d &scale) {
	if(id >= m_size) return; //error
	if(m_scale[id].size()>0 && frame <= m_scale[id].back()->frame) return; //error
	KeyScale *k = new KeyScale;
	k->frame = frame; 
	k->scale = scale;
	m_scale[id].push_back(k);
}
void AnimationSet::keyPosition(int id, int frame, const vector3d &position) {
	if(id >= m_size) return; //error
	if(m_position[id].size()>0 && frame <= m_position[id].back()->frame) return; //error
	KeyPosition *k = new KeyPosition;
	k->frame = frame; 
	k->position = position;
	m_position[id].push_back(k);
}
void AnimationSet::keyOrientation(int id, int frame, const quaternion &orientation) {
	if(id >= m_size) return; //error
	if(m_orientation[id].size()>0 && frame <= m_orientation[id].back()->frame) return; //error
	KeyOrientation *k = new KeyOrientation;
	k->frame = frame;
	k->orientation = orientation;
	m_orientation[id].push_back(k);
}


// Get frame //
vector3d AnimationSet::scale(int id, float frame) const {
	if(m_scale[id].empty()) return vector3d(1.0, 1.0, 1.0);
	
	for(unsigned int i=0; i<m_scale[id].size(); i++){
		
		//find first keyframe after current frame
		if((float)m_scale[id][i]->frame > frame){
			
			//frame before first keyframe
			if(i==0) return m_scale[id][i]->scale;
			
			//linear interpolation between keyframe i-1 and keyframe i
			float ka = (float)m_scale[id][i-1]->frame;
			float kb = (float)m_scale[id][i]->frame;
			
			float pcnt = (frame - ka) / (kb - ka);
			
			return vector3d::lerp(m_scale[id][i-1]->scale, m_scale[id][i]->scale, pcnt);
		}
	}
	
	//after last keyframe
	return m_scale[id].back()->scale;
}

vector3d AnimationSet::position(int id, float frame) const {
	if(m_position[id].empty()) return vector3d();
	
	for(unsigned int i=0; i<m_position[id].size(); i++){
		
		//find first keyframe after current frame
		if((float)m_position[id][i]->frame > frame){
			
			//frame before first keyframe
			if(i==0) return m_position[id][i]->position;
			
			//linear interpolation between keyframe i-1 and keyframe i
			float ka = (float)m_position[id][i-1]->frame;
			float kb = (float)m_position[id][i]->frame;
			
			float pcnt = (frame - ka) / (kb - ka);
			
			return vector3d::lerp(m_position[id][i-1]->position, m_position[id][i]->position, pcnt);
		}
	}
	
	//after last keyframe
	return m_position[id].back()->position;
}

quaternion AnimationSet::orientation(int id, float frame) const {
	if(m_orientation[id].empty()) return quaternion();
	
	for(unsigned int i=0; i<m_orientation[id].size(); i++){
		
		//find first keyframe after current frame
		if((float)m_orientation[id][i]->frame > frame){
			
			//frame before first keyframe
			if(i==0) return m_orientation[id][i]->orientation;
			
			//linear interpolation between keyframe i-1 and keyframe i
			float ka = (float)m_orientation[id][i-1]->frame;
			float kb = (float)m_orientation[id][i]->frame;
			
			float pcnt = (frame - ka) / (kb - ka);
			
			return quaternion::slerp(m_orientation[id][i-1]->orientation, m_orientation[id][i]->orientation, pcnt);
		}
	}
	
	//after last keyframe
	return m_orientation[id].back()->orientation;
}
