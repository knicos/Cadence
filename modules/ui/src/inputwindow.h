#ifndef _IWINDOW_H_
#define _IWINDOW_H_

#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <cadence/doste/oid.h>
#include "history.h"

class MsgAgent;

class IWindow : public QWidget {
	Q_OBJECT

	public:
	IWindow(MsgAgent *msg, HistoryLog *hist, const char *notation);
	~IWindow();
	
	void loadFile(QString &file);
	void saveFile(QString &file);
	QTextEdit *input() { return m_input; }

	private:

	QTextEdit *m_input;
	QPushButton *m_submit;
	QListWidget *m_keylist;
	QTextEdit *m_output;
	cadence::doste::OID m_notation;
	HistoryLog *m_history;
	MsgAgent *m_msg;
	int m_historyix;

	public slots:
	void submit();
	void editPrevious();
	void editNext();
	void editCut();
	void editCopy();
	void editPaste();
	void textchanged();
};

#endif
