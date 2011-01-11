#include "objecttree.h"
#include <iostream>
//#include <QDragObject>
#include <QCursor>
#include <QFileDialog>
#include <QTextStream>
#include <stdio.h>
#include <stdlib.h>

const char *icon_files[] = {
						"icons/unknown.png",		//Unknown
						"icons/integer.png",		//Integer
						"icons/float.png",		//Float
						"icons/null.png",		//Null
						"icons/boolean.png",		//Boolean
						"icons/index.png",		//Index
						"icons/dev_screen.png",		//Dev_Screen
						"icons/dev_console.png",		//Dev_Console
						"icons/dev_core.png",		//Dev_Core
						"icons/dev_ide.png",		//Dev_IDE
						"icons/dev_clock.png",		//Dev_Clock
						"icons/system.png",		//System
						"icons/character.png",		//Char
						"icons/vkey.png",		//VKEY
						"icons/special.png",		//Special
						"icons/string.png",		//String
						"icons/font.png",		//Font
						"icons/color.png",		//Color
						"icons/window.png",		//Window
						"icons/button.png",		//Button
						"icons/label.png",		//Label
						"icons/groupbox.png",		//Groupbox
						"icons/opengl.png",		//Opengl
						"icons/lineedit.png",		//Lineedit
						"icons/line.png",		//Line
						"icons/rectangle.png",		//Rectangle
						"icons/circle.png",		//Circle
						"icons/vector.png",		//Vector
						"icons/polygon.png",		//Polygon
						"icons/model.png",		//Model
						"icons/light.png",		//Light
						"icons/document.png",		//Document
						"icons/image.png",		//Image
						"icons/object.png",		//Object
						"icons/current.png"
					};

char *icon_deffile = "icons/definition.png";
QPixmap *icon_def;
QPixmap *icon_array;

ObjectItem::ObjectItem(QTreeWidget *parent, OID o, OID attrib, OID value, QString name, bool def)
	: QTreeWidgetItem(parent)
{
	//setExpandable(true);
	//setDragEnabled(true);
	sortChildren(0, Qt::AscendingOrder);
	
	object = Null;
	Update(o,attrib,value,name,def);
}

ObjectItem::ObjectItem(ObjectItem *parent, OID o, OID attrib, OID value, QString name, bool def)
	: QTreeWidgetItem(parent)
{
	//setExpandable(true);
	//setRenameEnabled(0, true);
	//setRenameEnabled(1, true);
	//setDragEnabled(true);
	sortChildren(0, Qt::AscendingOrder);

	object = Null;
	Update(o,attrib,value,name,def);
}

ObjectItem::~ObjectItem()
{
	//Object *obj = Object::Lookup(object);
	//if (obj != 0)
	//{
	//	obj->Open();
	//	obj->RemoveNotifiable(this);
	//	obj->Close();
	//}

	//Do i need to delete all children?
}

void ObjectItem::Update(OID parent, OID attrib, OID value, QString name, bool def)
{
	char buf[50];

	//Object *obj = Object::Lookup(object);
	//if (obj != 0)
	//{
	//	obj->Open();
	//	obj->RemoveNotifiable(this);
	//	obj->Close();
	//}

	this->object = value;
	this->attrib = attrib;
	this->pobject = parent;

	//obj = Object::Lookup(object);
	//if (obj != 0)
	//{
	//	obj->Open();
	//	obj->AddNotifiable(this, 0);
	//	obj->Close();
	//}

	//int objtype = GUIFactory::IdentifyType(object, attrib);
	int objtype = OBJTYPE_INTEGER;
	pm_open = &icon_array[objtype];
	pm_closed = pm_open;
	//setPixmap(0, *pm_closed);

	if (objtype == OBJTYPE_STRING)
	{
		DString obstr(object);
		obstr.toString(buf, 48);
		setText(1, QString().sprintf("\"%s\"", buf));
	} else if (objtype == OBJTYPE_INTEGER ||
				objtype == OBJTYPE_FLOAT ||
				objtype == OBJTYPE_BOOLEAN ||
				objtype == OBJTYPE_INDEX ||
				objtype == OBJTYPE_CHAR)
	{
		object.toString(buf, 48);
		setText(1, buf);
	}

	//if (def)
		//setPixmap(2, *icon_def);
}

void ObjectItem::Expand()
{
	char buf[50];
	bool def;

	for (OID::iterator i=object.begin(); i!=object.end(); i++) {
		def = false;
		(*i).toString(buf,48);
		new ObjectItem(this, object, *i, object.get(*i), buf, def);
	}

	//sort();
	//setPixmap(0, *pm_open);
}

