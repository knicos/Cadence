#include "browser.h"
#include <QPainter>
#include <QMouseEvent>
#include <cadence/file.h>
#include <string.h>
#include <sys/stat.h>
#include <cadence/agenthandler.h>
#include <QToolTip>
#include "observableviewer.h"
#include <QApplication>
#include <QMimeData>

QImage *Browser::image_line1=0;
QImage *Browser::image_line2=0;
QImage *Browser::image_line3=0;
QImage *Browser::image_line4=0;
QImage *Browser::image_line5=0;
QImage *Browser::image_line6=0;
QImage *Browser::image_line7=0;
QImage *Browser::image_line8=0;
QImage *Browser::image_node[50]= { 0 };

using namespace cadence;
using namespace cadence::doste;

OnEvent(BrowserItem, evt_changed) {
	//std::cout << "Browser: Changed!\n";
	m_aid = aid;
	m_eid = eid;
	if (m_expanded) {
		char buf[50];
		std::list<BrowserItem*>::iterator j = m_children.begin();
		for (cadence::doste::OID::iterator i = m_object.begin(); i != m_object.end(); i++) {
			if (m_host->hideid() && (*i) == OID("id")) continue;
			if (m_host->hidekey() && (*i) == Key) continue;
			if (m_host->hideparent() && (*i) == This) continue;
			if (j != m_children.end()) {
				(*j)->setObject(m_object.get(*i));
				/*if (((OID)(**j)) != m_object.get(*i)) {
					delete *j;
					j = m_children.erase(j);
					(*i).toString(buf,50);
					j = m_children.insert(j, new BrowserItem(m_host, m_object.get(*i), buf));
				}*/
				j++;
			} else {
				(*i).toString(buf,50);
				m_children.push_back(new BrowserItem(m_host, m_object.get(*i), buf, *i, m_object));
			}
		}
		
		m_host->changed();
	}
}

IMPLEMENT_EVENTS(BrowserItem, BaseAgent);

BrowserItem::BrowserItem(Browser *host, const cadence::doste::OID &o, const char *label, const cadence::doste::OID &e, const cadence::doste::OID &p) {
	m_host = host;
	m_object = o;
	m_parent = p;
	m_edge = e;
	m_changed = false;
	m_defbrowse = false;
	if (m_object.isReserved() == false && m_object.get(0).isChar() && m_object.get("size") != Null) {
		dstring str(m_object);
		str.toString(m_vallabel, 50);
	} else {
		m_object.toString(m_vallabel,50);
	}
	strcpy(m_label, label);
	m_expanded = false;
	
	m_height = 16;
	m_width = 1000;
	m_aid = -1;
	m_eid = 0;
	
	m_defobject = p.definition(e);
	Definition def = m_defobject;
	if (def != Null) {
		m_def = new char[2000];	//HACK
		m_def[0] = 0;
		def.toString(m_def,2000);
	} else {
		m_def = 0;
	}
	
	if (m_object.isBool() || !m_object.isReserved())
		registerEvents();
}

BrowserItem::~BrowserItem() {
	if (m_def) delete [] m_def;
}

void BrowserItem::setObject(const cadence::doste::OID &o) {
	if (m_object == o) {
		return;
	} else {
		if (!m_defbrowse)
			collapse();
		m_object = o;
		if (m_object.isReserved() == false && m_object.get(0).isChar() && m_object.get("size") != Null) {
			dstring str(m_object);
			str.toString(m_vallabel, 50);
		} else {
			m_object.toString(m_vallabel,50);
		}
		//Redo the event handling
		if (m_aid != -1) {
			if (m_object.isBool() || !m_object.isReserved())
				cadence::AgentHandler::update(m_aid, this, m_eid, m_object(cadence::doste::All), 0);
		} else {
			if (m_object.isBool() || !m_object.isReserved())
				registerEvents();
		}
		
		m_changed = true;
	}
}

