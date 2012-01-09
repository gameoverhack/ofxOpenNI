/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_NITE_LOG_H_
#define _XNV_NITE_LOG_H_

#include <XnLog.h>

/**
* Creation of Generators and Listeners, and their destruction
*/
#define XNV_NITE_MASK_CREATE	"XNV_NITE_MASK_CREATE"
/**
* Any Generator sending Point Create/Destroy and Input Start/Stop, and any PointListener receiving such events.
* If multi-threaded environment is used, adding to the queue also uses this log mask.
*/
#define XNV_NITE_MASK_FLOW		"XNV_NITE_MASK_FLOW"
/**
* Connection and disconnection of Listeners to Generators, and changing active controls in Flow Routers
*/
#define XNV_NITE_MASK_CONNECT	"XNV_NITE_MASK_CONNECT"

/**
* Creation and destruction of points
*/
#define XNV_NITE_MASK_POINTS	"XNV_NITE_MASK_POINTS"

/**
 * Session state changes
 */
#define XNV_NITE_MASK_SESSION	"XNV_NITE_MASK_SESSION"
/**
 * Access to the multi-thread Queue
 */
#define XNV_NITE_MASK_MT_QUEUE	"XNV_NITE_MASK_MT_QUEUE"

/**
 * Events in specific controls and detectors
 */
#define XNV_NITE_MASK_EVENTS	"XNV_NITE_MASK_EVENTS"

#endif