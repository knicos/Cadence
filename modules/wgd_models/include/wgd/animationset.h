#ifndef _ANIMATION_SET_
#define _ANIMATION_SET_

#include <wgd/vector.h>
#include <wgd/quaternion.h>
#include <vector>

namespace wgd {

	class AnimationSet {
		public:
		
		struct KeyScale {
			int frame;
			vector3d scale;
		};
		
		struct KeyPosition {
			int frame;
			vector3d position;
		};
		
		struct KeyOrientation {
			int frame;
			quaternion orientation;
		};
	
		// constructor - set size (number of bones with animations) //
		AnimationSet(int size);
		~AnimationSet();
		
		// get values at frames //
		vector3d scale(int id, float frame) const;
		vector3d position(int id, float frame) const;
		quaternion orientation(int id, float frame) const;
		
		// add keyframes //
		void keyScale(int id, int frame, const vector3d &scale);
		void keyPosition(int id, int frame, const vector3d &position);
		void keyOrientation(int id, int frame, const quaternion &orientation);
		
		
		private:
		
		std::vector<KeyScale*> *m_scale;
		std::vector<KeyPosition*> *m_position;
		std::vector<KeyOrientation*> *m_orientation;
		
		int m_size;
	};
	

};

#endif
