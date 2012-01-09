/*
 * ofxHardwareDriver.cpp
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


#include "ofxHardwareDriver.h"

#if defined (TARGET_OSX)

ofxHardwareDriver::ofxHardwareDriver() {
	printf("Init lib_usb control of Kinect Motor, LEDs and accelerometers");
}

ofxHardwareDriver::~ofxHardwareDriver() {
	shutDown();
}

void ofxHardwareDriver::setup(int index)
{
	//libusb_context *ctx;
	libusb_init(&ctx);
	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	ssize_t cnt = libusb_get_device_list (ctx, &devs); //get the list of devices
	if (cnt < 0)
	{
		printf("No device on USB\n");
		return;
	}
	
	int nr_mot(0);
	for (int i = 0; i < cnt; ++i)
	{
		struct libusb_device_descriptor desc;
		const int r = libusb_get_device_descriptor (devs[i], &desc);
		if (r < 0)
			continue;
		
		printf("Device: %i Vendor: %i Product: %i\n", i, desc.idVendor, desc.idProduct);
		
		// Search for the aux
		if (desc.idVendor == MS_MAGIC_VENDOR && desc.idProduct == MS_MAGIC_MOTOR_PRODUCT)
		{
			// If the index given by the user matches our camera index
			if (nr_mot == index)
			{
				if ((libusb_open (devs[i], &dev) != 0) || (dev == 0))
				{
					printf("Cannot open aux %d\n", index);
					return;
				}
				// Claim the aux
				printf("Openning device aux %d on 0\n", i);
				libusb_claim_interface (dev, 0);
				break;
			}
			else
				nr_mot++;
		}
	}
	
	libusb_free_device_list (devs, 1);  // free the list, unref the devices in it
	
	// capture current tilt angle (all good if we don't move the kinect)
	update();
	tilt_angle = ofClamp(getTiltAngle(), -30, 30); // to be sure, to be sure ;-)
	//setTiltAngle(0);	// leaving this reset out for now as that way installations 
	// do not need to be reset ever startup however can cause 
	// strange behaviour if the kinect is tilted in between 
	// application starts eg., the angle continues to be set 
	// even when app not running...which is odd...
	
}

void ofxHardwareDriver::update() 
{
	//freenect_context *ctx = dev->parent;
	uint8_t buf[10];
	uint16_t ux, uy, uz;

	int ret = libusb_control_transfer(dev, 0xC0, 0x32, 0x0, 0x0, buf, 10, 10);
	if (ret != 10) {
		printf("Error in accelerometer reading, libusb_control_transfer returned %d\n", ret);
		return;// ret < 0 ? ret : -1;
	}
	
	ux = ((uint16_t)buf[2] << 8) | buf[3];
	uy = ((uint16_t)buf[4] << 8) | buf[5];
	uz = ((uint16_t)buf[6] << 8) | buf[7];
	
	//the documentation for the accelerometer (http://www.kionix.com/Product%20Sheets/KXSD9%20Product%20Brief.pdf)
	//states there are 819 counts/g
	tilt_state.accelerometer_x = (int16_t)ux/FREENECT_COUNTS_PER_G*GRAVITY;
	tilt_state.accelerometer_y = (int16_t)uy/FREENECT_COUNTS_PER_G*GRAVITY;
	tilt_state.accelerometer_z = (int16_t)uz/FREENECT_COUNTS_PER_G*GRAVITY;
	tilt_state.tilt_angle = (int)buf[8];
	tilt_state.tilt_status = (tilt_status_code)buf[9];
	
	//return ret;
}

void ofxHardwareDriver::setTiltAngle(int angle)
{
	
	ofClamp(angle, -30, 30); // just to make super sure...
	
	uint8_t empty[0x1];
	
	angle = (angle<MIN_TILT_ANGLE) ? MIN_TILT_ANGLE : ((angle>MAX_TILT_ANGLE) ? MAX_TILT_ANGLE : angle);
	angle = angle * 2;
	const int ret = libusb_control_transfer(dev, 0x40, 0x31, (uint16_t)angle, 0x0, empty, 0x0, 0);
	if (ret != 0)
	{
		printf("Error in setting tilt angle, libusb_control_transfer returned %i", ret);
		return;
	}
	
	//tilt_angle = angle;
}

int ofxHardwareDriver::getTiltAngle() {
	return tilt_state.tilt_angle/2;
}

ofPoint ofxHardwareDriver::getAccelerometers() {
	return ofPoint(tilt_state.accelerometer_x, tilt_state.accelerometer_y, tilt_state.accelerometer_z);
}

void ofxHardwareDriver::setLedOption(uint16_t option)
{
	uint8_t empty[0x1];

	const int ret = libusb_control_transfer(dev, 0x40, 0x06, (uint16_t)option, 0x0, empty, 0x0, 0);
	if (ret != 0)
	{
		printf("Error in setting tilt angle, libusb_control_transfer returned %i", ret);
	}
}

void ofxHardwareDriver::shutDown() {
	//setTiltAngle(0);	// leaving this reset out for now as that way installations 
						// do not need to be reset ever startup however can cause 
						// strange behaviour if the kinect is tilted in between 
						// application starts eg., the angle continues to be set 
						// even when app not running...which is odd...
	libusb_exit(ctx);
}

#endif
