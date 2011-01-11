#ifndef _HISTORYLOG_
#define _HISTORYLOG_

#include <QListWidget>

class IWindow;

class HistoryLog : public QWidget  {
	Q_OBJECT
	
	public:
	HistoryLog(QTabWidget *tabs);
	~HistoryLog() {};
	
	void setSource(IWindow *source) { m_source = source; }
	void addEntry(const QString &str);
	int count() { return (int)m_history.size(); }
	const QString &entry(int ix) { return m_history[ix]; }
	
	private:
	QListWidget *m_output;
	std::vector<QString> m_history;
	IWindow *m_source;
	QTabWidget *m_tabs;
	QColor m_color1;
	QColor m_color2;
	QColor *m_color;
	
	public slots:
	void doubleclick(QListWidgetItem *item);
	void savehist();
}; 

#endif