void BrowserItem::expandDefinition() {
	char buf[50];
	
	if (!m_expanded && (m_defobject != Null)) {
		m_defbrowse = true;
		m_expanded = true;
		for (cadence::doste::OID::iterator i = m_defobject.begin(); i != m_defobject.end(); i++) {
			if (m_host->hideid() && (*i) == OID("id")) continue;
			if (m_host->hidekey() && (*i) == Key) continue;
			if (m_host->hideparent() && (*i) == This) continue;
			(*i).toString(buf,50);
			m_children.push_back(new BrowserItem(m_host, m_defobject.get(*i), buf, *i, m_defobject));
		}
		m_host->repaint();
	}
}

void BrowserItem::expand() {
	char buf[50];
	
	if (!m_expanded) {
		m_expanded = true;
		for (cadence::doste::OID::iterator i = m_object.begin(); i != m_object.end(); i++) {
			if (m_host->hideid() && (*i) == OID("id")) continue;
			if (m_host->hidekey() && (*i) == Key) continue;
			if (m_host->hideparent() && (*i) == This) continue;
			(*i).toString(buf,50);
			m_children.push_back(new BrowserItem(m_host, m_object.get(*i), buf, *i, m_object));
		}
		m_host->repaint();
	}
}

void BrowserItem::collapse() {
	if (m_expanded) {
		m_defbrowse = false;
		m_expanded = false;
		for (std::list<BrowserItem*>::iterator i = m_children.begin(); i != m_children.end(); i++) {
			delete (*i);
		}
		m_children.clear();
		m_host->repaint();
	}
}

void BrowserItem::mouseClick(int x, int y, Qt::MouseButton button) {
	int cury = 17;
	int curx = 100;
	
	if (x > 0 && x < 17 && y < 17) {
		if (button == Qt::LeftButton) {
			if (m_expanded) collapse();
			else expand();
		} else {
			m_host->menuPopup(this, false);
		}
	} else if (x > -100 && x < 1 && y < 17) {
		if (button == Qt::RightButton) {
			m_host->menuPopup(this, true);
		}
	} else if (y >= 17) {
		//find correct child.
		for (std::list<BrowserItem*>::iterator i = m_children.begin(); i != m_children.end(); i++) {
			//cury += 2;
			if (y <= cury+ (*i)->count()*17) {
				(*i)->mouseClick(x-curx, y-cury, button);
				break;
			} else {
				cury += (*i)->count()*17;
			}
		}
	}
}

void BrowserItem::toolTip(int x, int y, const QPoint &p) {
	int cury = 17;
	int curx = 100;
	
	if (x > -100 && x < 100 && y < 17) {
		//Show tool tip for this item.
		if (m_def) QToolTip::showText(p, m_def);
		else QToolTip::showText(p, "");
	} else if (y >= 17) {
		//QToolTip::showText(p, "");
		
		//find correct child.
		for (std::list<BrowserItem*>::iterator i = m_children.begin(); i != m_children.end(); i++) {
			//cury += 2;
			if (y <= cury+ (*i)->count()*17) {
				(*i)->toolTip(x-curx, y-cury, p);
				break;
			} else {
				cury += (*i)->count()*17;
			}
		}
	}
}

BrowserItem *BrowserItem::findItem(int x, int y) {
	int cury = 17;
	int curx = 100;
	
	if (x > -100 && x < 100 && y < 17) {
		if (x < 0) m_edgeclick = true;
		else m_edgeclick = false;
		return this;
	} else if (y >= 17) {
		//QToolTip::showText(p, "");
		
		//find correct child.
		for (std::list<BrowserItem*>::iterator i = m_children.begin(); i != m_children.end(); i++) {
			//cury += 2;
			if (y <= cury+ (*i)->count()*17) {
				return (*i)->findItem(x-curx, y-cury);
			} else {
				cury += (*i)->count()*17;
			}
		}
	}
	
	return 0;
}

int BrowserItem::getScale() {
	//Calculate scale based on the last time it was pinged
	return SCALE_NORMAL;
}

