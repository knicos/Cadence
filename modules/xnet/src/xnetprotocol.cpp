#include "xnetprotocol.h"
#include "xnetconnection.h"
#include <cadence/cadence.h>
#include <cadence/dstring.h>
#include <cadence/messages.h>
#include <cadence/doste/buffer.h>
#include <string.h>

#include "tinyxml.h"

#undef ERROR

using namespace cadence;
using namespace cadence::doste;

/*struct EvtTypeToString {
	int type;
	const char *name;
};*/

/*EvtTypeToString cnvtype[] = {
	{Event::SET, "SET"},
	{Event::GET, "GET"},
	{Event::ADDDEP, "ADDDEP"},
	//{Event::ADDDEP_AGENT, "ADDDEP_AGENT"},
	{Event::NOTIFY, "NOTIFY"},
	{Event::DEFINE, "DEFINE"},
	{Event::DEFINE_FUNC, "DEFINE_FUNC"},
	{Event::GET_KEYS, "GET_KEYS"},
	{0,0}
};*/

XNetProtocol::XNetProtocol(XNetConnection *conn)
 : m_conn(conn), m_datasize(0) {
	m_this = Void;
	
	for (int i=0; i<MAX_RESULTS; i++)
		m_results[i].avail = true;
		
	for (int i=0; i<MAX_INDEXES; i++) {
		m_indexes[i] = -1;
	}
}

XNetProtocol::~XNetProtocol() {

}

void XNetProtocol::begin() {
	//Generate <doste> tag
	m_data2 = m_data+sizeof(int);
	m_data2[0] = 0;
	//append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	//append("<!DOCTYPE cadence SYSTEM \"http://www.doste.co.uk/cadence.dtd\">\n");
	append("<cadence>\n\0");
}

void XNetProtocol::end() {
	append("</cadence>\n");
	int *length = (int*)m_data;
	*length = m_data2-m_data + strlen(m_data2)+1 - sizeof(int);
	m_conn->send(m_data, (*length) + sizeof(int));
}

void XNetProtocol::endResponse() {
	append("</cadence>\n");
	int *length = (int*)m_data;
	*length = m_data2-m_data + strlen(m_data2)+1 - sizeof(int);
	m_conn->send(m_data, (*length) + sizeof(int));
}

void XNetProtocol::local(const OID &base) {
	/*XNet_Local *l = (XNet_Local*)&m_data[m_datasize];
	l->h.id = MSG_LOCAL;
	l->h.size = sizeof(XNet_Local);
	l->local = base;
	m_datasize += sizeof(XNet_Local);*/
	
	append("\t<local>");
	append(base);
	append("\t</local>\n");
}

void XNetProtocol::ilookup(const OID &index) {
	//std::cout << "ilookup\n";
	/*XNet_ILookup *l = (XNet_ILookup*)&m_data[m_datasize];
	l->h.id = MSG_ILOOKUP;
	l->h.size = sizeof(XNet_ILookup);
	l->index = index;
	m_datasize += sizeof(XNet_ILookup);*/
	
	append("<ilookup>");
	append(index);
	append("</ilookup>\n");
}

void XNetProtocol::ivalue(int id, const char *value) {
	/*XNet_IValue *l = (XNet_IValue*)&m_data[m_datasize];
	l->h.id = MSG_IVALUE;
	l->h.size = sizeof(XNet_IValue);
	l->id = id;
	strcpy(l->value, value);
	m_datasize += sizeof(XNet_IValue);*/
	
	append("<ivalue>\n");
	append("<id>");
	append(id);
	append("</id>\n");
	append("<value>");
	append(value);
	append("</value>\n");
	append("<ivalue>\n");
}

void XNetProtocol::share(const OID &s) {
	/*XNet_Share *l = (XNet_Share*)&m_data[m_datasize];
	l->h.id = MSG_SHARE;
	l->h.size = sizeof(XNet_Share);
	l->share = s;
	m_datasize += sizeof(XNet_Share);*/
	
	append("\t<share>");
	append(s);
	append("\t</share>\n");
}

