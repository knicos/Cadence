#include "modules.h"
#include <QVBoxLayout>

using namespace cadence;
using namespace cadence::doste;

OnEvent(ModuleList, evt_mod) {
	//Remove all existing rows
	for (int i=m_modules->rowCount()-1; i>=0; i--) {
		m_modules->removeRow(i);
	}
	//Add module to listing.
	int r=0;
	int c=0;
	QTableWidgetItem *item;
	char buf[50];
	for (cadence::doste::OID::iterator i = m_object.begin(); i != m_object.end(); i++) {
		if ((*i) == This || (*i) == Key) continue;
		m_modules->setRowCount(r+1);
		(*i).toString(buf,50);
		item = new QTableWidgetItem(buf);
		item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);
		m_modules->setItem(r,c++,item);
		
		r++;
		c = 0;
	}
}

IMPLEMENT_EVENTS(ModuleList, BaseAgent);

ModuleList::ModuleList() {
	m_modules = new QTableWidget(1,1);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_modules);
	setLayout(mainLayout);
	
	m_object = cadence::doste::root["modules"];
	
	registerEvents();
}