void BrowserItem::drawNode(QPainter &p) {
	//0 = default node type.
	if (m_object == cadence::doste::Null) {
		p.drawImage(m_screenx, m_screeny, *Browser::image_node[1]);
	} else if (m_object.isModifier()) {
		
		p.setPen(Qt::NoPen);
		if (m_changed)
			p.setBrush(QBrush(QColor(250,240,240)));
		else
			p.setBrush(QBrush(QColor(240,240,240)));
		p.drawEllipse(m_screenx+100,m_screeny,16,16);
		p.drawRect(m_screenx+8,m_screeny,100,16);
		
		p.drawImage(m_screenx, m_screeny, *Browser::image_node[4]);
		p.setPen(Qt::SolidLine);
		p.drawText(m_screenx + 20, m_screeny+1, 85, 16, Qt::AlignLeft, m_vallabel);
	} else if (m_object.isReserved()) {
		
		p.setPen(Qt::NoPen);
		if (m_changed)
			p.setBrush(QBrush(QColor(250,240,240)));
		else
			p.setBrush(QBrush(QColor(240,240,240)));
		p.drawEllipse(m_screenx+100,m_screeny,16,16);
		p.drawRect(m_screenx+8,m_screeny,100,16);
		
		p.drawImage(m_screenx, m_screeny, *Browser::image_node[2]);
		p.setPen(Qt::SolidLine);
		p.drawText(m_screenx + 20, m_screeny+1, 85, 16, Qt::AlignLeft, m_vallabel);
	} else if (m_object.get(0).isChar() && m_object.get("size") != Null) {
		
		p.setPen(Qt::NoPen);
		p.setBrush(QBrush(QColor(240,240,240)));
		p.drawEllipse(m_screenx+100,m_screeny,16,16);
		p.drawRect(m_screenx+8,m_screeny,100,16);
		
		if (m_expanded)
			p.drawImage(m_screenx, m_screeny, *Browser::image_node[3]);
		else
			p.drawImage(m_screenx, m_screeny, *Browser::image_node[0]);
		p.setPen(Qt::SolidLine);
		p.setPen(QColor("red"));
		p.drawText(m_screenx + 20, m_screeny+1, 85, 16, Qt::AlignLeft, m_vallabel);
		p.setPen(QColor("black"));
	} else if (m_host->useid() && (m_object.get("id") != Null)) {
		char buf[50];
		m_object.get("id").toString(buf,50);
		
		p.setPen(Qt::NoPen);
		p.setBrush(QBrush(QColor(240,240,250)));
		p.drawEllipse(m_screenx+100,m_screeny,16,16);
		p.drawRect(m_screenx+8,m_screeny,100,16);
		
		if (m_expanded)
			p.drawImage(m_screenx, m_screeny, *Browser::image_node[3]);
		else
			p.drawImage(m_screenx, m_screeny, *Browser::image_node[0]);
		
		p.setPen(Qt::SolidLine);
		p.drawText(m_screenx + 20, m_screeny+1, 85, 16, Qt::AlignLeft, buf);
	} else {
		char buf[50];
		if (m_host->useid() == false) {
			m_object.toString(buf,50);
			
			p.setPen(Qt::NoPen);
			p.setBrush(QBrush(QColor(240,240,250)));
			p.drawEllipse(m_screenx+100,m_screeny,16,16);
			p.drawRect(m_screenx+8,m_screeny,100,16);
		}
		
		if (m_expanded)
			p.drawImage(m_screenx, m_screeny, *Browser::image_node[3]);
		else
			p.drawImage(m_screenx, m_screeny, *Browser::image_node[0]);
		
		if (m_host->useid() == false) {
			p.setPen(Qt::SolidLine);
			p.drawText(m_screenx + 20, m_screeny+1, 85, 16, Qt::AlignLeft, buf);
		}
	}
}

void BrowserItem::drawEdges(QPainter &p) {
	int curx = m_screenx + 8;
	int cury = m_screeny + 16;
	int countres;
	int j=0;
	int x = m_screenx;
	if (m_defbrowse) x -= 70;
	
	m_count = 1;
	
	if (m_expanded) {
		for (std::list<BrowserItem*>::iterator i = m_children.begin(); i != m_children.end(); i++) {
			//cury += 2;
			//(*i)->paint(p, px+curx, py+cury);
			//NOTE: 16 is the height of a single entry.
			countres = drawEdge(p, cury, (*i), (j == m_children.size()-1));
			cury += 17;
			
			for (int k=1; k<countres; k++) {
				if (j != m_children.size()-1) {
					if (m_defbrowse)
						p.drawImage(x, cury, *Browser::image_line6);
					else
						p.drawImage(x, cury, *Browser::image_line2);
				}
				cury += 17;
			}
			j++;
			m_count += countres;
		}
	}
	
	m_height = cury - m_screeny;
}

