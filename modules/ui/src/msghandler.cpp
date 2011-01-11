#include "msghandler.h"
//#include <QMessageBox>
#include <QVBoxLayout>
#include "browser.h"

using namespace cadence;
using namespace cadence::doste;

OnEvent(MsgAgent, evt_error) {
	if (get("error") == Null) return;

	char *msg = new char[2000];
	char buf[50];
	strcpy(msg, (const char*)dstring(get("error").get("message")));
	
	if (get("error").get("object") != Null) {
		strcat(msg, " - Object: ");
		get("error").get("object").toString(buf,50);
		strcat(msg,buf);
	}
	
	if (get("error").get("key") != Null) {
		strcat(msg, " Key: ");
		get("error").get("key").toString(buf,50);
		strcat(msg,buf);
	}

	//QMessageBox::warning(0, "Cadence Error", msg, QMessageBox::Ok, QMessageBox::NoButton);
	QListWidgetItem *item = new QListWidgetItem(msg, m_output);
	cadence::findFile(msg, "data/ui/error.png");
	item->setIcon(QIcon(msg));
	item->setBackgroundColor(*m_color);
	if (m_color == &m_color2) m_color = &m_color1; else m_color = &m_color2;
	m_output->setCurrentRow(m_output->count()-1);
	delete [] msg;
	
	m_tabs->setCurrentWidget(this);
}

OnEvent(MsgAgent, evt_warning) {
	if (get("warning") == Null) return;

	char *msg = new char[2000];
	char buf[50];
	strcpy(msg, (const char*)dstring(get("warning").get("message")));
	
	if (get("warning").get("object") != Null) {
		strcat(msg, "\nObject: ");
		get("warning").get("object").toString(buf,50);
		strcat(msg,buf);
	}
	
	if (get("warning").get("key") != Null) {
		strcat(msg, "\nKey: ");
		get("warning").get("key").toString(buf,50);
		strcat(msg,buf);
	}
	
	//QMessageBox::warning(0, "Cadence Warning", msg, QMessageBox::Ok, QMessageBox::NoButton);
	
	QListWidgetItem *item = new QListWidgetItem(msg, m_output);
	cadence::findFile(msg, "data/ui/warning.png");
	item->setIcon(QIcon(msg));
	item->setBackgroundColor(*m_color);
	if (m_color == &m_color2) m_color = &m_color1; else m_color = &m_color2;
	m_output->setCurrentRow(m_output->count()-1);
	delete [] msg;
	
	m_tabs->setCurrentWidget(this);
}

OnEvent(MsgAgent, evt_debug) {
	if (get("debug") == Null) return;

	//QMessageBox::information(0, "Cadence Debug", (const char*)dstring(get("debug").get("message")), QMessageBox::Ok, QMessageBox::NoButton);
}

OnEvent(MsgAgent, evt_info) {
	if (get("info") == Null) return;

	char *msg = new char[2000];
	char buf[50];
	strcpy(msg, (const char*)dstring(get("info").get("message")));
	
	QListWidgetItem *item = new QListWidgetItem(msg, m_output);
	cadence::findFile(msg, "data/ui/info.png");
	item->setIcon(QIcon(msg));
	item->setBackgroundColor(*m_color);
	if (m_color == &m_color2) m_color = &m_color1; else m_color = &m_color2;
	m_output->setCurrentRow(m_output->count()-1);
	delete [] msg;
	
	m_tabs->setCurrentWidget(this);
}

IMPLEMENT_EVENTS(MsgAgent, Agent);

void MsgAgent::infoMessage(const char *msg) {
	QListWidgetItem *item = new QListWidgetItem(msg, m_output);
	item->setBackgroundColor(*m_color);
	if (m_color == &m_color2) m_color = &m_color1; else m_color = &m_color2;
	//cadence::findFile("data/ui/info.png");
	//item->setIcon(QIcon(msg));
	m_output->setCurrentRow(m_output->count()-1);
	m_tabs->setCurrentWidget(this);
}

MsgAgent::MsgAgent(QTabWidget *tabs, const cadence::doste::OID &obj) : Agent(obj) {
	m_output = new QListWidget();
	m_output->setSelectionMode(QAbstractItemView::NoSelection);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_output);
	setLayout(mainLayout);
	m_tabs = tabs;
	
	m_color1 = QColor(242,242,242);
	m_color2 = QColor(255,255,255);
	m_color = &m_color1;
	
	registerEvents();
}
