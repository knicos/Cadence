#ifndef _doste_XNETCONNECTION_H_
#define _doste_XNETCONNECTION_H_

#include <cadence/doste/oid.h>
#include <cadence/agent.h>
#include "xnet.h"

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
typedef int socklen_t;
#define MSG_WAITALL 0
#endif

class XNetProtocol;
class XNetHandler;

struct XNet_Header {
	unsigned char id;	//Message type
	unsigned short size;
} __attribute__ ((packed));

class XNetConnection : public cadence::Agent {
	public:
	
	OBJECT(Agent, XNetConnection);
	
	XNetConnection(const cadence::doste::OID &o);
	XNetConnection(const cadence::doste::OID &o, int sck);
	~XNetConnection();
	
	PROPERTY_RF(bool, connect, "connect");
	PROPERTY_WF(bool, connect, "connect");
	
	PROPERTY_RF(cadence::dstring, address, "address");
	PROPERTY_WF(cadence::dstring, address, "address");
	
	PROPERTY_RF(int, port, "port");
	PROPERTY_WF(int, port, "port");
	
	PROPERTY_RF(cadence::doste::OID, local, "local");

	PROPERTY_RF(cadence::dstring, username, "username");
	PROPERTY_WF(cadence::dstring, username, "username");
	PROPERTY_RF(cadence::dstring, password, "password");
	PROPERTY_WF(cadence::dstring, password, "password");
	
	BEGIN_EVENTS(Agent);
	EVENT(evt_connect, (*this)("connect"));
	END_EVENTS;

	//const doste::doste::OID &local() { return m_local; };
	void send(const char *data, int length);
	XNetProtocol *protocol() const { return m_proto; };
	bool ssDependency() { return true; };
	XNetHandler *xhandler() const { return m_handler; };
	void xhandler(const cadence::doste::OID &base);

	static XNetConnection *lookup(const cadence::doste::OID &o);
	static void listen(const cadence::doste::OID &base);
	static void stop();
	static void update();

	static const int MAX_CONNECTIONS = 10;

	private:
	
	bool _connect(const char *addr, int port);
	void _disconnect();
	
	int sock() { return m_socket; };
	bool data();	//Process XML input data.
	void error();	//Urrr... probably disconnected.
	
	int m_socket;
	char m_addr[15];
	cadence::doste::OID m_local;
	XNetProtocol *m_proto;
	int m_bufsize;
	int m_bpos;
	char *m_buffer;
	XNetHandler *m_handler;
	
	//#ifndef WIN32
	//static void *listen_thread(void*);
	//#else
	//static DWORD listen_thread(void*);
	//#endif
	static bool acceptConnections();
	static int setDescriptors();
	static bool initServer(short port);
	
	static XNetConnection *s_conns[MAX_CONNECTIONS];
	static cadence::doste::OID s_base;
	//#ifndef WIN32
	//static pthread_t s_thread;
	//#else
	//static HANDLE s_thread;
	//#endif
	static bool s_running;
	static bool s_server;
	static int s_sd;
	static fd_set s_fdread;
	static fd_set s_fderror;
	static sockaddr_in s_localAddr;
};

#endif
