#ifndef _DOSTE_XNET_
#define _DOSTE_XNET_

#ifdef WIN32
#if BUILD_XNET_DLL
# define XNETIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define XNETIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */
#else
#define XNETIMPORT
#endif

#endif
