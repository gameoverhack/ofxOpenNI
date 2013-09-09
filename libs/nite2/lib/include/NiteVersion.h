/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0                                                        *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#include <OniPlatform.h>

#define NITE_VERSION_MAJOR		2
#define NITE_VERSION_MINOR		2
#define NITE_VERSION_MAINTENANCE	0
#define NITE_VERSION_BUILD		10

#define NITE_BRIEF_VERSION_STRING \
	ONI_STRINGIFY(NITE_VERSION_MAJOR) "." \
	ONI_STRINGIFY(NITE_VERSION_MINOR) "." \
	ONI_STRINGIFY(NITE_VERSION_MAINTENANCE) \
	" (Build " ONI_STRINGIFY(NITE_VERSION_BUILD) ")"

/** OpenNI version (in numeric format): (OpenNI major version * 100000000 + OpenNI minor version * 1000000 + OpenNI maintenance version * 10000 + OpenNI build version). */
#define NITE_VERSION (NITE_VERSION_MAJOR*100000000 + NITE_VERSION_MINOR*1000000 + NITE_VERSION_MAINTENANCE*10000 + NITE_VERSION_BUILD)

#define NITE_VERSION_STRING \
	NITE_BRIEF_VERSION_STRING  "-" \
	ONI_PLATFORM_STRING " (" ONI_TIMESTAMP ")"
