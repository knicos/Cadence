#ifdef WIN32

#if BUILD_DVM_DLL
# define DVMIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DVMIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#else
#define DVMIMPORT
#endif
