#ifndef _WGD_INPUT_
#define _WGD_INPUT_

#include <cadence/agent.h>
#include <wgd/keyboard.h>
#include <wgd/dll.h>

namespace wgd {
	class BASEIMPORT Input : public cadence::Agent {
		public:
		OBJECT(Agent,Input);

		Input(const cadence::doste::OID &o);
		~Input();
		
		static void update ();
	};
};

#endif
