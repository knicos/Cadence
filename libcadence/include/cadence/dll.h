/*
 * includes/doste/dll.h.h
 * 
 * Windows dllimport and dllexport definitions
 * Copyright (C) 2008 Nicolas Pope
 *
 * Author(s) : Nicolas Pope
 * Date      : 2008
 * Home Page : http://www.doste.co.uk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef WIN32

#if BUILD_XARA_DLL
# define XARAIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define XARAIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#if BUILD_DVM_DLL
# define DVMIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DVMIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

#else
#define XARAIMPORT
#define DVMIMPORT
#endif
