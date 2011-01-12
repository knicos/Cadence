#ifndef _WGD_ANIMATION_CONTROLLER_
#define _WGD_ANIMATION_CONTROLLER_

#include <wgd/bone.h>

#include <wgd/matrix.h>
#include <wgd/vector.h>
#include <wgd/quaternion.h>
#include <cadence/agent.h>
#include <cadence/doste/modifiers.h>
#include <wgd/common.h>
#include <wgd/model.h>
#include <wgd/instance3d.h>

#define MAX_ANIMATIONS 9

namespace wgd {
	
	class AnimationController;
	struct MODELSIMPORT Animation {
		friend class AnimationController;
		
		float weight;	//weight
		float speed;	//animation speed
		float time;	//time into animation 
		float length() { return m_length; } ///< length of animation in seconds
		bool loop;	//animation looped, or clamped
		
		const OID& animation() const { return m_animation; } ///< get current animation
		
		/// start a new animation
		void start(const OID &name, float weight=1.0f, const OID &bone=ix::root, float speed=1.0f);
		
		//?
		Animation& operator=(Animation &a) { memcpy(this, &a, sizeof(Animation)); return *this; }
		
		private:
		float m_length;
		OID m_animation;
		
		AnimationController* m_parent;	//parent animationController
		int m_index;			//animation index
	};
	
	class AnimationController {
	friend class SkinController;
		public:
		AnimationController(wgd::Instance3D* inst);
		~AnimationController();
		
		//update animation - returns true if changed
		bool update(bool force);
		
		void drawSkeleton();
		
		/** get public animation data */
		Animation &animation(int i) { return m_animation[i]; }

		
		//change animation
		void startAnimation(int anim, const OID &bone=Null);
		
		//local transformation matrix for each mesh
		matrix &transformMesh(Mesh *mesh);
		
		/** Get the transformation matrix of a bone */
		matrix &boneMatrix(const wgd::OID &boneName);
		
		private:
		/** Recursively calculate bone transformations */
		void calculateBone(const OID &boneName, const wgd::matrix &parent, int mask=0);

		//update specific animation
		bool update(int blend, float time);
		
		//final bone matrices
		matrix *m_transform;
		matrix *m_transformA;
		
		//current transformation
		vector3d *m_cscale;
		vector3d *m_cposition;
		quaternion *m_corientation;
		
		//animation blending
		struct Blend {
			float start;	//start frame
			float end;	//end frame
			wgd::OID next;	//next animation
			float frames;	//number of frames in animation
			float frame;	//current frame
			AnimationSet *set; //animationSet of this animation
			int root;	//root boneID
			float weight;	//blend amount
		} m_blend[MAX_ANIMATIONS];
		//public data for animation blending
		Animation m_animation[MAX_ANIMATIONS];

		
		//get blended scale
		void transform(int bid, int mask, vector3d &position, vector3d &scale, quaternion &orientation);
		
		
		//also needs to know where the IBone objects are
		//and the model bones and the model animations and animationsets
		wgd::Model *m_model;
		wgd::Instance3D *m_inst;
		
	};
	
	/**
	 * This class is used for the user to have direct control of the 
	 * bones in the model. When override is true, the bone will be controlled 
	 * by this class. There is no effect if override is false. <br>
	 * Bones can be manipulated by giving them a target to point at, or
	 * by specifying an orientation.
	 */
	class MODELSIMPORT IBone : public cadence::Agent {
		friend class AnimationController;
		public:
		
		IBone();
		IBone(const cadence::doste::OID &id);
		
		/** A point in world space that the bone will point at */
		PROPERTY_W(vector3d, target);
		
		/** Local orientation of a bone */
		PROPERTY_WF(vector3d, orientation, ix::orientation);
		PROPERTY_RF(vector3d, orientation, ix::orientation);
		
		/** Minimum orientation of a bone */
		PROPERTY_WF(vector3d, minimum, "min");
		PROPERTY_RF(vector3d, minimum, "min");
		
		/** Maximum orientation of a bone */
		PROPERTY_WF(vector3d, maximum, "max");
		PROPERTY_RF(vector3d, maximum, "max");
			
		/** Rotation speed (0=infinite) - used with target */
		PROPERTY_WF(float, speed, "speed");
		PROPERTY_RF(float, speed, "speed");
		
		/** Bone scale */
		PROPERTY_WF(wgd::vector3d, scale, ix::scale);
		PROPERTY_RF(wgd::vector3d, scale, ix::scale);
		void scale(float s) { scale(vector3d(s,s,s)); }

		/** Does the IBone override the animation */
		PROPERTY_W(bool, active);
		PROPERTY_R(bool, active);
		
		BEGIN_EVENTS(cadence::Agent);
		EVENT_VECTOR( evt_target, (*this)("target"));
		EVENT_VECTOR( evt_pitchyawRoll, (*this)(ix::orientation)); //(cadence::doste::modifiers::Seq)(*this)(ix::scale));
		END_EVENTS;
		
		OBJECT(Object, IBone);
		
		/** Used by IModel to get the transform matrix of this bone */
		const matrix &transform(const matrix&, const matrix&);
		
		private:
		
		//The two methods of calculating it
		
		void calcPYR();
		void calcTarget(const matrix&, const matrix&);
		
		//cache target
		wgd::vector3d m_target;
		bool m_useTarget;
		
		// user defined matrix for this bone //
		// or should be quaternion + scale ? //
		matrix m_matrix;
	};
};

#endif
