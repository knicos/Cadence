#include "xnetconnection.h"
#include "xnetprotocol.h"
#include "xnethandler.h"
#include <cadence/messages.h>
#include <string.h>

#ifndef WIN32
#include <errno.h>
#include <fcntl.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#undef ERROR

using namespace cadence;
using namespace cadence::doste;

XNetConnection *XNetConnection::s_conns[MAX_CONNECTIONS] = {0};
OID XNetConnection::s_base;
//#ifndef WIN32
//pthread_t XNetConnection::s_thread;
//#else
//HANDLE XNetConnection::s_thread;
//#endif
bool XNetConnection::s_running = false;
bool XNetConnection::s_server = false;
int XNetConnection::s_sd;
fd_set XNetConnection::s_fdread;
fd_set XNetConnection::s_fderror;
sockaddr_in XNetConnection::s_localAddr;

OnEvent(XNetConnection, evt_connect) {
	//DMsg dbg(DMsg::DEBUG);
	//dbg << "evt_connected: " << get("address") << ", " << get("port") << ", " << get("connect") << "\n";
	if (XNetConnection::connect()) {
		if (m_socket >= 0) {
			//#ifdef LINUX
			//fcntl(m_socket, F_SETFL, O_NONBLOCK);
			//#endif
			//Send handshake data.
			//protocol()->begin();
			//protocol()->local(OID::local());
			//protocol()->end();
		} else {
			if (!_connect(address(),port())) {
				char *buf = new char[1000];
				strcpy(buf, "XNetConnection: Failed to connect to '");
				strcat(buf, (const char*)address());
				Error(0, buf);
				delete [] buf;
				return;
			}
		}
	} else {
		//Disconnect
		#ifndef WIN32
		close(m_socket);
		#else
		closesocket(m_socket);
		#endif
		m_socket = -1;
	}
}

IMPLEMENT_EVENTS(XNetConnection, Agent);

XNetConnection::XNetConnection(const OID &o) : Agent(o) {

	m_local = o;
	m_socket = -1;
	m_bpos = 0;
	m_handler = 0;
	m_bufsize = 10000;
	m_buffer = new char[10000];
	//std::cout << "NEW LOCAL CONNECTION\n";
	m_proto = new XNetProtocol(this);

	registerEvents();
}

XNetConnection::XNetConnection(const OID &o, int sck) : Agent(o) {
	m_local = o;
	m_socket = sck;
	m_bpos = 0;
	m_handler = 0;
	m_bufsize = 10000;
	m_buffer = new char[10000];
	//std::cout << "NEW REMOTE CONNECTION\n";
	//#ifdef LINUX
	//fcntl(m_socket, F_SETFL, O_NONBLOCK);
	//#endif
	m_proto = new XNetProtocol(this);
	connect(true);

	registerEvents();
}

XNetConnection::~XNetConnection() {
	#ifndef WIN32
	close(m_socket);
	#else
	closesocket(m_socket);
	#endif

	delete m_buffer;
	delete m_proto;
	if (m_handler != 0)
		delete m_handler;
}

XNetConnection *XNetConnection::lookup(const OID &o) {
	for (int i=0; i<MAX_CONNECTIONS; i++) {
		if (s_conns[i] && s_conns[i]->local() == o) {
			return s_conns[i];
		}
	}
	return 0;
}

void XNetConnection::xhandler(const OID &base) {
	m_handler = new XNetHandler(this, base);
}

void XNetConnection::stop() {
	s_running = false;

	for (int i=0; i<MAX_CONNECTIONS; i++) {
		#ifndef WIN32
		if (s_conns[i]) close(s_conns[i]->sock());
		#else
		if (s_conns[i]) closesocket(s_conns[i]->sock());
		#endif
	}

	//#ifndef WIN32
	//pthread_join(s_thread, 0);
	//#else

	//#endif
	
	for (int i=0; i<MAX_CONNECTIONS; i++) {
		if (s_conns[i]) delete s_conns[i];
	}
	
	#ifndef WIN32
	close(s_sd);
	#else
	closesocket(s_sd);
	#endif
}

