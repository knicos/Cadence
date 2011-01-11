#ifndef _MSGHANDLER_
#define _MSGHANDLER_


#include <cadence/agent.h>
#include <cadence/doste/oid.h>
#include <QListWidget>
#include <QTabWidget>

class MsgAgent : public QWidget,  public cadence::Agent  {
	Q_OBJECT
	
	public:
	MsgAgent(QTabWidget *tabs, const cadence::doste::OID &obj);
	~MsgAgent() {};
	
	void infoMessage(const char *msg);
	
	BEGIN_EVENTS(cadence::Agent);
	EVENT(evt_error, (*this)("error"));
	EVENT(evt_warning, (*this)("warning"));
	EVENT(evt_debug, (*this)("debug"));
	EVENT(evt_info, (*this)("info"));
	END_EVENTS;
	
	private:
	QListWidget *m_output;
	QTabWidget *m_tabs;
	QColor m_color1;
	QColor m_color2;
	QColor *m_color;
}; 

#endif
