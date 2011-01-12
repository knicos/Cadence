#ifndef _WGD_CAMERA_
#define _WGD_CAMERA_

#include <cadence/agent.h>

#ifdef _MSC_VER
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#endif

namespace wgd {
	class Camera : public cadence::Agent {
		public:
		Camera(const cadence::doste::OID &id) : Agent(id) {};
		virtual ~Camera() {};
		
		virtual void bind() = 0;
		virtual void unbind() = 0;

		void size(int w, int h){ m_width = w; m_height=h; };
		int width() const { return m_width; }
		int height() const { return m_height; }
		
		VOBJECT(Agent, Camera);

		protected:
		int m_width, m_height;
		
	};
};

#endif