bool XNetConnection::initServer(short port) {
	#ifdef WIN32
	WSAData wsaData;
	//If Win32 then load winsock
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
		Error(0, "XNet: Could not start WinSock");

		return false;
	}
	#endif

	s_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (s_sd == INVALID_SOCKET) {
		Error(0, "XNet: Socket Error.");

		return false;
	}

	//Specify listen port and address
	//memset(&s_localAddr, 0, sizeof(s_localAddr));
	s_localAddr.sin_family = AF_INET;
	s_localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_localAddr.sin_port = htons(port);
	
	int rc = bind(s_sd, (struct sockaddr*)&s_localAddr, sizeof(s_localAddr));
	
	if (rc == SOCKET_ERROR) {
		Error(0, "XNet: Socket Bind Error");
		
		#ifndef WIN32
		close(s_sd);
		#else
		closesocket(s_sd);
		#endif
		return false;
	}

	//Attempt to start listening for connection requests.
	rc = ::listen(s_sd, 1);

	if (rc == SOCKET_ERROR) {
		Error(0, "XNet: Socket Listen Error.");

		#ifndef WIN32
		close(s_sd);
		#else
		closesocket(s_sd);
		#endif
		return false;
	}
	
	return true;
}

void XNetConnection::listen(const OID &base) {
	s_base = base;
	int port = s_base["port"];

	for (int i=0; i<MAX_CONNECTIONS; i++) {
		s_conns[i] = 0;
	}
	
	if (!initServer(port)) return;
	s_running = true;
	s_server = true;

	Info(0, "XNet: Listening");
	//std::cout.flush();
	
	//#ifndef WIN32
	//pthread_create(&s_thread, 0, listen_thread, 0);
	//#else
	//s_thread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)listen_thread,0,0,0);
	//#endif
	
	//Make thread to actually accept / process connections.
}

bool XNetConnection::acceptConnections() {

	int freeclient = -1;
	int rsize = sizeof(sockaddr_in);
	sockaddr_in remoteAddr;

	//Find a free client slot and allocated it
	for (int i=0; i<MAX_CONNECTIONS; i++) {
		if (s_conns[i] == 0) {
			freeclient = i;
			break;
		}
	}

	//Max clients reached, so send error
	if (freeclient == -1) {
		Error(0, "XNet: Exceeded maximum supported client connections");
		return false;
	}

	//Finally accept this client connection.
	int csock = accept(s_sd, (sockaddr*)&remoteAddr, (socklen_t*)&rsize);

	if (csock == INVALID_SOCKET) {
		Error(0, "XNet: Could not accept client connection.");
		return false;
	}
	
	XNetConnection *newcon = new XNetConnection(s_base, csock);
	s_conns[freeclient] = newcon;
	//newcon->m_socket = csock;
	
	//Save the ip address
	#ifdef WIN32
	strcpy(newcon->m_addr, inet_ntoa(remoteAddr.sin_addr));
	#else
	strcpy(newcon->m_addr, inet_ntoa(remoteAddr.sin_addr));
	#endif

	return true;
}

