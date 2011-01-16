#include "cqwidget.h"

using namespace cadence;
using namespace cadence::doste;


OnEvent(CWidgetManager, evt_newwidget) {
	CQWidget *w;
	
	for (OID::iterator i = begin(); i != end(); i++) {
		if (((*i) != This) && ((*i) != Key)) {
			w = (CQWidget*)get(*i);
			if (m_tabs->indexOf(w) == -1) {
				dstring s = w->get("title");
				m_tabs->addTab(w, (const char*)s);
				w->show();
			}
		}
	}
}

IMPLEMENT_EVENTS(CWidgetManager, Agent);


CWidgetManager::CWidgetManager(const OID &o, QTabWidget *tabs)
 : Agent(o) {
	m_tabs = tabs;
	registerEvents(); 
}

CWidgetManager::~CWidgetManager() {
	
}