void XNetProtocol::result(int id, const OID &res) {
	//std::cout << "result\n";
	if (res.isBuffer()) {
		/*XNet_Block *b = (XNet_Block*)&m_data[m_datasize];
		b->h.id = MSG_BLOCK;
		b->id = id;
		Buffer *buf = Buffer::lookup(res);
		b->size = buf->count();
		b->h.size = sizeof(XNet_Block) + buf->count()*sizeof(OID);

		//std::cout << "block size= " << b->size << "\n";
		
		//DMsg msg(DMsg::DEBUG);
		OID *element = (OID*)(b+1);
		for (int i=0; i<b->size; i++) {
			element[i] = buf->get(i);
			//msg << " sending element " << element[i] << "\n";
		}
		
		Buffer::free(res);
		
		m_datasize += sizeof(XNet_Block) + (sizeof(OID) * b->size);*/
		
		append("\t<resultblock id=\"");
		append(id);
		append("\">\n");
		append("\t\t<count>");
		
		Buffer *buf = Buffer::lookup(res);
		
		append(buf->count());
		append("</count>\n");
		
		for (int i=0; i<buf->count(); i++) {
			append("\t\t<element>");
			append(buf->get(i));
			append("</element>\n");
		}
		Buffer::free(res);
		
		append("\t</resultblock>\n");
	} else {
		/*XNet_Result *r = (XNet_Result*)&m_data[m_datasize];
		r->h.id = MSG_RESULT;
		r->h.size = sizeof(XNet_Result);
		r->id = id;
		r->result = res;
		m_datasize += sizeof(XNet_Result);*/
		
		append("\t<result id=\"");
		append(id);
		append("\">");
		append(res);
		append("</result>\n");
	}
}

int XNetProtocol::event(Event &evt, int flags) {
	//Convert to event tags
	//DMsg msg(DMsg::INFO);
	//msg << "send event: " << evt.type() << " " << evt.dest() << "(" << evt.param<0>() << ")\n";
	unsigned char id=0xFF;
	
	if (flags & EFLAG_WAITRESULT) {
		for (id=0; id<MAX_RESULTS; id++)
			if (m_results[id].avail) break;

		m_results[id].avail = false;
		m_results[id].waiting = true;
	}
	
	/*XNet_Event *e = (XNet_Event*)&m_data[m_datasize];
	e->h.id = MSG_EVENT;
	e->h.size = sizeof(XNet_Event);
	e->id = id;
	e->type = evt.type();
	e->dest = evt.dest();
	e->p1 = evt.param<0>();
	e->p2 = evt.param<1>();
	e->p3 = evt.param<2>();
	e->p4 = evt.param<3>();
	m_datasize += sizeof(XNet_Event);*/
	
	append("\t<event id=\"");
	append(id);
	append("\">\n");
	append("\t\t<type>");
	append(evt.type());
	append("</type>\n");
	append("\t\t<destination>");
	append(evt.dest());
	append("</destination>\n");
	append("\t\t<parameter id=\"0\">");
	append(evt.param<0>());
	append("</parameter>\n");
	append("\t\t<parameter id=\"1\">");
	append(evt.param<1>());
	append("</parameter>\n");
	append("\t\t<parameter id=\"2\">");
	append(evt.param<2>());
	append("</parameter>\n");
	append("\t\t<parameter id=\"3\">");
	append(evt.param<3>());
	append("</parameter>\n");
	append("\t</event>\n");
	
	return id;
}

void XNetProtocol::login(const char *user, const char *passwd) {
	//std::cout << "login\n";
	/*XNet_Login *l = (XNet_Login*)&m_data[m_datasize];
	l->h.id = MSG_LOGIN;
	l->h.size = sizeof(XNet_Login);
	strcpy(l->username, user);
	strcpy(l->password, passwd);
	m_datasize += sizeof(XNet_Login);*/
	
	append("\t<login>\n");
	append("\t\t<username>");
	append(user);
	append("</username>\n");
	append("\t\t<password>");
	append(passwd);
	append("</password>\n");
	append("\t</login>\n");
}

void printhex(const char *d, int count) {
	std::cout << std::hex;
	for (int i=0; i<count; i++) {
		std::cout << (int)d[i] << " ";
	}
	std::cout << "\n";
}