int BrowserItem::drawEdge(QPainter &p, int y, BrowserItem *item, bool last) {
	int x = m_screenx;
	if (m_defbrowse) x -= 70;
	
	if (m_defbrowse) {
		if (!last) {
			p.drawImage(x, y, *Browser::image_line5);
		} else {
			p.drawImage(x, y, *Browser::image_line7);
		}
	} else {
		if (!last) {
			p.drawImage(x, y, *Browser::image_line1);
		} else {
			p.drawImage(x, y, *Browser::image_line3);
		}
	}
	
	if (item->hasDefinition()) {
		p.setPen(QColor("blue"));
	}
	
	QRect brect;
	
	if (item->changed()) {
		QFont nf(p.font());
		nf.setBold(true);
		p.setFont(nf);
		p.drawText(x + 20, y+1, 80, 16, Qt::AlignLeft, item->getEdgeLabel(), &brect);
		nf.setBold(false);
		p.setFont(nf);
	} else {
		p.drawText(x + 20, y+1, 80, 16, Qt::AlignLeft, item->getEdgeLabel(), &brect);
	}
	
	if (item->hasDefinition()) {
		p.setPen(QColor("black"));
	}
	
	p.drawImage(x+90, y, *Browser::image_line4);
	
	//Calculate how many lines to draw.
	if (brect.width() < 80) {
		int excess = (80 - brect.width()) / 10;
		for (int i=0; i<excess; i++) {
			if (m_defbrowse)
				p.drawImage(x+90 - (i * 10), y, *Browser::image_line8);
			else
				p.drawImage(x+90 - (i * 10), y, *Browser::image_line4);
		}
	}
	
	if (m_defbrowse) {
		p.drawImage(x+100, y, *Browser::image_line8);
		p.drawImage(x+110, y, *Browser::image_line8);
		p.drawImage(x+120, y, *Browser::image_line8);
		p.drawImage(x+130, y, *Browser::image_line8);
		p.drawImage(x+140, y, *Browser::image_line8);
		p.drawImage(x+150, y, *Browser::image_line8);
		p.drawImage(x+160, y, *Browser::image_line8);
	}
	item->paint(p, m_screenx + 100, y);
	return item->count();
}

void BrowserItem::paint(QPainter &p, int px, int py) {
	//Second part
	/*p.setPen(Qt::NoPen);
	p.setBrush(QBrush(QColor(200,200,240)));
	p.drawEllipse(px+200,py,16,16);
	p.drawRect(px+100,py,108,16);
	p.setPen(Qt::SolidLine);
	p.drawText(px+122,py+12, m_vallabel);
	
	p.setPen(Qt::NoPen);
	p.setBrush(QBrush(QColor(230,230,230)));
	p.drawEllipse(px+100,py,16,16);
	p.drawRect(px+8,py,100,16);
	p.setPen(Qt::SolidLine);
	p.drawText(px+20,py+12, m_label);
	p.setPen(QPen(QBrush(QColor(0,0,50)), 2));
	p.setBrush(QBrush(QColor(250,250,250)));
	p.drawEllipse(px,py,16,16);
	
	int curx = 10;
	int cury = 16;
	if (m_expanded) {
		for (std::list<BrowserItem*>::iterator i = m_children.begin(); i != m_children.end(); i++) {
			cury += 2;
			(*i)->paint(p, px+curx, py+cury);
			cury += (*i)->height();
		}
	}
	
	m_height = cury;*/
	m_screenx = px;
	m_screeny = py;
	drawNode(p);
	drawEdges(p);
}

OnEvent(Browser, evt_root) {
	if (m_roots.size() >= 1) {
		m_roots.front()->setObject(m_base.get("root"));
		changed();
	}
}

OnEvent(Browser, evt_useid) {
	m_useid = m_base.get("useid");
}

