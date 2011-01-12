#ifndef _WGD_INSTANCE3D_
#define _WGD_INSTANCE3D_

#include <wgd/scenegraph.h>
#include <wgd/camera3d.h>
#include <wgd/vector.h>
#include <wgd/matrix.h>
#include <wgd/quaternion.h>

#include <cadence/agent.h>
#include <cadence/doste/modifiers.h>


namespace wgd {
	class BASE3DIMPORT Instance3D : public cadence::Agent {
		public:
		Instance3D();
		Instance3D(const cadence::doste::OID &id);
		virtual ~Instance3D(){};
		
		/** must be called by all instances to update local data */
		virtual void draw(SceneGraph &graph, Camera3D *camera);
		
		OBJECT(Agent, Instance3D);
		
		PROPERTY_RF(vector3d, position, ix::position);
		PROPERTY_WF(vector3d, position, ix::position);

		void move(const vector3d &v) { position(position()+v); };
		void translate(const vector3d &v) { position(position().translate(m_quat.conjugate(), v)); };
		vector3d worldPosition();
		
		PROPERTY_RF(vector3d, orientation, ix::orientation);
		PROPERTY_WF(vector3d, orientation, ix::orientation);
		
		void rotate(const vector3d &r) { orientation(orientation() + r); };
		vector3d worldOrientation();
		
		/** Quaternions */
		const Quaternion &quaternion() const {return m_quat; };
		void quaternion(const Quaternion &q) { m_quat=q; };
		
		//Get object AABB information for Scene
		wgd::vector3d boundingBoxLow() { return m_low; };
		wgd::vector3d boundingBoxHigh() { return m_high; };

		/** Children */
		PROPERTY_RF(cadence::doste::OID, children, ix::children);
		PROPERTY_WF(cadence::doste::OID, children, ix::children);
		
		/** parent instance */
		PROPERTY_R(Instance3D, parent);
		PROPERTY_W(Instance3D, parent);

		BEGIN_EVENTS(Agent);
		EVENT_VECTOR(evt_orientation,	(*this)(ix::orientation));
		EVENT_VECTOR(evt_position,	(*this)(ix::position));
		END_EVENTS;
		
		const matrix &localMatrix();
		void localMatrix(const matrix& m);
		
		private:
		//AABB for object
		wgd::vector3d m_low, m_high;
		
		bool m_update;
		
		//quaternion
		Quaternion m_quat;
		
		
		matrix m_localMatrix;
	};
};

#endif
