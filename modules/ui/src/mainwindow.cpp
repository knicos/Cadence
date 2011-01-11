#include "mainwindow.h"
#include "inputwindow.h"
#include <QToolBar>
#include <QToolButton>
#include <QIcon>
#include <QMenuBar>
#include <QFileDialog>
#include <QStringList>
#include <cadence/cadence.h>
#include <cadence/doste/doste.h>
#include <QScrollArea>

using namespace cadence;
using namespace cadence::doste;

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	char *buf = new char[1000];
	resize(1000,600);
	setWindowTitle("Cadence IDE");
	//AddNotifiable(this, 0);

	//for (int i=0; i<MAX_ATTRIBUTES; i++)
	//	attribs[i] = 0;
	//attribobj = Null;

	//for (int i=0; i<MAX_OBJECTS; i++)
	//	objects[i] = 0;
	//objectobj = Null;

	//for (int i=0; i<MAX_INPUTS; i++)
	//	inputs[i] = 0;
	//curinput = -1;

	QWidget *cwidget = new QWidget(this);
	setCentralWidget(cwidget);
	mainlayout = new QVBoxLayout(cwidget);

	//QToolBar *mainbar = addToolBar("Main Tool Bar");
	//mainbar->setFloatable(false);
	//mainbar->setLabel("Main Tool Bar");
	//mainbar->addAction(QIcon(QPixmap("icons/launch.png")),"Run", this, SLOT(fileRun()));
	
	//mainbar->addAction(QIcon(QPixmap("icons/filenew.png")), "New", this, SLOT(fileNew()));
	//mainbar->addAction(QIcon(QPixmap("icons/fileopen.png")), "Open", this, SLOT(fileOpen()));
	//mainbar->addAction(QIcon(QPixmap("icons/filesave.png")), "Save", this, SLOT(fileSave()));
	//mainbar->addAction(QIcon(QPixmap("icons/remove.png")), "Close", this, SLOT(fileClose()));

	//splithor = new QSplitter(cwidget);
	//objtree = new ObjectTree(splithor);
	splithor = new QSplitter(Qt::Horizontal);
	splitver = new QSplitter(Qt::Vertical);
	mainlayout->addWidget(splithor);
	itabs = new QTabWidget();
	history = new HistoryLog(itabs);

	browser = new Browser(history, itabs, cadence::doste::root.get("ui").get("browser"));
	browser->addRoot(cadence::doste::root);
	browser->setLayout(new QVBoxLayout());
	
	QScrollArea *browser_scroll = new QScrollArea();
	browser_scroll->setPalette(QPalette(QColor(255,255,255)));
	browser_scroll->setWidget(browser);
	
	splithor->addWidget(browser_scroll);
	splithor->addWidget(splitver);
	splitver->addWidget(itabs);
	
	//itabs->addTab(browser_scroll, "Browser");
	//screen = new Screen(itabs);
	//screen->Open();
	//itabs->addTab(screen, QIconSet(QPixmap("icons/dev_screen.png")), "Screen");

	//ObjectWindow *objwindow = new ObjectWindow(Null, itabs);
	//itabs->addTab(objwindow, QIconSet(QPixmap("icons/object.png")), "Object");
	//AttributeWindow *attribwindow = new AttributeWindow(Null, Null, itabs);
	//itabs->addTab(attribwindow, QIconSet(QPixmap("icons/object.png")), "Attribute");

	//Object::GetSystem()->Set(Index::Lookup("screen"), screen->GetOID());
	mtabs = new QTabWidget();
	splitver->addWidget(mtabs);
	//msgwin = new MessageWindow(mtabs);
	msgwin = new MsgAgent(mtabs, cadence::doste::root);
	inputwin = new IWindow(msgwin, history, "dasm");
	history->setSource(inputwin);
	modwin = new ModuleList();
	//IWindow *edenin = new IWindow(history, "eden");
	//tracewin = new TraceWindow(mtabs);
	//monitorwin = new MonitorWindow(mtabs);
	cadence::findFile(buf, "data/ui/console.png");
	itabs->addTab(inputwin, QIcon(QPixmap(buf)), "DASM Input");
	//mtabs->addTab(edenin, QIcon(QPixmap("icons/dev_console.png")), "Eden Input");
	cadence::findFile(buf, "data/ui/console.png");
	mtabs->addTab(msgwin, QIcon(QPixmap(buf)),"Messages");
	cadence::findFile(buf, "data/ui/history.png");
	mtabs->addTab(history, QIcon(QPixmap(buf)),"History");
	cadence::findFile(buf, "data/ui/module.png");
	mtabs->addTab(modwin, QIcon(QPixmap(buf)), "Modules");
	//mtabs->addTab(msgwin, QIconSet(QPixmap("icons/message.png")), "Messages");
	//mtabs->addTab(tracewin,QIconSet(QPixmap("icons/trace.png")), "Trace");
	//mtabs->addTab(monitorwin,QIconSet(QPixmap("icons/monitor.png")),"Monitors");

	MakeMenu();

	QList<int> vlist;
	//vlist.push_back(100);
	//splithor->setSizes(vlist);
	//vlist.clear();
	vlist.push_back(200);
	vlist.push_back(500);
	splithor->setSizes(vlist);
	vlist.clear();
	vlist.push_back(400);
	vlist.push_back(200);
	splitver->setSizes(vlist);

	//timer = new Clock();
	//timer->Open();

	//Compiler::ExecuteFile("ide.dos");
	delete [] buf;
}

