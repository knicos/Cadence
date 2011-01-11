#include <cadence/dstring.h>
#include <cadence/processor.h>
#include <cadence/memory.h>
#include <string.h>

using namespace cadence;

DString::DString() : m_buffer(0) {
	m_obj = OID::create();
}

DString::DString(const char *str) : m_buffer(0) {
	m_obj = OID::create();
	int size = strlen(str);
	
	for (int i=0; i<size; i++) {
		m_obj[i].set(str[i]);
	}
	
	m_obj[Size].set(size);
	//Processor::processAll();
}

DString::DString(OID o) : m_buffer(0) {
	m_obj = o;
}

DString::~DString() {
	if (m_buffer != 0) delete [] m_buffer;
}

void DString::toString(char *str, int max) {

	//The whole object may be an integer etc...
	if (m_obj[Size] == Null || !((OID)m_obj[0]).isChar()) {
		m_obj.toString(str,max);
		return;
	}

	int size = (OID)m_obj[Size];

	if (size >= max) size = max-1;
	int j = 0;
	OID v;
	
	for (int i=0; i<size; i++) {
		v = m_obj[i];
		if (v.isChar())
			str[j++] = (OID)m_obj[i];
		else {
			int k = 0;
			char buf[500];
			v.toString(buf,500);
			while (buf[k] != 0) {
				str[j++] = buf[k++];
			}
		}
	}

	str[j] = 0;
};

DString::operator const char*() {
	if (m_buffer == 0) m_buffer = NEW char[size()+100];
	toString(m_buffer,size()+1);
	return m_buffer;
}
