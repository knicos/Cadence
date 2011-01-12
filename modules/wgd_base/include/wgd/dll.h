#ifndef _WGD_DLL_
#define _WGD_DLL_

#ifdef WIN32

#if BUILD_BASE_DLL
# define BASEIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define BASEIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#if BUILD_RESOURCES_DLL
# define RESIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define RESIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#if BUILD_WIDGETS_DLL
# define WIDIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define WIDIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#if BUILD_BASE2D_DLL
# define BASE2DIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define BASE2DIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#if BUILD_BASE3D_DLL
# define BASE3DIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define BASE3DIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#if BUILD_MODELS_DLL
# define MODELSIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define MODELSIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#if BUILD_HEIGHTMAP_DLL
# define HEIGHTMAPIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define HEIGHTMAPIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#if BUILD_INPUT_DLL
# define INPUTIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define INPUTIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#else
#define BASEIMPORT
#define RESIMPORT
#define WIDIMPORT
#define BASE2DIMPORT
#define BASE3DIMPORT
#define MODELSIMPORT
#define HEIGHTMAPIMPORT
#define INPUTIMPORT
#endif

#endif
