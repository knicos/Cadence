#ifndef _OBJECTTREE_
#define _OBJECTTREE_

#include <QTreeWidget>
#include <doste/agent.h>
#include <doste/dvm/oid.h>
#include <QMenu>

#define OBJTYPE_UNKNOWN			0
#define OBJTYPE_INTEGER			1
#define OBJTYPE_FLOAT			2
#define OBJTYPE_NULL			3
#define OBJTYPE_BOOLEAN			4
#define OBJTYPE_INDEX			5
#define OBJTYPE_DEV_SCREEN		6
#define OBJTYPE_DEV_CONSOLE		7
#define OBJTYPE_DEV_CORE		8
#define OBJTYPE_DEV_IDE			9
#define OBJTYPE_DEV_CLOCK		10
#define OBJTYPE_SYSTEM			11
#define OBJTYPE_CHAR			12
#define OBJTYPE_VKEY			13
#define OBJTYPE_SPECIAL			14
#define OBJTYPE_STRING			15
#define OBJTYPE_FONT			16
#define OBJTYPE_COLOR			17
#define OBJTYPE_WINDOW			18
#define OBJTYPE_BUTTON			19
#define OBJTYPE_LABEL			20
#define OBJTYPE_GROUPBOX		21
#define OBJTYPE_OPENGL			22
#define OBJTYPE_LINEEDIT		23
#define OBJTYPE_LINE			24
#define OBJTYPE_RECTANGLE		25
#define OBJTYPE_CIRCLE			26
#define OBJTYPE_VECTOR			27
#define OBJTYPE_POLYGON			28
#define OBJTYPE_MODEL			29
#define OBJTYPE_LIGHT			30
#define OBJTYPE_DOCUMENT		31
#define OBJTYPE_IMAGE			32
#define OBJTYPE_OBJECT			33
#define OBJTYPE_CURRENT			34
#define OBJTYPE_TEXTURE			35

using namespace doste;
using namespace doste::dvm;

class ObjectItem : public QTreeWidgetItem, public Agent
{
	public:
		ObjectItem(QTreeWidget *parent, OID o, OID attrib, OID value, QString name, bool def);
		ObjectItem(ObjectItem *parent, OID o, OID attrib, OID value, QString name, bool def);
		~ObjectItem();

		void Update(OID parent, OID attrib, OID value, QString name, bool def);

		OID GetObject() { return pobject; };
		OID GetAttrib() { return attrib; };
		OID GetValue() { return object; };

		//void OnChange(OID o, OID attrib, OID value);
		void Expand();
		void Collapse();

	private:
		OID object;
		OID attrib;
		OID pobject;
		QPixmap *pm_open;
		QPixmap *pm_closed;
};

class ObjectListView : public QTreeWidget
{
	public:
		ObjectListView(QWidget *parent);
		~ObjectListView();

		QString GeneratePath(QTreeWidgetItem* item);

	private:
		void startDrag();
};

class ObjectTree : public QWidget
{
	Q_OBJECT

	public:
		ObjectTree(QWidget *parent);
		~ObjectTree();

	private:
		ObjectListView *list;
		QMenu *menu;
		ObjectItem *popitem;
		//AttribNameDialog *aname;
		bool copying;
		bool cutting;
		bool referencing;
		QString copypath;

	private slots:
		void onItem(QTreeWidgetItem *i);
		void itemClicked(QTreeWidgetItem *i);
		void expanded(QTreeWidgetItem *item);
		void collapsed(QTreeWidgetItem *item);
		void itemRenamed(QTreeWidgetItem *item, int col, const QString &text);
		void rightButtonClicked(QTreeWidgetItem *item, const QPoint &pos, int col);

		void addAttribute();
		void deleteObject();
		void saveObject();
		void monitorObject();
		void objcopy();
		void objcut();
		void objref();
		void traceObject();
		void editObject();
		void viewObject();
};

#endif