void XNetProtocol::data(const char *d, int length) {
	//std::cout << "\nXNetProtocol Data:\n" << d << "\n";
	//DMsg msg(DMsg::DEBUG);
	//msg << "\nData:\n" << d << "\n";
	
	//std::cout << "DATA: " << d << "\n";

	TiXmlDocument doc;
	doc.Parse(d);
	TiXmlNode *el = doc.RootElement();
	if (strcmp(el->Value(), "cadence") != 0) {
			std::cout << "XNet: Invalid XML data\n";
			return;
	}
	
	for (el = el->FirstChild(); el != 0; el = el->NextSibling()) {
		if (el->Type() == TiXmlNode::TINYXML_ELEMENT) {
			if (strcmp(el->Value(), "local") == 0) onLocal((TiXmlElement*)el);
			else if (strcmp(el->Value(), "login") == 0) onLogin((TiXmlElement*)el);
			else if (strcmp(el->Value(), "share") == 0) onShare((TiXmlElement*)el);
			else if (strcmp(el->Value(), "event") == 0) onEvent((TiXmlElement*)el);
			else if (strcmp(el->Value(), "result") == 0) onResult((TiXmlElement*)el);
			else if (strcmp(el->Value(), "resultblock") == 0) onBlock((TiXmlElement*)el);
			else if (strcmp(el->Value(), "ilookup") == 0) onILookup((TiXmlElement*)el);
			else if (strcmp(el->Value(), "ivalue") == 0) onIValue((TiXmlElement*)el);
			else {
				std::cout << "XNet: Unknown Command\n";
			}
		}
	}
	
	/*int i = 0;
	XNet_Header *h;
	const char *od = d;

	DMsg err(DMsg::ERROR);
	while (i<length) {
		h = (XNet_Header*)d;
		switch (h->id) {
		case MSG_LOCAL:		i += onLocal(d); break;
		case MSG_LOGIN:		i += onLogin(d); break;
		case MSG_SHARE:		i += onShare(d); break;
		case MSG_EVENT:		i += onEvent(d); break;
		case MSG_SMALLEVT:	i += onEvent(d); break;
		case MSG_RESULT:	i += onResult(d); break;
		case MSG_BLOCK:		i += onBlock(d); break;
		case MSG_ILOOKUP:	i += onILookup(d); break;
		case MSG_IVALUE:	i += onIValue(d); break;
		default:	err << "XNetProtocol: I received a message that appears to be corrupt. " << length-i << "\n";
				printhex(d, 20);
				//i++;
				return;
		}
		
		d = od + i;
		
	}*/

}

void XNetProtocol::onILookup(TiXmlElement *x) {
	//std::cout << "ILOOKUP\n";
	/*XNet_ILookup *l = (XNet_ILookup*)buf;
	char buf2[50];
	l->index.toString(buf2, 50);
	//std::cout << "Lookup: " << buf2 << "\n";
	begin();
	ivalue(l->index.d(), buf2);
	end();
	return sizeof(XNet_ILookup);*/
	
	
}

void XNetProtocol::onIValue(TiXmlElement *x) {
	//std::cout << "IVALUE\n";
	//std::cout << "IVALUE\n";
	/*XNet_IValue *l = (XNet_IValue*)buf;
	m_indexes[l->id] = OID(l->value).d();
	return sizeof(XNet_IValue);*/
	
	
}

void XNetProtocol::onLocal(TiXmlElement *x) {
	//std::cout << "LOCAL\n";
	//m_conn->handler(OID(tag->content()));
	/*XNet_Local *l = (XNet_Local*)buf;
	m_conn->xhandler(l->local);
	return sizeof(XNet_Local);*/
	
	TiXmlNode *n = x->FirstChild();
	if (n->Type() == TiXmlNode::TINYXML_TEXT) {
		char buf[100];
		strcpy(buf, n->Value());
		if (buf[0] == '[') buf[0] = '<';
		m_conn->xhandler(buf);
	} else {
		std::cout << "XNet: Local tag incorrectly formed\n";
	}
	
}

void XNetProtocol::onShare(TiXmlElement *x) {
	//std::cout << "SHARE\n";
	//connection()->set("share", OID(tag->content()));
	/*XNet_Share *l = (XNet_Share*)buf;
	connection()->set("share", l->share);
	return sizeof(XNet_Share);*/
	
	TiXmlNode *n = x->FirstChild();
	if (n->Type() == TiXmlNode::TINYXML_TEXT) {
		char buf[100];
		strcpy(buf, n->Value());
		if (buf[0] == '[') buf[0] = '<';
		connection()->set("share", OID(buf));
	} else {
		std::cout << "XNet: Share tag incorrectly formed\n";
	}
}

