/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_NITE_VERSION_H_
#define _XNV_NITE_VERSION_H_

#define XNV_NITE_MAJOR_VERSION 1
#define XNV_NITE_MINOR_VERSION 3
#define XNV_NITE_MAINTENANCE_VERSION 0
#define XNV_NITE_BUILD_VERSION 18

/** Nite Framework version (in brief string format): "Major.Minor.Maintenance (Build)" */ 
#define XNV_NITE_BRIEF_VERSION_STRING \
	XN_STRINGIFY(XNV_NITE_MAJOR_VERSION) "." \
	XN_STRINGIFY(XNV_NITE_MINOR_VERSION) "." \
	XN_STRINGIFY(XNV_NITE_MAINTENANCE_VERSION) \
	" (Build " XN_STRINGIFY(XNV_NITE_BUILD_VERSION) ")"

/** Nite Framework version (in numeric format): (major version * 100000000 + minor version * 1000000 + maintenance version * 10000 + build version). */
#define XNV_NITE_VERSION (XNV_NITE_MAJOR_VERSION*100000000 + XNV_NITE_MINOR_VERSION*1000000 + XNV_NITE_MAINTENANCE_VERSION*10000 + XNV_NITE_BUILD_VERSION)

/** Nite Framework version (in string format): "Major.Minor.Maintenance.Build-Platform (MMM DD YYYY HH:MM:SS)". */ 
#define XNV_NITE_VERSION_STRING \
	XNV_NITE_BRIEF_VERSION_STRING  "-" \
	XN_PLATFORM_STRING " (" XN_TIMESTAMP ")"

#endif // _XNV_NITE_FW_VERSION_H_