MainWindow::~MainWindow()
{
	//timer->Close();
	//delete tracewin;
	//delete inputwin;
	//delete msgwin;
	//delete mtabs;
	//screen->Close();
	//delete objtree;
	//delete splithor;
	//delete splitver;
	//delete file;
	//delete menu;
	//delete mainlayout;
}

void MainWindow::MakeMenu()
{
	menu = menuBar();
	char *buf = new char[1000];
	statusBar();
	QMenu *file = menu->addMenu("&File");
	file->addAction(QIcon(QPixmap("icons/filenew.png")), "&New Script...", this, SLOT(fileNew()));
	file->addAction(QIcon(QPixmap("icons/fileopen.png")), "&Open Script...", this, SLOT(fileOpen()));
	file->addSeparator();
	cadence::findFile(buf, "data/ui/save.png");
	file->addAction(QIcon(QPixmap(buf)), "&Save History", history, SLOT(savehist()));
	//file->addAction(QIcon(QPixmap("icons/filesaveas.png")), "Save As...");
	//file->addAction(QIcon(QPixmap("icons/filesaveobj.png")), "S&ave Objects", this, SLOT(fileSaveObjects()));
	file->addSeparator();
	file->addAction(QIcon(QPixmap("icons/run.png")), "&Run Script", this, SLOT(fileRun()));
	file->addSeparator();
	file->addAction(QIcon(QPixmap("icons/exit.png")), "&Quit", this, SLOT(quit()));

	edit = menu->addMenu("&Edit");
	edit->addAction(QIcon(QPixmap("icons/cut.png")), "Cut", inputwin, SLOT(editCut()));
	edit->addAction(QIcon(QPixmap("icons/copy.png")), "Copy", inputwin, SLOT(editCopy()));
	edit->addAction(QIcon(QPixmap("icons/paste.png")), "Paste", inputwin, SLOT(editPaste()));
	edit->addSeparator();
	edit->addAction(QIcon(QPixmap("icons/next.png")), "&Next", inputwin, SLOT(editNext()), QKeySequence("ALT+N"));
	edit->addAction(QIcon(QPixmap("icons/prev.png")), "&Previous", inputwin, SLOT(editPrevious()), QKeySequence("ALT+P"));
	//edit->addAction(QIcon(QPixmap("icons/undo.png")), "&Undo", inputwin, SLOT(editUndo()), CTRL+Key_Z, 0);
	//edit->addAction(QIcon(QPixmap("icons/redo.png")), "Re&do", inputwin, SLOT(editRedo()), 0, 1);
	//edit->addSeparator();
	//edit->addAction(QIcon(QPixmap("icons/editcut.png")), "Cu&t", inputwin, SLOT(editCut()), CTRL+Key_X, 2);
	//edit->addAction(QIcon(QPixmap("icons/editcopy.png")), "&Copy", inputwin, SLOT(editCopy()), CTRL+Key_C, 3);
	//edit->addAction(QIcon(QPixmap("icons/editpaste.png")), "&Paste", inputwin, SLOT(editPaste()), CTRL+Key_V, 4);
	//edit->addSeparator();
	//edit->addAction("Select &All", inputwin, SLOT(editSelectall()), CTRL+Key_A, 5);
	//edit->addAction("Dese&lect", inputwin, SLOT(editClear()), 0, 6);
	//edit->addSeparator();
	//edit->addAction("&Find");
	//edit->addAction("Find &Next");
	//edit->addAction("Find Pre&vious");
	//edit->addAction("&Replace");
	//edit->addSeparator();
	//edit->addAction("&Go to Line");
	//connect(edit, SIGNAL(aboutToShow()), this, SLOT(editAboutToShow()));
	
	QMenu *view = menu->addMenu("&View");
	//view->addAction("Show Object Tree");
	//view->addAction("Detach Screen");
	//QMenu *tools = menu->addMenu("&Tools");
	QMenu *settings = menu->addMenu("&Settings");
	QMenu *help = menu->addMenu("&Help");
	help->addAction("About");
	//help->addAction("Contents");
	//help->addAction("Search");
	delete [] buf;
}

