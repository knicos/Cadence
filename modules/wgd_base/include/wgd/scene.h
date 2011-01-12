#ifndef _WGD_SCENE_
#define _WGD_SCENE_

#include <cadence/agent.h>
#include <wgd/index.h>

#include <wgd/drawable.h>
#include <vector>


#ifdef _MSC_VER
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#endif

namespace wgd {

	
	//for the morton keys
	typedef unsigned long uint;
	class Camera;

	class RESIMPORT Scene : public cadence::Agent {
		public:
		Scene(const cadence::doste::OID &id) : Agent(id) { };
		virtual ~Scene() { };
		
		virtual void draw(Camera *) = 0;
		
		
		VOBJECT(Agent, Scene);
	};
};


#endif
