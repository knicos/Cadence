#include <cadence/cadence.h>
#include <cadence/file.h>
#include <cadence/doste/type_traits.h>
#include <cadence/memory.h>
#include <QtGui>

using namespace cadence;

QApplication *qtapp=0;

int main(int argc, char *argv[]) {
	qtapp = new QApplication(argc,argv);
	cadence::initialise(argc, argv);

	doste::root["notations"]["dasm"]["run"] = NEW LocalFile("linux.dasm");
	doste::root["notations"]["dasm"]["run"] = NEW LocalFile("config.dasm");
	
	while (doste::root["running"] == True) {
		cadence::update();
		if (qtapp) qtapp->processEvents();
	}
	
	cadence::finalise();
	delete qtapp;
}
