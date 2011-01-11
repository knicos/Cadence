#ifndef _doste_DVM_TEMPLATES_H_
#define _doste_DVM_TEMPLATES_H_

#include <doste/dvm/handler.h>
#include <doste/dvm/oid.h>

namespace doste {
	namespace dvm {
		class Templates : public Handler {
			public:
			Templates();
			~Templates();
			
			bool handle(Event &evt);
			
			private:
		};
	};
};

#endif