void ObjectItem::Collapse()
{
	QTreeWidgetItem *next;
	QTreeWidgetItem *cur = firstChild();
	while (cur != 0)
	{
		next = cur->nextSibling();
		delete cur;
		cur = next;
	}

	//setPixmap(0, *pm_closed);
}

/*void ObjectItem::OnChange(OID obj, OID attrib, OID value)
{
	char buf[50];
	bool def;

	Object *object;

	ObjectItem *cur = (ObjectItem*)firstChild();
	while (cur != 0)
	{
		if (cur->GetAttrib() == attrib)
		{
			def = false;
			object = Object::Lookup(obj);
			if (object != 0)
			{
				object->Open();
				if (object->GetDefinition(attrib) != 0)
					def = true;
				else
					def = false;
				object->Close();
			}
			//delete cur;
			OIDToString(attrib, buf, 48);
			//new ObjectItem(this, obj, attrib, value, buf, def);
			cur->Update(obj,attrib,value,buf,def);
			sort();
			return;
		}
		cur = (ObjectItem*)cur->nextSibling();
	}

	if (isOpen())
	{
		unsigned char flags = 0;

		def = false;
		object = Object::Lookup(obj);
		if (object != 0)
		{
			object->Open();
			flags = object->GetFlags(attrib);
			if (object->GetDefinition(attrib) != 0)
				def = true;
			else
				def = false;
			object->Close();
		}

		if ((flags & FLAG_HIDDEN) == 0)
		{
			OIDToString(attrib, buf, 48);
			new ObjectItem(this, obj, attrib, value, buf, def);
			sort();
		}
	}
}*/

ObjectTree::ObjectTree(QWidget *parent)
	: QVBox(parent)
{
	copying = false;
	cutting = false;
	referencing = false;

	list = new ObjectListView(this);
	list->addColumn("Objects");
	list->addColumn("Value");
	list->addColumn("Def");
	list->setRootIsDecorated(true);
	list->setItemMargin(3);
	list->setTreeStepSize(15);
	list->setAcceptDrops(true);

	menu = new QMenu(list);
	menu->addAction(QIcon(QPixmap("icons/add.png")), "Add", this, SLOT(addAttribute()));
	menu->addAction(QIcon(QPixmap("icons/remove.png")), "Delete", this, SLOT(deleteObject()));
	menu->addAction(QIcon(QPixmap("icons/object.png")), "View", this, SLOT(viewObject()));
	menu->addAction(QIcon(QPixmap("icons/filesaveobj.png")), "Save...", this, SLOT(saveObject()));
	menu->addSeparator();
	menu->addAction(QIcon(QPixmap("icons/editcut.png")), "Cut", this, SLOT(objcut()));
	menu->addAction(QIcon(QPixmap("icons/editcopy.png")), "Copy", this, SLOT(objcopy()));
	menu->addAction(QIcon(QPixmap("icons/definition.png")), "Reference", this, SLOT(objref()));
	menu->addSeparator();
	menu->addAction(QIcon(QPixmap("icons/trace.png")), "Trace", this, SLOT(traceObject()));
	menu->addAction(QIcon(QPixmap("icons/monitor.png")), "Monitor");
	menu->addAction(QIcon(QPixmap("icons/edit.png")), "Edit", this, SLOT(editObject()));

	icon_array = new QPixmap[NUM_OBJTYPES];
	for (int i=0; i<NUM_OBJTYPES; i++)
	{
		icon_array[i] = QPixmap(icon_files[i]);
	}
	icon_def = new QPixmap(icon_deffile);
	
	new ObjectItem(list, Null, Null, doste::root, "root", false);
	//new ObjectItem(list, Null, Null, doste::Current, "current", false);

	//aname = new AttribNameDialog();

	connect(list, SIGNAL(expanded(QTreeWidgetItem*)), this, SLOT(expanded(QTreeWidgetItem*)));
	connect(list, SIGNAL(collapsed(QTreeWidgetItem*)), this, SLOT(collapsed(QTreeWidgetItem*)));
	connect(list, SIGNAL(itemRenamed(QTreeWidgetItem *, int, const QString&)), this, SLOT(itemRenamed(QTreeWidgetItem*, int, const QString&)));
	connect(list, SIGNAL(rightButtonClicked(QTreeWidgetItem *, const QPoint&, int)), this, SLOT(rightButtonClicked(QTreeWidgetItem*, const QPoint&, int)));
	connect(list, SIGNAL(onItem(QTreeWidgetItem*)), this, SLOT(onItem(QTreeWidgetItem*)));
	connect(list, SIGNAL(clicked(QTreeWidgetItem*)), this, SLOT(itemClicked(QTreeWidgetItem*)));
}

ObjectTree::~ObjectTree()
{
	//delete aname;
	delete menu;
	delete list;
	delete icon_def;
	delete [] icon_array;
}

