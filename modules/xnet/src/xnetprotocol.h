#ifndef _cadence_XNETPROTOCOL_H_
#define _cadence_XNETPROTOCOL_H_

#include <cadence/doste/event.h>
#include "xnet.h"

#include <cstring>
#include <cstdio>
#include <vector>
#include <map>

#define MAX_RESULTS 10
#define MAX_INDEXES 10000

#include "xnetconnection.h"

class XNetCallback {
	public:
	virtual ~XNetCallback() {}
	virtual void event(cadence::doste::Event &evt) {}
	virtual void script(const char *s) {};
	virtual void result(const char *r) {};
	virtual void error(const char *err) {};
};

struct XNet_Local {
	XNet_Header h;
	cadence::doste::OID local;
} __attribute__ ((packed));

struct XNet_Share {
	XNet_Header h;
	cadence::doste::OID share;
} __attribute__ ((packed));

struct XNet_Login {
	XNet_Header h;
	char username[20];
	char password[20];
} __attribute__ ((packed));

struct XNet_SmallEvent {
	XNet_Header h;
	unsigned char id;
	unsigned char type;
	cadence::doste::OID dest;
	cadence::doste::OID p1;
} __attribute__ ((packed));

struct XNet_Event {
	XNet_Header h;
	unsigned char id;
	unsigned char type;
	cadence::doste::OID dest;
	cadence::doste::OID p1, p2, p3, p4;
} __attribute__ ((packed));

struct XNet_Result {
	XNet_Header h;
	int id;
	cadence::doste::OID result;
} __attribute__ ((packed));

struct XNet_Block {
	XNet_Header h;
	int id;
	int size;
	//Might include type information.
	//Followed by 'size' OIDs.
} __attribute__ ((packed));

struct XNet_ILookup {
	XNet_Header h;
	cadence::doste::OID index;
} __attribute__ ((packed));

struct XNet_IValue {
	XNet_Header h;
	int id;
	char value[40];
} __attribute__ ((packed));

class TiXmlElement;

class XNETIMPORT XNetProtocol {
	public:
	XNetProtocol(XNetConnection *conn);
	virtual ~XNetProtocol();

	XNetConnection *connection() const { return m_conn; };

	//Allow for callbacks to overload default behaviour.
	void callback(XNetCallback *cb, int flags) { m_cb = cb; m_cbflags = flags; };
	XNetCallback *callback() const { return m_cb; };
	int callbackFlags() const { return m_cbflags; };

	void begin();
	void local(const cadence::doste::OID &base);
	void share(const cadence::doste::OID &share);
	void login(const char *user, const char *passwd);
	int event(cadence::doste::Event &evt, int flags);
	//void script(const char *d);
	//void results(bool v);
	//void errors(bool v);
	void result(int id, const cadence::doste::OID &res);
	void ilookup(const cadence::doste::OID &index);
	void ivalue(int id, const char *value);
	void end();
	void endResponse();
	
	bool wait(int id) { return m_results[id].waiting; };
	cadence::doste::OID getResult(int id) {
		cadence::doste::OID res = m_results[id].res;
		m_results[id].avail = true;
		return res;
	};

	virtual void data(const char *d, int length);

	cadence::doste::OID lookup(const cadence::doste::OID &index);
	
	static const int EFLAG_WAITRESULT = 0x0001;

	static const int CBFLAG_RESULT = 0x01;
	static const int CBFLAG_ERROR = 0x02;
	static const int CBFLAG_EVENT = 0x03;
	static const int CBFLAG_SCRIPT = 0x04;
	
	static const unsigned char MSG_LOCAL = 0x01;
	static const unsigned char MSG_SHARE = 0x02;
	static const unsigned char MSG_LOGIN = 0x03;
	static const unsigned char MSG_SMALLEVT = 0x04;
	static const unsigned char MSG_EVENT = 0x05;
	static const unsigned char MSG_RESULT = 0x06;
	static const unsigned char MSG_BLOCK = 0x07;
	static const unsigned char MSG_ILOOKUP = 0x08;
	static const unsigned char MSG_IVALUE = 0x09;

	private:
	XNetConnection *m_conn;
	//std::string m_data;
	XNetCallback *m_cb;
	int m_cbflags;

	/*class XMLTag {
		public:
		XMLTag(const char *name, XMLTag *parent=0)
			: m_parent(parent), m_name(name) {};
		~XMLTag();

		const char *name() const { return m_name.data(); };
		std::string &attribute(const char *name) { return m_attribs[name]; };
		void addAttribute(const char *name, const char *value) { m_attribs[name] = value; };
		void appendContent(const char *content) { m_content += content; };
		const char *content() const { return m_content.data(); };
		void addChild(XMLTag *tag) { m_children.push_back(tag); };
		int numChildren() const { return m_children.size(); };
		XMLTag *child (int c) { return m_children[c]; };
		XMLTag *parent() const { return m_parent; };

		private:
		XMLTag *m_parent;
		std::string m_content;
		std::string m_name;
		std::vector<XMLTag*> m_children;
		//Something for attributes.
		std::map<std::string, std::string> m_attribs;
	};*/

	//XMLTag *m_root;
	//XMLTag *m_current;
	int m_datasize;
	char m_data[1000];
	char *m_data2;
	cadence::doste::OID m_this;
	
	void append(const char *str) {
		strcpy(m_data2, str);
		m_data2 += strlen(m_data2);
	}
	
	void append(int n) {
		sprintf(m_data2, "%d", n);
		m_data2 += strlen(m_data2);
	}
	
	void append(const cadence::doste::OID &o) {
		char buf[100];
		o.toString(buf,100);
		if (buf[0] == '<') buf[0] = '[';
		strcpy(m_data2, buf);
		m_data2 += strlen(m_data2);
	}
	
	struct ResultEntries {
		bool waiting;
		bool avail;
		cadence::doste::OID res;
	};
	
	ResultEntries m_results[MAX_RESULTS];

	void onLocal(TiXmlElement *x);
	void onEvent(TiXmlElement *x);
	//void onScript(char *buf);
	void onLogin(TiXmlElement *x);
	void onResult(TiXmlElement *x);
	//void onError(char *buf);
	void onShare(TiXmlElement *x);
	void onBlock(TiXmlElement *x);
	void onSmallEvent(TiXmlElement *x);
	void onILookup(TiXmlElement *x);
	void onIValue(TiXmlElement *x);
	//Cache stuff.

	//int parseTag(const char *buf);
	//int parseAttributes(const char *d);
	//int countWS(const char *d);
	//void executeTree();
	//void deleteTree();

	int m_indexes[MAX_INDEXES];	//NOTE: Max of 10000 indexes!!!
	
};


#endif
