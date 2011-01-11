#ifndef _MODULE_VIEW_
#define _MODULE_VIEW_

#include <cadence/doste/doste.h>
#include <cadence/agent.h>
#include <QWidget>
#include <QTableWidget>

class ModuleList : public QWidget, public cadence::BaseAgent {
	Q_OBJECT
	
	public:
	ModuleList();
	~ModuleList() {}
	
	BEGIN_EVENTS(cadence::BaseAgent);
	EVENT(evt_mod, m_object(cadence::doste::All));
	END_EVENTS;
	
	private:
	QTableWidget *m_modules;
	cadence::doste::OID m_object;
};

#endif
