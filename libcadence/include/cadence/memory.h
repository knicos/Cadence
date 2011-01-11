#ifndef _DMEMORY_
#define _DMEMORY_

#include <iostream>
#include <stdlib.h>

void AddTrack(void *p, unsigned int size, int line, const char *fname);
bool RemoveTrack(void *p);
void DisplayLeaks();

#ifdef DEBUG
inline void *operator new (size_t size, const char *fname, int line)
{
	void *p = malloc(size);
	AddTrack(p, size, line, fname);
	return p;
};

inline void *operator new[] (size_t size, const char *fname, int line)
{
	void *p = malloc(size);
	AddTrack(p, size, line, fname);
	return p;
};

inline void operator delete(void *p)
{
	RemoveTrack(p);
	free(p);
};

inline void operator delete[](void *p)
{
	RemoveTrack(p);
	free(p);
};
#endif

#ifdef DEBUG
#define NEW new(__FILE__,__LINE__)
#else
#define NEW new
#endif

#endif 
