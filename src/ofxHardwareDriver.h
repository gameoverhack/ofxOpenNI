/*
 * ofxHardwareDriver.h
 *
 * Copyright 2011 (c) Matthew Gingold http://gingold.com.au
 * Originally forked from a project by roxlu http://www.roxlu.com/ 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */


#ifndef _H_OFXHARDWAREDRIVER
#define _H_OFXHARDWAREDRIVER

#include "ofMain.h"

#if defined (TARGET_OSX)
// based on ROS kinect_aux.cpp driver and freenect tilt.c for controlling motor and accelerometers 

#include <libusb.h>

// VID and PID for Kinect and motor/acc/leds
#define MS_MAGIC_VENDOR 0x45e
#define MS_MAGIC_MOTOR_PRODUCT 0x02b0
// Constants for accelerometers
#define GRAVITY 9.80665
#define FREENECT_COUNTS_PER_G 819.
// The kinect can tilt from +31 to -31 degrees in what looks like 1 degree increments
// The control input looks like 2*desired_degrees
#define MAX_TILT_ANGLE 31.
#define MIN_TILT_ANGLE (-31)

// these enums ripped from libfreenect.h

/// Enumeration of LED states
/// See http://openkinect.org/wiki/Protocol_Documentation#Setting_LED for more information.
typedef enum {
	LED_OFF              = 0, /**< Turn LED off */
	LED_GREEN            = 1, /**< Turn LED to Green */
	LED_RED              = 2, /**< Turn LED to Red */
	LED_YELLOW           = 3, /**< Turn LED to Yellow */
	LED_BLINK_GREEN      = 4, /**< Make LED blink Green */
	// 5 is same as 4, LED blink Green
	LED_BLINK_RED_YELLOW = 6, /**< Make LED blink Red/Yellow */
} led_options;

/// Enumeration of tilt motor status
typedef enum {
	TILT_STATUS_STOPPED = 0x00, /**< Tilt motor is stopped */
	TILT_STATUS_LIMIT   = 0x01, /**< Tilt motor has reached movement limit */
	TILT_STATUS_MOVING  = 0x04, /**< Tilt motor is currently moving to new position */
} tilt_status_code;

/// Data from the tilt motor and accelerometer
typedef struct {
	float                   accelerometer_x; /**< Raw accelerometer data for X-axis, see FREENECT_COUNTS_PER_G for conversion */
	float                   accelerometer_y; /**< Raw accelerometer data for Y-axis, see FREENECT_COUNTS_PER_G for conversion */
	float                   accelerometer_z; /**< Raw accelerometer data for Z-axis, see FREENECT_COUNTS_PER_G for conversion */
	int						tilt_angle;      /**< Raw tilt motor angle encoder information */
	tilt_status_code		tilt_status;     /**< State of the tilt motor (stopped, moving, etc...) */
} raw_tilt_state;

// TODO: probably only works for one device at the moment -> make it work for many!!!

class ofxHardwareDriver {
public:
	
	ofxHardwareDriver();
	~ofxHardwareDriver();
	
	void	setup(int index = 0);
	void	update();
	
	void	setTiltAngle(int angle);
	int		getTiltAngle();
	
	void	setLedOption(uint16_t option);
	
	ofPoint	getAccelerometers();
	
	int		tilt_angle;
	
	void shutDown();
    
    libusb_context *ctx;
	
private:
	
	libusb_device_handle *dev;
	
	raw_tilt_state tilt_state;
	
};

#endif
#endif