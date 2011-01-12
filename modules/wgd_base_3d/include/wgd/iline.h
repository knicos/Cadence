#ifndef _WGD_ILINE_H_
#define _WGD_ILINE_H_

#include <wgd/instance3d.h>
#include <wgd/vector.h>
#include <wgd/colour.h>

namespace wgd {
	
	class BASE3DIMPORT ILine3D : public wgd::Instance3D {
		public:
		ILine3D(const cadence::doste::OID&);
		~ILine3D();
		
		OBJECT(Instance3D, ILine3D);
		
		PROPERTY_RF(vector3d, point1, "point1");
		PROPERTY_WF(vector3d, point1, "point1");
		
		PROPERTY_RF(vector3d, point2, "point2");
		PROPERTY_WF(vector3d, point2, "point2");
		
		PROPERTY_RF(wgd::colour, colour, "colour");
		PROPERTY_WF(wgd::colour, colour, "colour");
		
		PROPERTY_RF(int, width, "width");
		PROPERTY_WF(int, width, "width");
		
		virtual void draw(SceneGraph &graph, Camera3D *camera);
	};
	
};

#endif 
