#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>
#include <QBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QMenu>
#include "inputwindow.h"
#include "browser.h"
#include "msghandler.h"
#include "modules.h"


class MainWindow : public QMainWindow
{
	Q_OBJECT

	QBoxLayout *mainlayout;
	QMenuBar *menu;

	IWindow *inputwin;
	HistoryLog *history;
	//TraceWindow *tracewin;
	MsgAgent *msgwin;
	ModuleList *modwin;
	//MonitorWindow *monitorwin;
	QSplitter *splitver;
	QSplitter *splithor;
	QTabWidget *mtabs;
	QTabWidget *itabs;
	//Screen *screen;
	//ObjectTree *objtree;
	Browser *browser;
	QMenu *edit;

	//AttributeWindow *attribs[MAX_ATTRIBUTES];
	//InputWindow *inputs[MAX_INPUTS];
	//ObjectWindow *objects[MAX_OBJECTS];
	//int curinput;
	//OID objectobj;
	//OID attribobj;

	public:
	MainWindow(QWidget *parent=0, Qt::WindowFlags flags=0);
	~MainWindow();

	void MakeMenu();

	private slots:
	void fileNew();
	void fileOpen();
	void fileRun();
	void fileSave();
	void fileClose();
	void fileSaveObjects();
	void editAboutToShow();
	void quit();
};

#endif
