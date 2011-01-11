#include <QtGui>
#include <QTextStream>
#include "inputwindow.h"
#include "syntax.h"
#include <iostream>
#include <cadence/cadence.h>
#include <cadence/doste/oid.h>
#include <cadence/dasm.h>
#include <cadence/messages.h>
#include "browser.h"
#include "msghandler.h"

using namespace cadence;
using namespace cadence::doste;

QString soutput;

IWindow::IWindow(MsgAgent *msg, HistoryLog *hist, const char *notation) {

	m_history = hist;
	m_msg = msg;
	QHBoxLayout *buttonLayout = new QHBoxLayout;

	m_notation = OID(notation);

	m_input = new QTextEdit();
	m_input->setTabStopWidth(40);
	char *buf = new char[1000];
	cadence::findFile(buf, "data/ui/accept.png");
	m_submit = new QPushButton(QIcon(buf), "Accept");
	delete [] buf;
	m_submit->setFixedSize(100,25);
	m_historyix = 0;

	//m_output = new QTextEdit();

	buttonLayout->addWidget(m_submit);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_input);
	mainLayout->addLayout(buttonLayout);
	//mainLayout->addWidget(m_output);
	setLayout(mainLayout);

	m_keylist = new QListWidget(this);
	m_keylist->move(x(),y()+300);
	m_keylist->resize(200,100);

	if (strcmp(notation, "eden") == 0) {
		EdenSyntax *syn = new EdenSyntax(m_input->document());
	} else {
		Syntax *syn = new Syntax(m_input->document(), m_keylist);
	}
	resize(640,480);
	show();


	connect(m_submit, SIGNAL(clicked()), this, SLOT(submit()));
	connect(m_input, SIGNAL(textChanged()), this, SLOT(textchanged()));
}

IWindow::~IWindow() {

}

void IWindow::textchanged() {
	if (m_keylist->count() > 0) {
		if (m_keylist->isVisible() == false) {
			//m_keylist->move();
			m_keylist->show();
		}
	} else {
		if (m_keylist->isVisible()) m_keylist->hide();
	}
}

void IWindow::submit() {
	OID dasm = cadence::doste::root["notations"][m_notation];

	if (dasm == Null) {
		cadence::Error(cadence::Error::CUSTOM, "Unrecognised Notation");
		return; 
	}

	//char *ibuf = m_input->toPlainText().toLocal8Bit().data();
	//std::cout << ibuf << "\n";

	char *pbuf = new char[100000];
	m_history->addEntry(m_input->toPlainText());
	m_historyix = m_history->count();
	strcpy(pbuf, (const char*)(m_input->toPlainText().toAscii().constData()));
	
	//soutput.append("<b>");
	//soutput.append(pbuf);
	//soutput.append("</b><br/>\n");
	
	//std::cout << pbuf << "\n";
	
	((Notation*)dasm)->execute(pbuf);
	//OID res = dasm.get("result");
	/*if (!res.isReserved() && res.get(Size) != Null && res.get(0).isChar()) {
		dstring(res).toString(pbuf,1000);
		m_msg->infoMessage(pbuf);
		//std::cout << "  \"" << pbuf << "\"\n";
	} else {
		res.toString(pbuf, 1000);
		//m_msg->infoMessage(pbuf);
		//std::cout << "  " << pbuf << "\n";
	}*/
	/*if (!res.isReserved() && res.get(Size) != Null && res.get(0).isChar()) {
		dstring(res).toString(pbuf,1000);
		//std::cout << "  \"" << pbuf << "\"\n";
		soutput.append("\"");
		soutput.append(pbuf);
		soutput.append("\"<br/>\n");
		
	} else {
		res.toString(pbuf, 1000);
		//std::cout << "  " << pbuf << "\n";
		soutput.append(pbuf);
		soutput.append("<br/>\n");
	}
	m_output->document()->setHtml(soutput);
	*/
	m_input->clear();

	delete [] pbuf;
}

void IWindow::editPrevious() {
	if (m_historyix > 0) {
		m_historyix--;
		m_input->setPlainText(m_history->entry(m_historyix));
	}
}

void IWindow::editNext() {
	if (m_historyix < ((int)m_history->count())-1) {
		m_historyix++;
		m_input->setPlainText(m_history->entry(m_historyix));
	} else {
		m_historyix = m_history->count();
		m_input->clear();
	}
}

void IWindow::editCut() {
	m_input->cut();
}

void IWindow::editCopy() {
	m_input->copy();
}

void IWindow::editPaste() {
	m_input->paste();
}

void IWindow::loadFile(QString &file)
{
	FILE *fh = fopen(file.toAscii().data(), "r");
	if (fh == 0)
		return;
	QTextStream *ts = new QTextStream(fh, QIODevice::ReadOnly);
	m_input->document()->setPlainText(ts->readAll());
	delete ts;
	fclose(fh);
}

void IWindow::saveFile(QString &file)
{
	FILE *fh = fopen(file.toAscii().data(), "w");
	if (fh == 0)
		return;
	QTextStream *ts = new QTextStream(fh, QIODevice::WriteOnly);
	*ts << m_input->toPlainText().toAscii().constData();
	delete ts;
	fclose(fh);
}