OnEvent(Browser, evt_hideid) {
	m_hideid = m_base.get("hideid");
}

OnEvent(Browser, evt_hidekey) {
	m_hidekey = m_base.get("hidekey");
}

OnEvent(Browser, evt_hideparent) {
	m_hideparent = m_base.get("hideparent");
}

OnEvent(Browser, evt_refresh) {
	m_timer->setInterval(m_base.get("refreshrate"));
}

IMPLEMENT_EVENTS(Browser, BaseAgent);

Browser::Browser(HistoryLog *hist, QTabWidget *tabs, const cadence::doste::OID &base, QWidget *parent)
	: QWidget(parent), m_base(base) {
	m_history = hist;
	setPalette(QPalette(QColor(255,255,255)));
	setAutoFillBackground(true);
	resize(1000,1000);
	m_tabs = tabs;
	
	char *buf = new char[1000];
	findFile(buf, "data/ui/node_0.png");
	image_node[0] = new QImage(buf);
	findFile(buf, "data/ui/node_1.png");
	image_node[1] = new QImage(buf);
	findFile(buf, "data/ui/node_2.png");
	image_node[2] = new QImage(buf);
	findFile(buf, "data/ui/node_0_b.png");
	image_node[3] = new QImage(buf);
	findFile(buf, "data/ui/node_4.png");
	image_node[4] = new QImage(buf);
	findFile(buf, "data/ui/edge_1.png");
	image_line1 = new QImage(buf);
	findFile(buf, "data/ui/edge_2.png");
	image_line2 = new QImage(buf);
	findFile(buf, "data/ui/edge_3.png");
	image_line3 = new QImage(buf);
	findFile(buf, "data/ui/edge_4.png");
	image_line4 = new QImage(buf);
	findFile(buf, "data/ui/edge_5.png");
	image_line5 = new QImage(buf);
	findFile(buf, "data/ui/edge_6.png");
	image_line6 = new QImage(buf);
	findFile(buf, "data/ui/edge_7.png");
	image_line7 = new QImage(buf);
	findFile(buf, "data/ui/edge_8.png");
	image_line8 = new QImage(buf);
	delete [] buf;
	
	m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(updateScreen()));
	m_timer->start(20);
	
	//Build popup menu.
	m_menu = new QMenu(this);
	m_menu->addAction(QIcon(QPixmap("icons/add.png")), "&Add Edge", this, SLOT(adddefinition()));
	m_menu->addAction(QIcon(QPixmap("icons/definition.png")), "&Edit Definition", this, SLOT(showdefinition()));
	m_menu->addAction(QIcon(QPixmap("icons/definition.png")), "&Browse Definition", this, SLOT(browsedefinition()));
	
	m_lineedit = new QLineEdit(this);
	m_lineedit->hide();
	m_lineedit->resize(120, m_lineedit->height());
	connect(m_lineedit, SIGNAL(editingFinished()), this, SLOT(finishEdit()));
	
	m_newedge = new QLineEdit(this);
	m_newedge->hide();
	m_newedge->resize(120, m_newedge->height());
	connect(m_newedge, SIGNAL(editingFinished()), this, SLOT(finishNewEdge()));
	
	registerEvents();
}

Browser::~Browser() {
	
}

bool Browser::event(QEvent *event) {
	if (event->type() == QEvent::ToolTip) {
            QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
	    m_roots.front()->toolTip(helpEvent->x()-10, helpEvent->y()-10, helpEvent->globalPos());
	    return true;
            /*int index = itemAt(helpEvent->pos());
            if (index != -1)
                QToolTip::showText(helpEvent->globalPos(), shapeItems[index].toolTip());
            else
                QToolTip::showText(helpEvent->globalPos(), "");*/
        }
        return QWidget::event(event);

}

void Browser::showdefinition() {
	ObservableViewer *obs = new ObservableViewer(m_history, m_currentitem->getParent(), m_currentitem->getEdge());
	char *buf = new char[1000];
	cadence::findFile(buf, "data/ui/object.png");
	m_tabs->addTab(obs, QIcon(QPixmap(buf)),m_currentitem->getEdgeLabel());
	m_tabs->setCurrentWidget(obs);
	delete [] buf;
}

