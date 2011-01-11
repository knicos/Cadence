#include <list>
#include <cadence/memory.h>

struct STrack
{
	void *p;
	unsigned int size;
	int line;
	const char *fname;
};

std::list<STrack*> tracks;

void AddTrack(void *p, unsigned int size, int line, const char *fname)
{
	if (tracks.size() < 5000)
	{
		STrack *t = (STrack*)malloc(sizeof(STrack));
		t->p = p;
		t->size = size;
		t->line = line;
		t->fname = fname;
		tracks.push_back(t);
	}
}

bool RemoveTrack(void *p)
{
	/*for (std::list<STrack*>::iterator i=tracks.begin(); i!=tracks.end(); i++)
	{
		if ((*i) == 0)
			continue;

		if ((*i)->p == p)
		{
			free(*i);
			tracks.erase(i);
			return true;
		}
	}*/
	return false;
}

void DisplayLeaks()
{
	for (std::list<STrack*>::iterator i=tracks.begin(); i!=tracks.end(); i++)
	{
		if ((*i) == 0)
			continue;

		std::cout << "Leak: File=" << (*i)->fname << " Line=" << (*i)->line << " Size=" << (*i)->size << std::endl;
		free(*i);
		(*i) = 0;
		//tracks.erase(i);
	}
}
