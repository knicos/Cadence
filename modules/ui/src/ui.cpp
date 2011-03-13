#include <cadence/agent.h>
#include <QtGui>
#include "inputwindow.h"
#include "mainwindow.h"
#include "msghandler.h"
#include "cqwidget.h"

using namespace cadence;
using namespace cadence::doste;

extern "C" void initialise(const cadence::doste::OID &base) {
	//new MsgAgent(base);
	
	//IWindow *input = new IWindow();
	MainWindow *mainwin = new MainWindow(0,0);
	mainwin->show();
	
	Object::registerType<CQWidget>();
}

extern "C" void finalise() {
	
}
