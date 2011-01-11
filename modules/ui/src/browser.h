#ifndef _BROWSER_H_
#define _BROWSER_H_

#include <cadence/doste/oid.h>
#include <cadence/agent.h>
#include <QWidget>
#include <list>
#include <QMenu>
#include <QTabWidget>
#include "history.h"
#include <QLineEdit>
#include <QTimer>

class Browser;

class BrowserItem : public cadence::BaseAgent {
	public:
	BrowserItem(Browser *host, const cadence::doste::OID &o, const char *label, const cadence::doste::OID &e, const cadence::doste::OID &p);
	~BrowserItem();
	
	void paint(QPainter &p, int px, int py);
	void mouseClick(int x, int y, Qt::MouseButton button);
	void toolTip(int x, int y, const QPoint &p);
	
	BrowserItem *findItem(int x, int y);
	
	void setObject(const cadence::doste::OID &o);
	
	int height() { return m_height; }
	int width() { return m_width; }
	int count() { return m_count; }
	
	bool edgeClick() { return m_edgeclick; }
	
	char *getEdgeLabel() { return m_label; }
	char *getValueLabel() { return m_vallabel; }
	const cadence::doste::OID &getEdge() { return m_edge; }
	const cadence::doste::OID &getValue() { return m_object; }
	const cadence::doste::OID &getParent() { return m_parent; }
	int getScale();
	bool changed() { return m_changed; }
	bool hasDefinition() { return m_def != 0; }
	const char *definition() { return m_def; }
	
	int screenX() { return m_screenx; }
	int screenY() { return m_screeny; }
	
	void expand();
	void expandDefinition();
	void collapse();
	
	BEGIN_EVENTS(BaseAgent);
	EVENT(evt_changed, (m_object)(cadence::doste::All));
	END_EVENTS;
	
	static const int SCALE_LARGE = 1;
	static const int SCALE_NORMAL = 2;
	static const int SCALE_SMALL = 3;
	static const int SCALE_TINY = 4;
	
	private:
	cadence::doste::OID m_object;
	cadence::doste::OID m_parent;
	cadence::doste::OID m_edge;
	char m_label[50];
	char m_vallabel[50];
	bool m_expanded;
	int m_scale;
	int m_screenx;
	int m_screeny;
	int m_type;
	int m_height;
	int m_width;
	int m_count;
	Browser *m_host;
	int m_aid;
	int m_eid;
	bool m_changed;
	bool m_defbrowse;
	char *m_def;
	cadence::doste::OID m_defobject;
	bool m_edgeclick;
	
	std::list<BrowserItem*> m_children;
	
	void drawNode(QPainter &p);
	void drawEdges(QPainter &p);
	int drawEdge(QPainter &p, int y, BrowserItem *item, bool last); 
};

namespace cadence {
	void findFile(char *buf, const char *filename);
};

class Browser : public QWidget, public cadence::BaseAgent
{
	Q_OBJECT

	public:
	Browser(HistoryLog *hist, QTabWidget *tabs, const cadence::doste::OID &base, QWidget *parent = 0); 
	~Browser();
	
	void addRoot(cadence::doste::OID r) {
		m_roots.push_back(new BrowserItem(this, r, "Object", cadence::doste::Null, cadence::doste::Null));
	};
	
	void changed() { m_changed = true; };
	void menuPopup(BrowserItem *item, bool edge);
	
	static QImage *image_line1;
	static QImage *image_line2;
	static QImage *image_line3;
	static QImage *image_line4;
	static QImage *image_line5;
	static QImage *image_line6;
	static QImage *image_line7;
	static QImage *image_line8;
	static QImage *image_node[50];
	
	bool useid() { return m_useid; }
	bool hideid() { return m_hideid; }
	bool hidekey() { return m_hidekey; }
	bool hideparent() { return m_hideparent; }
	
	BEGIN_EVENTS(cadence::BaseAgent);
	EVENT(evt_root, (m_base)("root"));
	EVENT(evt_useid, (m_base)("useid"));
	EVENT(evt_hideid, (m_base)("hideid"));
	EVENT(evt_hidekey, (m_base)("hidekey"));
	EVENT(evt_hideparent, (m_base)("hideparent"));
	EVENT(evt_refresh, (m_base)("refreshrate"));
	END_EVENTS;
	
	bool event(QEvent *event);
	
	public slots:
	void updateScreen();
	void showdefinition();
	void browsedefinition();
	void adddefinition();
	void finishEdit();
	void finishNewEdge();

	protected:
	void paintEvent(QPaintEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	
	private:
	std::list<BrowserItem*> m_roots;
	bool m_changed;
	cadence::doste::OID m_base;
	QMenu *m_menu;
	QPoint m_global;
	BrowserItem *m_currentitem;
	bool m_edgeclick;
	QTabWidget *m_tabs;
	HistoryLog *m_history;
	QPoint m_dragstart;
	QLineEdit *m_lineedit;
	QLineEdit *m_newedge;
	QTimer *m_timer;
	
	bool m_useid;
	bool m_hideid;
	bool m_hidekey;
	bool m_hideparent;
}; 

#endif