void XNetProtocol::onEvent(TiXmlElement *x) {
	//std::cout << "EVENT\n";
	
	//Get ID attribute
	//Get type
	//Get destination
	//Get 4 parameters
	
	int id;
	Event *evt = new Event();
	
	TiXmlAttribute *attrib = x->FirstAttribute();
	if (strcmp(attrib->Name(), "id") == 0) {
		id = atoi(attrib->Value());
	} else {
		std::cout << "XNet: Event must have an id\n";
		return;
	}
	
	TiXmlNode *n = x->FirstChild();
	if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "type") == 0)) {
		evt->type(atoi(n->FirstChild()->Value()));
	}
	
	n = n->NextSibling();
	if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "destination") == 0)) {
		evt->dest(OID(n->FirstChild()->Value()));
	}
	
	n = n->NextSibling();
	if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "parameter") == 0)) {
		evt->param<0>(OID(n->FirstChild()->Value()));
	}
	
	n = n->NextSibling();
	if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "parameter") == 0)) {
		evt->param<1>(OID(n->FirstChild()->Value()));
	}
	
	n = n->NextSibling();
	if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "parameter") == 0)) {
		evt->param<2>(OID(n->FirstChild()->Value()));
	}
	
	n = n->NextSibling();
	if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "parameter") == 0)) {
		evt->param<3>(OID(n->FirstChild()->Value()));
	}
	
	//Now do index lookups
	//if (evt->param<0>().isName()) evt->param<0>(lookup(evt->param<0>()));
	//if (evt->param<1>().isName()) evt->param<1>(lookup(evt->param<1>()));
	//if (evt->param<2>().isName()) evt->param<2>(lookup(evt->param<2>()));
	//if (evt->param<3>().isName()) evt->param<3>(lookup(evt->param<3>()));

	//std::cout << "onEvent: " << id << "\n";
	
	if (id != 0xFF) {
		//Blocking
		evt->send();
		//std::cout << "Sending result: " << id << "\n";
		begin();
		result(id, evt->result());
		endResponse();
		delete evt;
	} else {
		evt->send(Event::FLAG_FREE);
	}
	
	/*XNet_Event *e = (XNet_Event*)buf;
	Event *evt = new Event();
	
	int id = e->id;
	evt->dest(e->dest);
	evt->type(e->type);

	//DMsg msg(DMsg::DEBUG);
	//msg << "Event: type=" << (int)e->type << " dest=" << e->dest << " p1=" << e->p1 << "\n";
	//std::cout << "Event type=" << (int)e->type << " id=" << (int)e->id << "\n";

	evt->param<0>(e->p1);
	evt->param<1>(e->p2);
	evt->param<2>(e->p3);
	evt->param<3>(e->p4);

	//Now do index lookups
	if (evt->param<0>().isName()) evt->param<0>(lookup(evt->param<0>()));
	if (evt->param<1>().isName()) evt->param<1>(lookup(evt->param<1>()));
	if (evt->param<2>().isName()) evt->param<2>(lookup(evt->param<2>()));
	if (evt->param<3>().isName()) evt->param<3>(lookup(evt->param<3>()));

	//std::cout << "onEvent: " << id << "\n";
	
	if (id != 0xFF) {
		//Blocking
		evt->send();
		//std::cout << "Sending result: " << id << "\n";
		begin();
		result(id, evt->result());
		end();
		delete evt;
	} else {
		evt->send(Event::FLAG_FREE);
	}
	
	return sizeof(XNet_Event);*/
}

extern OID xnetbase;

void XNetProtocol::onLogin(TiXmlElement *x) {
	//std::cout << "LOGIN\n";
	//extract username and password and compare with the database.
	/*XNet_Login *l = (XNet_Login*)buf;

	DString pwd((OID)(xnetbase["users"][l->username]["password"]));
	char buf2[100];
	pwd.toString(buf2,100);
	if (strcmp(buf2, l->password) == 0) {
		//Login was successful
		char *buf3 = new char[1000];
		strcpy(buf3, "Successful Login: ");
		strcat(buf3, l->username);
		Info(0, buf3);
		delete [] buf3;
		m_this = xnetbase["users"][l->username];
	} else {
		char *buf3 = new char[1000];
		strcpy(buf3, "Invalid Login: ");
		strcat(buf3, l->username);
		Warning(0, buf3);
	}
	
	begin();
	local(OID::local());
	share(m_this);
	end();
	
	return sizeof(XNet_Login);*/
	
	char *passwd = new char[200];
	char *user = new char[200];
	
	TiXmlNode *n = x->FirstChild();
	if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "username") == 0)) {
		strcpy(user,n->FirstChild()->Value());
	} else if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "password") == 0)) {
		strcpy(passwd,n->FirstChild()->Value());
	} else {
		std::cout << "Xnet: Unexpected tag in Login\n";
	}
	
	n = n->NextSibling();
	
	if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "username") == 0)) {
		strcpy(user,n->FirstChild()->Value());
	} else if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "password") == 0)) {
		strcpy(passwd,n->FirstChild()->Value());
	} else {
		std::cout << "Xnet: Unexpected tag in Login\n";
	}
	
	DString pwd((OID)(xnetbase["users"][user]["password"]));
	char buf2[100];
	pwd.toString(buf2,100);
	
	if (strcmp(buf2, passwd) == 0) {
		//Login was successful
		char *buf3 = new char[1000];
		strcpy(buf3, "Successful Login: ");
		strcat(buf3, user);
		Info(0, buf3);
		delete [] buf3;
		m_this = xnetbase["users"][user];
	} else {
		char *buf3 = new char[1000];
		strcpy(buf3, "Invalid Login: ");
		strcat(buf3, user);
		Warning(0, buf3);
		delete [] buf3;
	}
	
	delete [] passwd;
	delete [] user;
	
	begin();
	local(OID::local());
	share(m_this);
	endResponse();
}

