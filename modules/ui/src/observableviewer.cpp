#include "observableviewer.h"
#include "browser.h"
#include <QHBoxLayout>
#include "syntax.h"
#include <cadence/cadence.h>

using namespace cadence;
using namespace cadence::doste;

ObservableViewer::ObservableViewer(HistoryLog *hist, const cadence::doste::OID &object, const cadence::doste::OID &key) {
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	m_history = hist;
	m_def = new QTextEdit();
	m_def->setTabStopWidth(40);
	m_object = object;
	m_key = key;
	
	m_obsname = new QLineEdit();
	
	m_deftype = new QComboBox();
	m_deftype->setMinimumWidth(50);
	m_deftype->addItem("=");
	m_deftype->addItem("is");
	m_deftype->addItem(":=");
	m_valstring = new QLineEdit();
	
	char *buf = new char[2000];
	int flags;
	OID def = m_object.definition(m_key);
	if (def != Null) {
		flags = m_object.flags(m_key);
		if (flags & OID::FLAG_FUNCTION) {
			//is
			m_valstring->setEnabled(false);
			m_deftype->setCurrentIndex(1);
		} else {
			//:=
			m_valstring->setEnabled(true);
			m_deftype->setCurrentIndex(2);
		}
		buf[0] = 0;
		Definition(def).toString(buf,2000);
		//std::cout << buf << "}\n";
		m_def->setPlainText(buf);
	} else {
		m_def->setEnabled(false);
		m_deftype->setCurrentIndex(0);
	}
	//delete [] buf;
	
	m_object.get(m_key).toString(buf,2000);
	m_valstring->setText(buf);
	
	m_key.toString(buf,2000);
	m_obsname->setText(buf);
	m_obsname->setEnabled(false);
	
	//char *buf = new char[1000];
	cadence::findFile(buf, "data/ui/close.png");
	QPushButton *closebut = new QPushButton(QIcon(buf),"");
	
	cadence::findFile(buf, "data/ui/save.png");
	m_submit = new QPushButton(QIcon(buf), "");
	delete [] buf;

	buttonLayout->addWidget(m_submit);
	buttonLayout->addWidget(m_obsname);
	buttonLayout->addWidget(m_deftype);
	buttonLayout->addWidget(m_valstring);
	buttonLayout->addWidget(closebut);
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(buttonLayout);
	mainLayout->addWidget(m_def);
	//mainLayout->addWidget(m_keylist);
	//mainLayout->addWidget(m_output);
	setLayout(mainLayout);
	
	m_keylist = new QListWidget(this);
	m_keylist->move(x(),y());
	m_keylist->resize(200,100);
	
	Syntax *syn = new Syntax(m_def->document(), m_keylist);
	//m_keylist->show();
	//show();


	connect(m_submit, SIGNAL(clicked()), this, SLOT(submit()));
	connect(closebut, SIGNAL(clicked()), this, SLOT(closeme()));
	connect(m_deftype, SIGNAL(currentIndexChanged(int)), this, SLOT(typechange(int)));
	connect(m_def, SIGNAL(textChanged()), this, SLOT(textchanged()));
}

ObservableViewer::ObservableViewer(HistoryLog *hist, const cadence::doste::OID &object) {
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	m_history = hist;
	m_def = new QTextEdit();
	m_def->setTabStopWidth(40);
	m_object = object;
	m_keylist = new QListWidget();
	
	m_obsname = new QLineEdit();
	
	Syntax *syn = new Syntax(m_def->document(), m_keylist);
	
	m_deftype = new QComboBox();
	m_deftype->setMinimumWidth(50);
	m_deftype->addItem("=");
	m_deftype->addItem("is");
	m_deftype->addItem(":=");
	m_valstring = new QLineEdit();
	
	char *buf = new char[2000];
	
	//char *buf = new char[1000];
	cadence::findFile(buf, "data/ui/close.png");
	QPushButton *closebut = new QPushButton(QIcon(buf),"");
	
	cadence::findFile(buf, "data/ui/save.png");
	m_submit = new QPushButton(QIcon(buf), "");
	delete [] buf;

	buttonLayout->addWidget(m_submit);
	buttonLayout->addWidget(m_obsname);
	buttonLayout->addWidget(m_deftype);
	buttonLayout->addWidget(m_valstring);
	buttonLayout->addWidget(closebut);
	
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(buttonLayout);
	mainLayout->addWidget(m_def);
	mainLayout->addWidget(m_keylist);
	//mainLayout->addWidget(m_output);
	setLayout(mainLayout);
	//show();


	connect(m_submit, SIGNAL(clicked()), this, SLOT(submit()));
	connect(closebut, SIGNAL(clicked()), this, SLOT(closeme()));
	connect(m_deftype, SIGNAL(currentIndexChanged(int)), this, SLOT(typechange(int)));
}

void ObservableViewer::textchanged() {
	if (m_keylist->count() > 0) {
		if (m_keylist->isVisible() == false) {
			//m_keylist->move();
			m_keylist->show();
		}
	} else {
		if (m_keylist->isVisible()) m_keylist->hide();
	}
}

void ObservableViewer::submit() {
	int index = m_deftype->currentIndex();
	char *buf = new char[10000];
	m_object.toString(buf,1000);
	strcat(buf, " ");
	
	if (m_obsname->isEnabled()) {
		m_key = OID((const char*)m_obsname->text().toAscii());
		strcat(buf, m_obsname->text().toAscii());
		m_obsname->setEnabled(false);
	} else {
		m_key.toString(buf+strlen(buf),1000);
	}
	
	if (index == 0) {
		strcat(buf, " = ");
		strcat(buf, (const char*)m_valstring->text().toAscii());
		strcat(buf, ";");
		
		//std::cout << buf;
	} else if (index == 1) {
		strcat(buf, " is { ");
		strcat(buf, (const char*)m_def->toPlainText().toAscii());
		strcat(buf, " }");
		
		//std::cout << buf;
	} else {
		strcat(buf, " := { ");
		strcat(buf, (const char*)m_def->toPlainText().toAscii());
		strcat(buf, " }\n");
		m_key.toString(buf+strlen(buf),1000);
		strcat(buf, " = ");
		strcat(buf, (const char*)m_valstring->text().toAscii());
		strcat(buf, ";");
		
		//std::cout << buf;
	}
	
	//std::cout << "CODE: " << buf << "\n";
	
	OID dasm = cadence::doste::root["notations"]["dasm"];
	((Notation*)dasm)->execute(buf);
	//m_history->addEntry(buf);
}

void ObservableViewer::closeme() {
	delete this;
}

void ObservableViewer::typechange(int index) {
	if (index == 0) {
		m_def->setEnabled(false);
		m_valstring->setEnabled(true);
	} else if (index == 1) {
		m_def->setEnabled(true);
		m_valstring->setEnabled(false);
	} else {
		m_def->setEnabled(true);
		m_valstring->setEnabled(true);
	}
}
