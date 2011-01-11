#ifndef _cadence_XNETHANDLER_H_
#define _cadence_XNETHANDLER_H_

#include <cadence/doste/handler.h>
#include <cadence/doste/event.h>
#include <cadence/list.h>
#include <cadence/vector.h>

class XNetConnection;
class XNetCache;

class XNetHandler : public cadence::doste::Handler {
	public:
	XNetHandler(XNetConnection *conn, const cadence::doste::OID &base);
	~XNetHandler();

	bool handle(cadence::doste::Event &evt);
	
	private:
		
	XNetConnection *m_conn;
	XNetCache *m_cache;
};

#endif