void XNetProtocol::onResult(TiXmlElement *x) {
	//std::cout << "RESULT\n";
	/*XNet_Result *r = (XNet_Result*)buf;

	//std::cout << "Received result: " << r->id << "\n";

	m_results[r->id].res = r->result;
	m_results[r->id].waiting = false;
	if (m_results[r->id].res.isName()) m_results[r->id].res = lookup(m_results[r->id].res);
	
	return sizeof(XNet_Result);*/
	
	int rid;
	OID res;
	
	TiXmlAttribute *attrib = x->FirstAttribute();
	if (strcmp(attrib->Name(), "id") == 0) {
		rid = atoi(attrib->Value());
	} else {
		std::cout << "XNet: Result must have an id\n";
		return;
	}
	
	res = OID(x->FirstChild()->Value());
	
	m_results[rid].res = res;
	m_results[rid].waiting = false;
	//if (m_results[rid].res.isName()) m_results[rid].res = lookup(m_results[rid].res);
}

void XNetProtocol::onBlock(TiXmlElement *x) {
	//std::cout << "BLOCK\n";
	/*XNet_Block *b = (XNet_Block*)buf;

	//std::cout << "onblock\n";

	m_results[b->id].res = Buffer::create(B_OID, b->size);
	Buffer *buf2 = Buffer::lookup(m_results[b->id].res);

	//DMsg msg(DMsg::DEBUG);
	OID *element = (OID*)(b+1);

	//The network buffer is reused by lookup.
	int size = b->size;
	int id = b->id;

	for (int i=0; i<size; i++) {
		if (element[i].isName()) element[i] = lookup(element[i]);
		buf2->seti(i, element[i]);
		//msg << " element = " << element[i] << "\n";
	}

	m_results[id].waiting = false;
	
	return sizeof(XNet_Block) + (size * sizeof(OID));*/
	
	int rid;
	
	TiXmlAttribute *attrib = x->FirstAttribute();
	if (strcmp(attrib->Name(), "id") == 0) {
		rid = atoi(attrib->Value());
	} else {
		std::cout << "XNet: Result must have an id\n";
		return;
	}
	
	Buffer *buf2;
	int size;
	
	TiXmlNode *n = x->FirstChild();
	if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "count") == 0)) {
		size = atoi(n->FirstChild()->Value());
		m_results[rid].res = Buffer::create(B_OID, size);
		buf2 = Buffer::lookup(m_results[rid].res);
	}
	
	for (int i=0; i<size; i++) {
		n = n->NextSibling();
		if (n->Type() == TiXmlNode::TINYXML_ELEMENT && (strcmp(n->Value(), "element") == 0)) {
			buf2->seti(i, OID(n->FirstChild()->Value()));
		}
	}
	
	m_results[rid].waiting = false;
}

OID XNetProtocol::lookup(const OID &index) {
	//DMsg msg(DMsg::DEBUG);
	//msg << "Lookup: " << index << "\n";

	if (m_indexes[index.d()] != -1) {
		return OID(0,5,0,m_indexes[index.d()]);
	} else {
		begin();
		ilookup(index);
		end();
		while (m_indexes[index.d()] == -1) connection()->update();
		return OID(0,5,0,m_indexes[index.d()]);
	}
}
