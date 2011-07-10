/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




#ifndef __XN_OS_WIN32_H__
#define __XN_OS_WIN32_H__

//---------------------------------------------------------------------------
// Prerequisites
//---------------------------------------------------------------------------
#ifndef WINVER						// Allow use of features specific to Windows XP or later
#define WINVER 0x0501
#endif
#ifndef _WIN32_WINNT				// Allow use of features specific to Windows XP or later
#define _WIN32_WINNT 0x0501
#endif						
#ifndef _WIN32_WINDOWS				// Allow use of features specific to Windows 98 or later
#define _WIN32_WINDOWS 0x0410
#endif
#ifndef _WIN32_IE					// Allow use of features specific to IE 6.0 or later
#define _WIN32_IE 0x0600
#endif
#define WIN32_LEAN_AND_MEAN			// Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_DEPRECATE 1	// Undeprecate CRT functions

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <io.h>
#include <assert.h>
#include <float.h>
#include <crtdbg.h>

//---------------------------------------------------------------------------
// Files
//---------------------------------------------------------------------------
/** A generic handle type. */ 
typedef	HANDLE	XN_HANDLE;

/** A file handle type. */ 
typedef HANDLE	XN_FILE_HANDLE;

/** The value of an invalid file handle. */
static const XN_FILE_HANDLE XN_INVALID_FILE_HANDLE = INVALID_HANDLE_VALUE;

/** A string that specifies the current directory. */ 
#define XN_FILE_LOCAL_DIR ".\\"

/** The file directory separator. */ 
#define XN_FILE_DIR_SEP "\\"

/** The file extension separator. */ 
#define XN_FILE_EXT_SEP "."

/** The file "all" wildcard. */ 
#define XN_FILE_ALL_WILDCARD "*"

/** The newline separation string. */
#define XN_NEW_LINE_SEP "\r\n"

//---------------------------------------------------------------------------
// INI Files
//---------------------------------------------------------------------------
/** A string that specifies the extension of INI files. */ 
#define XN_INI_FILE_EXT "ini"

/** The maximum allowed INI string length (in bytes). */
// Note: This must always be big enough to contain a 32-bit number!
#define XN_INI_MAX_LEN 256

//---------------------------------------------------------------------------
// Shared Libraries
//---------------------------------------------------------------------------
/** A shared library handle type. */ 
typedef	HMODULE	XN_LIB_HANDLE;

/** A string that specifies the prefix of shared library files. */ 
#define XN_SHARED_LIBRARY_PREFIX ""

/** A string that specifies the postfix of shared library files. */ 
#define XN_SHARED_LIBRARY_POSTFIX ".dll"

//---------------------------------------------------------------------------
// Threads
//---------------------------------------------------------------------------
/** A Xiron thread type. */ 
typedef	HANDLE XN_THREAD_HANDLE;

/** A Xiron thread ID. */ 
typedef	DWORD XN_THREAD_ID;

/** A Xiron process ID. */
typedef DWORD XN_PROCESS_ID;

/** The thread entry point function prototype. */
typedef	LPTHREAD_START_ROUTINE XN_THREAD_PROC_PROTO;

/** The thread entry point function definition. */
#define	XN_THREAD_PROC DWORD WINAPI

/** The thread return function. */
#define	XN_THREAD_PROC_RETURN(ret) return(ret)

/** The thread passable data pointer type. */ 
typedef	LPVOID XN_THREAD_PARAM;

//---------------------------------------------------------------------------
// Time Outs
//---------------------------------------------------------------------------
/** The mutex lock infinite timeout. */
#define	XN_WAIT_INFINITE INFINITE

//---------------------------------------------------------------------------
// Mutex
//---------------------------------------------------------------------------
/** A Xiron mutex type. */ 
typedef	HANDLE XN_MUTEX_HANDLE;

//---------------------------------------------------------------------------
// Critical Sections
//---------------------------------------------------------------------------
/** A Xiron critical sections type. */ 
typedef	CRITICAL_SECTION* XN_CRITICAL_SECTION_HANDLE;

//---------------------------------------------------------------------------
// Events
//---------------------------------------------------------------------------
/** A Xiron event type. */ 
typedef	HANDLE XN_EVENT_HANDLE;

//---------------------------------------------------------------------------
// Timer
//---------------------------------------------------------------------------
/** The Xiron OS timer structure. */ 
typedef struct XnOSTimer
{
	XnDouble dTicksPerTimeUnit;
	LARGE_INTEGER nStartTick;
} XnOSTimer;

//---------------------------------------------------------------------------
// Network
//---------------------------------------------------------------------------
/** The network host name and port separator. */ 
#define XN_NETWORK_HOST_PORT_SEP ":"

//---------------------------------------------------------------------------
// Swaps
//---------------------------------------------------------------------------
#define XN_PREPARE_VAR16_IN_BUFFER(var) (var)
#define XN_PREPARE_VAR32_IN_BUFFER(var) (var)
#define XN_PREPARE_VAR64_IN_BUFFER(var) (var)
#define XN_PREPARE_VAR_FLOAT_IN_BUFFER(var) (var)

#endif //__XN_OS_WIN32_H__