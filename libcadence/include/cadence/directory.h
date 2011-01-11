//Get the files contained in a directory

#ifndef _DOSTE_DIRECTORY_H_
#define _DOSTE_DIRECTORY_H_


#include <cadence/doste/handler.h>
#include <cadence/dstring.h>
#include <cadence/vector.h>

#undef MAX_PATH //damn windows
#define MAX_PATH	400
#define MAX_ENTRIES	100

namespace cadence {
	struct DirEntry {
		bool available;
		dstring name;
		char path[MAX_PATH];
		doste::OID entries[MAX_ENTRIES];
		int size;
	};

	class Directory : public doste::Handler {
		public:
		Directory();
		~Directory();

		bool handle(doste::Event &evt);

		private:
		Vector<DirEntry*> m_dirs;

		void search(DirEntry *ent);
		doste::OID addDirectory(const char *, const char *);
		doste::OID addFile(const char *, const char *);
		doste::OID lookupEntry(int id, const doste::OID &entry);
	};
};

#endif
