#include <cadence/cadence.h>
#include <cadence/file.h>
#include <cadence/doste/type_traits.h>
#include <cadence/memory.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> 

using namespace cadence;

int main(int argc, char *argv[]) {
	struct stat fileinfo;
	
	cadence::initialise(argc, argv);

	doste::root["notations"]["dasm"]["run"] = NEW LocalFile("linux.dasm");
	doste::root["notations"]["dasm"]["run"] = NEW LocalFile("config.dasm");
	
	cadence::run();
	cadence::finalise();
}