void ObjectTree::objcopy()
{
	copying = true;
	list->setCursor(QCursor(PointingHandCursor));
	copypath = list->GeneratePath(list->selectedItem());
}

void ObjectTree::objcut()
{
	cutting = true;
	list->setCursor(QCursor(PointingHandCursor));
	copypath = list->GeneratePath(list->selectedItem());
}

void ObjectTree::objref()
{
	referencing = true;
	list->setCursor(QCursor(PointingHandCursor));
	copypath = list->GeneratePath(list->selectedItem());
}

void ObjectTree::onItem(QTreeWidgetItem *i)
{
	if (copying || cutting || referencing)
	{
		list->setSelected(i, true);
	}
}

void ObjectTree::itemClicked(QTreeWidgetItem *i)
{
	if (i == 0)
		return;

	ObjectItem *objitem = (ObjectItem*)i;

	/*if (referencing)
	{
		vector<OID> *def = Compiler::CompileDefinition(copypath, Null);

		Object *object = Object::Lookup(objitem->GetValue());
		if (object != 0)
		{
			object->Open();
			char buf[50];
			OIDToString(def->back(), buf, 50);
			aname->SetName(buf);
			if (aname->exec() == QDialog::Accepted)
			{
				object->Set(Compiler::Execute(aname->GetName()), def);
				list->setOpen(i, true);
			}
			object->Close();
		}
		
		delete def;
		list->setCursor(QCursor(ArrowCursor));
		referencing = false;
	}
	else if (copying)
	{
		vector<OID> *def = Compiler::CompileDefinition(copypath, Null);

		Object *object = Object::Lookup(objitem->GetValue());
		if (object != 0)
		{
			object->Open();
			char buf[50];
			OIDToString(def->back(), buf, 50);
			aname->SetName(buf);
			def->push_back(Index::Lookup("uniond"));
			def->push_back(Null);
			if (aname->exec() == QDialog::Accepted)
			{
				
				OID nobj = Compiler::EvalDef(def);
				object->Set(Compiler::Execute(aname->GetName()), nobj);
				Object *nobject = Object::Lookup(nobj);
				if (nobject != 0)
				{
					nobject->Open();
					nobject->Set(Parent, objitem->GetValue());
					nobject->Close();
				}

				list->setOpen(i, true);
			}
			object->Close();
		}

		delete def;
		list->setCursor(QCursor(ArrowCursor));
		copying = false;
	}
	else if (cutting)
	{
		vector<OID> *def = Compiler::CompileDefinition(copypath, Null);

		Object *object = Object::Lookup(objitem->GetValue());
		if (object != 0)
		{
			object->Open();
			char buf[50];
			OIDToString(def->back(), buf, 50);
			aname->SetName(buf);
			if (aname->exec() == QDialog::Accepted)
			{
				object->Set(Compiler::Execute(aname->GetName()), Compiler::EvalDef(def));
				list->setOpen(i, true);
			}
			object->Close();
		}

		//NEEDS TO DELETE ORIGINAL

		delete def;
		list->setCursor(QCursor(ArrowCursor));
		cutting = false;
	}*/
}

void ObjectTree::expanded(QTreeWidgetItem *item)
{
	((ObjectItem*)item)->Expand();
}

void ObjectTree::collapsed(QTreeWidgetItem *item)
{
	((ObjectItem*)item)->Collapse();
}

void ObjectTree::itemRenamed(QTreeWidgetItem *item, int col, const QString &text)
{
	ObjectItem *it = (ObjectItem*)item;

	/*if (col == 1)
	{
		OID noid;
		if (text == "")
			noid = (new Object())->GetOID();
		else
			noid = Compiler::Execute(text);
		Object *obj = Object::Lookup(it->GetObject());
		if (obj != 0)
		{
			obj->Open();
			obj->Set(it->GetAttrib(), noid);
			obj->Close();
		}
	} else if (col == 0)
	{
		OID noid = StringToOID(text);
		Object *obj = Object::Lookup(it->GetObject());
		if (obj != 0)
		{
			obj->Open();
			//obj->Rename(it->GetAttrib(), noid);
			obj->Close();
		}
	}*/
}

void ObjectTree::rightButtonClicked(QTreeWidgetItem *item, const QPoint &pos, int col)
{
	if (item != 0)
	{
		popitem = (ObjectItem*)item;
		menu->popup(pos);
	}
}

void ObjectTree::addAttribute()
{
	/*Object *obj = Object::Lookup(popitem->GetValue());
	if (obj != 0)
	{
		obj->Open();
		if (aname->exec() == QDialog::Accepted)
		{
			Object *nobj = new Object();
			nobj->Open();
			obj->Set(Compiler::Execute(aname->GetName()), nobj->GetOID());
			nobj->Set(Parent, obj->GetOID());
			nobj->Close();
		}
		obj->Close();
	}*/
}

