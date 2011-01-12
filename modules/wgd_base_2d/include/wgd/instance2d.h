#ifndef _WGD_INSTANCE2D_
#define _WGD_INSTANCE2D_

#include <wgd/vector.h>
#include <cadence/agent.h>
#include <wgd/scenegraph.h>
#include <wgd/camera2d.h>

namespace wgd {
	class BASE2DIMPORT Instance2D : public cadence::Agent {
		public:
		OBJECT(Agent, Instance2D);
		
		Instance2D();
		Instance2D(const cadence::doste::OID &id);
		virtual ~Instance2D();
		
		virtual void draw(SceneGraph &graph, Camera2D *camera);
		
		PROPERTY_RF(vector2d, position, ix::position);
		PROPERTY_WF(vector2d, position, ix::position);
		
		void move(const vector2d&);
		void translate(const vector2d&);
		vector2d worldPosition();
		 
		PROPERTY_RF(float, orientation, ix::orientation);
		PROPERTY_WF(float, orientation, ix::orientation);
		
		void rotate(float);
		float worldOrientation();

		/** Children */
		PROPERTY_RF(cadence::doste::OID, children, ix::children);
		PROPERTY_WF(cadence::doste::OID, children, ix::children);
		
		/** parent instance */
		PROPERTY_R(Instance2D, parent);
		PROPERTY_W(Instance2D, parent);

	};
};

#endif