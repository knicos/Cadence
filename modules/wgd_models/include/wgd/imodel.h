#ifndef _WGD_IMODEL_
#define _WGD_IMODEL_

#include <wgd/common.h>
#include <wgd/instance3d.h>
#include <wgd/model.h>
#include <wgd/animationcontroller.h>
#include <wgd/morphcontroller.h>
#include <wgd/skincontroller.h>

namespace wgd {
	
	class MODELSIMPORT IModel : public wgd::Instance3D {
		public:
		IModel();
		IModel(Model* mdl);
		IModel(const cadence::doste::OID&);
		~IModel();
		
		OBJECT(Instance3D, IModel);
		
		/** The model this instance is of */
		PROPERTY_RF(Model, model, ix::model);
		PROPERTY_WF(Model, model, ix::model);		
		
		/** Get a material by name */
		wgd::Material* material(const cadence::doste::OID &);
		
		/** Object scale (in each dimension) */
		PROPERTY_RF(vector3d, scale, ix::scale);
		PROPERTY_WF(vector3d, scale, ix::scale);
		
		/** set the current animation, using default transition time */
		PROPERTY_RF(OID, animation, ix::animation);
		PROPERTY_WF(OID, animation, ix::animation);
		
		/** get the animation object to set multiple blended animations */
		Animation *animation(int index) {
			if(m_anim) return &m_anim->animation(index);
			return 0;
		}
		
		/** Object scale */
		void scale(float s) { scale(vector3d(s,s,s)); };
		
		virtual void draw(SceneGraph &graph, Camera3D *camera);
		
		/** get global transform matrix for a named bone */
		matrix boneMatrix(const OID &boneName);
		
		/** get an IBone object from the model. 
		 * Creates the IBone object if it dosent exist.
		 * IBones override any animation when active
		 * @return IBone object, or NULL if bone does not exist
		 */
		IBone* bone(const OID& bone);
		
		BEGIN_EVENTS(Instance3D);
		EVENT(evt_update, WGD(ix::time)); //Need something to trigger the update function - preferably not every instant
		EVENT(evt_setup, (*this)(ix::model)("loaded"));
		EVENT(evt_animation, (*this)(ix::animation));
		EVENT(evt_morph, (*this)(ix::meshes)(cadence::doste::All));
		EVENT(evt_bone, Null); //For the bones.
		END_EVENTS;
		
		bool handler(int);
		
		protected:
		
		//the model VBOs are built
		bool m_built;
		
		//a bone has been changed - force update
		bool m_forceUpdate;
		
		//controlers
		AnimationController *m_anim;
		MorphController *m_morph;
		SkinController *m_skin;
		
	};
	
};

#endif