int XNetConnection::setDescriptors() {
	//Reset all file descriptors
	FD_ZERO(&s_fdread);
	FD_ZERO(&s_fderror);

	int n = 0;

	//Set file descriptor for the listening socket.
	if (s_server) {
		FD_SET(s_sd, &s_fdread);
		FD_SET(s_sd, &s_fderror);
		n = s_sd;
	}

	//Set the file descriptors for each client
	for (int i=0; i<MAX_CONNECTIONS; i++) {
		#ifdef WIN32
		if (s_conns[i] && s_conns[i]->sock() != INVALID_SOCKET) {
		#else
		if (s_conns[i] && s_conns[i]->sock() >= 0) {
		#endif
			if (s_conns[i]->sock() > n) {
				n = s_conns[i]->sock();
			}

			FD_SET(s_conns[i]->sock(), &s_fdread);
			FD_SET(s_conns[i]->sock(), &s_fderror);
		}
	}

	return n;
}

//#ifndef WIN32
//void *XNetConnection::listen_thread(void *p) {
//#else
//DWORD XNetConnection::listen_thread(void *p) {
//#endif
void XNetConnection::update() {
	timeval block;
	int n;
	int selres = 1;
	int count = 0;
	
	//std::cout << "Thread Started\n";

	if (!s_running) return;

	while (true) {
		count++;
		n = setDescriptors();

		//Wait for a network event or timeout in 3 seconds
		block.tv_sec = 0;
		block.tv_usec = 0;
		selres = select(n+1, &s_fdread, 0, &s_fderror, &block);
		//Exit loop if connection termination requested.
		//if (!s_running) {
		//	break;
		//}

		//Some kind of error occured, it is usually possible to recover from this.
		if (selres <= 0) {
			//DMsg msg(DMsg::WARNING);
			//msg << "XNet: Select error.\n";
			return;
		}

		//If connection request is waiting
		if (s_server) {
			if (FD_ISSET(s_sd, &s_fdread)) {
				if (!acceptConnections()) {
					return;
				}
			}
		}

		//Also check each clients socket to see if any messages or errors are waiting
		for (int i=0; i<MAX_CONNECTIONS; i++) {
			#ifdef WIN32
			if (s_conns[i] && s_conns[i]->sock() != INVALID_SOCKET) {
			#else
			if (s_conns[i] && s_conns[i]->sock() >= 0) {
			#endif
				//If message received from this client then deal with it
				if (FD_ISSET(s_conns[i]->sock(), &s_fdread)) {
					s_conns[i]->data();
				//An error occured with this client.
				} else if (FD_ISSET(s_conns[i]->sock(), &s_fderror)) {
					s_conns[i]->error();
				}
			}
		}
	}
	
	return;
}

void XNetConnection::send(const char *data, int length) {
	if (::send(m_socket, data, length,0) != length) {
		Warning(0, "An Xnet message did not send properly");
	}
}

bool XNetConnection::data() {
	//Read data from socket
	int rc = 0;
	int length;
	rc = recv(m_socket, (char*)&length, sizeof(int), 0);
	
	//while (rc > 0) {
		//Check that rc == 4
		if (rc == sizeof(int)) {
			rc = recv(m_socket, m_buffer, length, MSG_WAITALL);
			//Check that rc == length
			if (rc == length) {
				m_proto->data(m_buffer, length);
				//std::cout << m_buffer;
			} else {
				Warning(0, "Connection problem");
			}
		} else {
		}
		
		//rc = recv(m_socket, (char*)&length, sizeof(int), MSG_DONTWAIT);
	//}

	if (rc < 0) {
		Info(0, "XNet: Disconnected.");
		#ifndef WIN32
		close(m_socket);
		#else
		closesocket(m_socket);
		#endif
		m_socket = INVALID_SOCKET;
		return false;
	}
	
	return true;
}

void XNetConnection::error() {

}

bool XNetConnection::_connect(const char *addr, int port) {
	int rc;
	sockaddr_in destAddr;

	//DMsg dbg(DMsg::DEBUG);
	//dbg << "XnetConnection: Connecting to '" << addr << ":" << port << "'\n";

	#ifdef WIN32
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
		//ERROR
		return false;
	}
	#endif
	
	//We want a TCP socket
	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (m_socket == INVALID_SOCKET) {
		return false;
	}

	#ifdef WIN32
	HOSTENT *host = gethostbyname(addr);
	#else
	hostent *host = gethostbyname(addr);
	#endif

	destAddr.sin_family = AF_INET;
	destAddr.sin_addr.s_addr = ((in_addr *)(host->h_addr))->s_addr;
	destAddr.sin_port = htons(port);
	
	rc = ::connect(m_socket, (struct sockaddr*)&destAddr, sizeof(destAddr));

	if (rc == SOCKET_ERROR) {
		#ifndef WIN32
		close(m_socket);
		#else
		closesocket(m_socket);
		#endif

		return false;
	}

	int freeclient = -1;

	//Find a free client slot and allocated it
	for (int i=0; i<MAX_CONNECTIONS; i++) {
		if (s_conns[i] == 0) {
			freeclient = i;
			break;
		}
	}

	//Max clients reached, so send error
	if (freeclient == -1) {
		Error(0, "XNet: Exceeded maximum supported connections\n");
		return false;
	}

	s_conns[freeclient] = this;

	if (!s_running) {
		s_running = true;

		//#ifndef WIN32
		//pthread_create(&s_thread, 0, listen_thread, 0);
		//#else
		//s_thread = CreateThread(0,0,(LPTHREAD_START_ROUTINE)listen_thread,0,0,0);
		//#endif
	}
	
	//Send handshake data.
	protocol()->begin();
	protocol()->local(OID::local());
	protocol()->login(username(), password());
	protocol()->end();

	return true;
}
