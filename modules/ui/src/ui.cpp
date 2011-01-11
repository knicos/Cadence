#include <cadence/agent.h>
#include <QtGui>
#include "inputwindow.h"
#include "mainwindow.h"
#include "msghandler.h"

using namespace cadence;
using namespace cadence::doste;

QApplication *qtapp=0;
int argc = 0;

extern "C" void initialise(const cadence::doste::OID &base) {
	qtapp = new QApplication(argc,0);

	//new MsgAgent(base);
	
	//IWindow *input = new IWindow();
	MainWindow *mainwin = new MainWindow(0,0);
	mainwin->show();
}

extern "C" void cadence_update() {
	if (qtapp) qtapp->processEvents();
}

extern "C" void finalise() {
	delete qtapp;
}
