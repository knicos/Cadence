#include "history.h"
#include <QVBoxLayout>
#include "inputwindow.h"
#include <QFileDialog>


HistoryLog::HistoryLog(QTabWidget *tabs) {
	m_output = new QListWidget();
	m_tabs = tabs;
	//m_output->setSelectionMode(QAbstractItemView::NoSelection);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_output);
	setLayout(mainLayout);
	
	m_color1 = QColor(242,242,242);
	m_color2 = QColor(255,255,255);
	m_color = &m_color1;
	
	connect(m_output, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(doubleclick(QListWidgetItem*)));
}

void HistoryLog::addEntry(const QString &str) {
	m_history.push_back(str);
	QListWidgetItem *item = new QListWidgetItem(str, m_output);
	item->setFont(QFont("Courier New"));
	item->setBackgroundColor(*m_color);
	if (m_color == &m_color2) m_color = &m_color1; else m_color = &m_color2;
}

void HistoryLog::savehist() {
	QString s = QFileDialog::getSaveFileName(this, "Save History", QString::null, "Scripts (*.dasm)");
	
	char *buf = new char[100000];
	buf[0] = 0;
	char *buf2 = buf;
	for (int i=0; i<m_history.size(); i++) {
		strcpy(buf2, (const char*)m_history[i].toAscii());
		strcat(buf2, "\n");
		buf2 = buf2 + strlen(buf2);
	}
	
	FILE *fh = fopen(s.toAscii().data(), "w");
	if (fh == 0)
		return;
	fwrite(buf, 1,strlen(buf), fh);
	fclose(fh);
	
	delete [] buf;
}

void HistoryLog::doubleclick(QListWidgetItem *item) {
	m_source->input()->setPlainText(item->text());
	m_tabs->setCurrentWidget(m_source);
}
