/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_DIRECTION_H_
#define _XNV_DIRECTION_H_

/**
 * Type for axis-aligned directions
 */
typedef enum XnVDirection
{
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_BACKWARD,
	DIRECTION_FORWARD,
	DIRECTION_ILLEGAL
} XnVDirection;

/**
 * The available axes
 */
typedef enum XnVAxis
{
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_ILLEGAL
} XnVAxis;

#if __cplusplus
#define INLINE_FUNC inline
#else
#define INLINE_FUNC
#endif

/**
 * Translate from a direction to the axis it follows
 */
INLINE_FUNC XnVAxis XnVDirectionAsAxis(XnVDirection eDir)
{
	switch (eDir)
	{
	case DIRECTION_LEFT:
	case DIRECTION_RIGHT:
		return AXIS_X;
	case DIRECTION_UP:
	case DIRECTION_DOWN:
		return AXIS_Y;
	case DIRECTION_BACKWARD:
	case DIRECTION_FORWARD:
		return AXIS_Z;
	default:
		return AXIS_ILLEGAL;
	}
}

/**
 * Translate from a direction to its name
 */
INLINE_FUNC const XnChar* XnVDirectionAsString(XnVDirection eDir)
{
	switch (eDir)
	{
	case DIRECTION_LEFT:
		return "Left";
	case DIRECTION_RIGHT:
		return "Right";
	case DIRECTION_UP:
		return "Up";
	case DIRECTION_DOWN:
		return "Down";
	case DIRECTION_BACKWARD:
		return "Backward";
	case DIRECTION_FORWARD:
		return "Forward";
	default:
		return "Unknown";
	}
}

#endif // _XNV_DIRECTION_H_