void MainWindow::fileRun()
{
	QString s = QFileDialog::getOpenFileName(this, "Run Script", QString::null, "DOSTE Scripts (*.dasm)");

	OID dasm = cadence::doste::root["notations"]["dasm"];
	((Notation*)dasm)->run((const char*)s.toAscii(),cadence::doste::root);
}

void MainWindow::fileOpen()
{
	QString s = QFileDialog::getOpenFileName(this, "Open Script", QString::null, "Scripts (*.dasm *.eden *.e *.s *.scout *.d)");

	QStringList ls = s.split('/');
	QStringList ex = ls.last().split('.');
	
	IWindow *nwin;
	if (ex.last() == "dasm") nwin = new IWindow(msgwin, history, "dasm");
	else nwin = new IWindow(msgwin, history, "eden");
	char *buf = new char[1000];
	cadence::findFile(buf, "data/ui/script.png");
	itabs->addTab(nwin, QIcon(QPixmap(buf)), ls.last());
	delete [] buf;
	itabs->setCurrentWidget(nwin);
	nwin->loadFile(s);
}

void MainWindow::fileSave()
{
	QString s = QFileDialog::getSaveFileName(this, "Save Script", QString::null, "DOSTE Scripts (*.dasm)");

	//ciwin->SaveFile(s);
}

void MainWindow::fileClose()
{
	//itabs->deleteTab(ciwin);
}

void MainWindow::fileNew()
{
	//ciwin = new InputWindow(itabs);
	//itabs->addTab(ciwin, QIconSet(QPixmap("icons/edit.png")), "File");
	//itabs->showPage(ciwin);
	
	IWindow *nwin;
	nwin = new IWindow(msgwin, history, "dasm");
	char *buf = new char[1000];
	cadence::findFile(buf, "data/ui/script.png");
	itabs->addTab(nwin, QIcon(QPixmap(buf)), "New Script");
	delete [] buf;
	itabs->setCurrentWidget(nwin);
}

void MainWindow::fileSaveObjects() {

}

void MainWindow::editAboutToShow() {

}

void MainWindow::quit() {
	cadence::doste::root["running"] = false;
}