void ObjectTree::deleteObject()
{
	/*Object *obj = Object::Lookup(popitem->GetObject());
	if (obj != 0)
	{
		obj->Open();
		obj->Set(popitem->GetAttrib(), Null);
		obj->Close();
	}
	delete popitem;
	popitem = 0;*/
}

void ObjectTree::saveObject()
{
	/*QString s = QFileDialog::getSaveFileName(
                    QString::null,
                    "DOSTE Scripts (*.dos)",
                    this,
                    "save object dialog",
                    "Choose a file" );

	FILE *fh = fopen(s, "w");
	if (fh == 0)
		return;

	char *buffer = new char[100000];		//Low limit!!!!
	QTextStream *ts = new QTextStream(fh, IO_WriteOnly);
	QString path = list->GeneratePath(list->selectedItem());
	DisplayObject(((ObjectItem*)list->selectedItem())->GetValue(), buffer, 100000, 0);
	(*ts) << QString().sprintf("%s = \n%s;", path.ascii(), buffer);
	delete ts;
	fclose(fh);
	delete [] buffer;*/
}

void ObjectTree::monitorObject()
{
	/*Object *object = Object::Lookup(Device_Ide);
	if (object != 0)
	{
		object->Open();
		OID monitor = object->Get(Index::Lookup("monitor"));
		object->Close();
		object = Object::Lookup(monitor);
		if (object != 0)
		{
			object->Open();
			object->Set(Index::Lookup("object"), ((ObjectItem*)list->selectedItem())->GetObject());
			object->Set(Index::Lookup("attribute"), ((ObjectItem*)list->selectedItem())->GetAttrib());
			object->Close();
		}
	}*/
}

void ObjectTree::traceObject()
{
	/*Object *object = Object::Lookup(Device_Ide);
	if (object != 0)
	{
		object->Open();
		OID trace = object->Get(Index::Lookup("trace"));
		object->Close();
		object = Object::Lookup(trace);
		if (object != 0)
		{
			object->Open();
			//Adds a new object to monitor.
			object->Set(Index::Lookup("object"), ((ObjectItem*)list->selectedItem())->GetObject());
			object->Set(Index::Lookup("attribute"), ((ObjectItem*)list->selectedItem())->GetAttrib());
			object->Close();
		}
	}*/
}

void ObjectTree::editObject()
{
	/*Object *object = Object::Lookup(Device_Ide);
	if (object != 0)
	{
		object->Open();
		OID attributes = object->Get(Index::Lookup("attributes"));
		object->Close();
		object = Object::Lookup(attributes);
		if (object != 0)
		{
			object->Open();
			attributes = Null;
			for (int i=0; i<MAX_ATTRIBUTES; i++)
				if (object->Get(IntToObj(i)) == Null)
				{
					attributes = (new Object())->GetOID();
					object->Set(IntToObj(i), attributes);
					break;
				}
			object->Close();
			object = Object::Lookup(attributes);
			if (object != 0)
			{
				object->Open();
				//Adds a new object to monitor.
				object->Set(Index::Lookup("object"), ((ObjectItem*)list->selectedItem())->GetObject());
				object->Set(Index::Lookup("attribute"), ((ObjectItem*)list->selectedItem())->GetAttrib());
				object->Close();
			}
		}
	}*/
}

void ObjectTree::viewObject()
{
	/*Object *object = Object::Lookup(Device_Ide);
	if (object != 0)
	{
		object->Open();
		OID attributes = object->Get(Index::Lookup("objects"));
		object->Close();
		object = Object::Lookup(attributes);
		if (object != 0)
		{
			object->Open();
			for (int i=0; i<MAX_OBJECTS; i++)
				if (object->Get(IntToObj(i)) == Null)
				{
					object->Set(IntToObj(i), ((ObjectItem*)list->selectedItem())->GetValue());
					break;
				}
			object->Close();
		}
	}*/
}

ObjectListView::ObjectListView(QWidget *parent)
	: QTreeWidget(parent)
{
}

ObjectListView::~ObjectListView()
{
}

QString ObjectListView::GeneratePath(QTreeWidgetItem *item)
{
	ObjectItem *objitem = (ObjectItem*)item;
	char buf[50];
	QString res = QString();
	
	while (objitem->parent() != 0)
	{
		objitem->GetAttrib().toString(buf, 50);
		res.prepend(QString().sprintf(" %s ", buf));
		objitem = (ObjectItem*)objitem->parent();
	}

	objitem->GetValue().toString(buf, 50);
	res.prepend(buf);

	return res;
}

void ObjectListView::startDrag()
{
	//QDragObject *d = new QTextDrag(GeneratePath(selectedItem()), this);
	//d->dragCopy();
}
 
