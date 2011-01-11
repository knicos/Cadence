#include <cadence/directory.h>
#include <cadence/doste/event.h>

#ifdef WIN32
#include <windows.h>
#pragma warning ( disable : 4996 )
#endif

#ifdef LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#endif

using namespace cadence;
using namespace cadence::doste;

Directory::Directory() : Handler(OID(0,101,0,0), OID(0,101,0,100000)) {
	addDirectory(".", ".");
}

Directory::~Directory() {

}

bool Directory::handle(Event &evt) {
	switch (evt.type()) {
	case Event::GET:	evt.result(lookupEntry(evt.dest().d(), evt.param<0>()));
				return true;
	case Event::ADDDEP:	return true;
	case Event::ADD_REF:	return true;
	case Event::REMOVE_REF:	return true;
	default:		return false;
	}
}

doste::OID Directory::lookupEntry(int id, const doste::OID &entry) {
	if (id < 0 || id >= m_dirs.size()) return doste::Null;

	DirEntry *dir = m_dirs[id];

	if (entry.isLongLong()) {
		if (!dir->available) search(dir);
		if ((int)entry < 0 || (int)entry >= dir->size) return doste::Null;
		return dir->entries[(int)entry];
	} else {
		if (entry == OID("name")) {
			return dir->name;
		} else if (entry == doste::Size) {
			if (!dir->available) search(dir);
			return dir->size;
		} else {
			return Null;
		}
	}
}

void Directory::search(DirEntry *ent) {
	//if(directory()==dvm::Null) return 0;
	//dstring dir = directory();
	ent->available = true;
	
	int n = 0;
	
	#ifdef WIN32
	WIN32_FIND_DATA findFileData;
	
	int tLen = strlen(ent->path);
	
	#ifdef UNICODE //convert directory to wchar_t for unicode
	wchar_t *wDir = new wchar_t[tLen+3];
	mbstowcs(wDir, ent->path, tLen+1);
	#else //not visual studio (MinGW) uses char
	char *wDir = new char[tLen+3];
	strcpy(wDir, ent->path);
	#endif
	
	wDir[tLen+0] = '/';	//this string must be a search pattern, so add /* to path
	wDir[tLen+1] = '*';
	wDir[tLen+2] = 0;
	HANDLE hFind = FindFirstFile(wDir, &findFileData);	
	
	if(hFind  != INVALID_HANDLE_VALUE) {
		do {
			//convert to char* from wchar_t
			char f[64];
			for(int i=0; i<64; i++) {
				f[i] = (char) findFileData.cFileName[i];
				if(f[i]==0) break;
			}
			//add directory or file to database
			if(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				ent->entries[n] = addDirectory(ent->path, f);
			} else {
				ent->entries[n] = addFile(ent->path, f);
			}
			n++;
		} while(FindNextFile(hFind, &findFileData));
	}
	FindClose(hFind);
	#endif
    
    #ifdef LINUX
	DIR *dp;
	struct stat st;
	struct dirent *dirp;
	char buf[500];
	if((dp = opendir((const char*)ent->path)) != NULL){
		while ((dirp = readdir(dp)) != NULL) {
			strcpy(buf,ent->path);
			strcat(buf,"/");
			strcat(buf,dirp->d_name);
			stat(buf, &st);
			if(S_ISDIR(st.st_mode)) {
				//std::cout << "  add dir " << dirp->d_name << "\n";
				ent->entries[n] = addDirectory(ent->path, dirp->d_name);
			} else {
				ent->entries[n] = addFile(ent->path, dirp->d_name);
			}
			n++;
		}
		closedir(dp);
	}
    #endif

	ent->size = n;
}

doste::OID Directory::addFile(const char *path, const char* name) {
	doste::OID nf = OID::create();
	nf.set(doste::Type, OID("LocalFile"), true);
	nf.set("filename", dstring(name, true), true);
	nf.set("base", dstring(path, true), true);

	Event *evt = new Event(Event::ADD_REF, nf);
	evt->send(Event::FLAG_FREE);

	return nf;
}

doste::OID Directory::addDirectory(const char *path, const char* name) {
	DirEntry *nd = new DirEntry;
	nd->available = false;
	nd->size = 0;
	//nd->entries = new OID[MAX_ENTRIES];
	nd->name = dstring(name, true);
	strcpy(nd->path, path);
	strcat(nd->path, "/");
	strcat(nd->path, name);
	int id = m_dirs.size();
	m_dirs.add(nd);
	return OID(0,101,0,id);
}
