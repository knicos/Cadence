#ifndef _CQWIDGET_H_
#define _CQWIDGET_H_

#include <cadence/doste/oid.h>
#include <cadence/agent.h>
#include <QWidget>
#include <QTabWidget>

class CQWidget : public cadence::Agent, public QWidget {
	public:
	OBJECT(Agent, CQWidget);
	
	CQWidget(const cadence::doste::OID &o) : Agent(o) { registerEvents(); }
	~CQWidget() {}
};

class CWidgetManager : public cadence::Agent {
	public:
	CWidgetManager(const cadence::doste::OID &o, QTabWidget *tabs);
	~CWidgetManager();
	
	BEGIN_EVENTS(Agent);
	EVENT(evt_newwidget, (*this)(cadence::doste::Key));
	END_EVENTS;
	
	private:
	QTabWidget *m_tabs;
};

#endif
