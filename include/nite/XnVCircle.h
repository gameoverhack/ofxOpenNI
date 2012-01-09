/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_CIRCLE_H_
#define _XNV_CIRCLE_H_

#include <XnOpenNI.h>

/**
* A (2D) circle is defined by its center and radius
*/
struct XnVCircle
{
	XnPoint3D ptCenter;
	XnFloat fRadius;
};

#endif // _XNV_CIRCLE_H_