void Browser::browsedefinition() {
	m_currentitem->expandDefinition();
}

void Browser::adddefinition() {
	/*ObservableViewer *obs;
	if (m_edgeclick)
		obs = new ObservableViewer(m_history, m_currentitem->getParent());
	else
		obs = new ObservableViewer(m_history, m_currentitem->getValue());
	char *buf = new char[1000];
	cadence::findFile(buf, "data/ui/object.png");
	m_tabs->addTab(obs, QIcon(QPixmap(buf)),"New");
	m_tabs->setCurrentWidget(obs);
	delete [] buf;*/
	
	m_newedge->move(m_currentitem->screenX()-86, m_currentitem->screenY()+10);
	m_newedge->setText("");
	m_newedge->show();
}

void Browser::menuPopup(BrowserItem *item, bool edge) {
	m_currentitem = item;
	m_edgeclick = edge;
	m_menu->popup(m_global);
}

void Browser::updateScreen() {
	if (m_changed) {
		m_changed = false;
		update();
	}
}

void cadence::findFile(char *buf, const char *filename) {
	int i=0;
	struct stat fileinfo;
	
	while (cadence::LocalFile::s_path[i] != 0) {
		strcpy(buf, cadence::LocalFile::s_path[i]);
		strcat(buf, "/");
		strcat(buf, filename);
		if (stat(buf, &fileinfo) == 0) {
			return;
		}
		i++;
	}
	//std::cout << "ui::Browser: Unable to find file: " << filename << "\n";
	strcpy(buf,filename);
}

void Browser::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton)
		m_dragstart = event->pos();
}

void Browser::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - m_dragstart).manhattanLength()
			< QApplication::startDragDistance())
		return;

	BrowserItem *item = m_roots.front()->findItem(event->x()-10, event->y()-10);
	if (item == 0) return;
	
	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;

	char buf[200];
	if (item->edgeClick())
		item->getEdge().toString(buf,200);
	else
		item->getValue().toString(buf,200);
	strcat(buf," ");
	
	mimeData->setText(buf);
	drag->setMimeData(mimeData);

	Qt::DropAction dropAction = drag->start(Qt::CopyAction | Qt::MoveAction);
}

void Browser::mouseReleaseEvent(QMouseEvent *event) {
	BrowserItem *item;
	m_global = event->globalPos();
	
	//Find relevant root widget.
	if (event->x() >= 10 && event->y() >= 10) {
		item = m_roots.front();
		if (event->x() <= 10+item->width() && event->y() <= 10+item->height()) {
			item->mouseClick(event->x()-10, event->y()-10, event->button());
		}
	}
}

void Browser::mouseDoubleClickEvent(QMouseEvent *event) {
	BrowserItem *item = m_roots.front()->findItem(event->x()-10, event->y()-10);
	if (item == 0) return; 
	m_currentitem = item;
	
	m_lineedit->move(item->screenX()+14, item->screenY()-6);
	m_lineedit->setText(item->getValueLabel());
	m_lineedit->selectAll();
	m_lineedit->show();
	m_lineedit->setFocus(Qt::MouseFocusReason);
}

void Browser::finishEdit() {
	if (m_lineedit->text() == "new") {
		m_currentitem->getParent().set(m_currentitem->getEdge(), OID::create());
	} else {
		m_currentitem->getParent().set(m_currentitem->getEdge(), OID((const char*)(m_lineedit->text().toAscii())));
	}
	m_lineedit->hide();
}

void Browser::finishNewEdge() {
	if (m_edgeclick)
		m_currentitem->getParent().set(OID((const char*)(m_newedge->text().toAscii())), Null);
	else
		m_currentitem->getValue().set(OID((const char*)(m_newedge->text().toAscii())), Null);
	m_newedge->hide();
}

void Browser::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	
	for (std::list<BrowserItem*>::iterator i = m_roots.begin(); i != m_roots.end(); i++) {
		(*i)->paint(painter, 10, 10);
		resize((*i)->width()+20, (*i)->height()+40);
	}
}
