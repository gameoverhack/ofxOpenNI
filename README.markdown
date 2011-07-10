OFXOPENNI FOR MAC & WIN
=======================
The ofxOpenNI module is a wrapper for the openNI + NITE + SensorKinect libraries/middleware for openFrameworks. 

Tested and working on Mac OSX (10.6.7), Linux (Ubuntu 10.10 64 & 32), Windows 7 (VS2010 and Codeblocks)

Latest changes:

* Updated to latest drivers (OpenNI 1.1.0.41 unstable, NITE 1.3.1.5 , SensorKinect (Avin) 5.0.1.32)
* Compiled these as portable dylibs for Mac OSX (see NOTE ABOUT PORTABILITY below)
* Added Codeblocks support for Windows
* Added Linux support (Codeblocks)
* Bugfixed HandTracker and GestureGenerator (was ignoring initial gestures due to timestamp issue)
* Added setMaxNumberOfUsers to UserGenerator (although memory allocation is still done with a define for MAX_NUMBER_USERS cpu intense iterations for point clouds and masks can now be limited by setting a 'soft' max using this function. TODO: make the arrays/mem dynamically allocate)
* Other minor bug fixes, etc
* Updated this readme...

INSTALLATION
============
Go to your addons directory, and follow these commands if you're using git

<pre>
git clone https://github.com/gameoverhack/ofxOpenNI.git
cd ofxOpenNI
git checkout master
</pre>

or if you wanna see what I'm working on:

<pre>
git checkout experimental
</pre>

[I've setup my branche's as per ofxKinect - see  https://github.com/ofTheo/ofxKinect/ for differences between master, develop and experimental. Basically bleeding edge stuff goes into experimental, then merges to develop and finally to master]

As of 11/07/11 master, develop and experimental branches are currently all merged.

NOTES ON THE EXAMPLE(S)
=======================

I've tried to include a bit of every feature in the example. As a consequence it's become a bit of a beast. Some things to note:

* Multiple hand tracking is not supported natively in openNI. The way it works is that a GestureGenerator works out when your waving, raising or clicking (pushing your hand forward). Based on the coordinates of the detected gesture, hand tracking is instantiated for that hand. In openNI examples you would then turn off gesture recognition and just track the hand. When the hand is lost you turn on gesture recognition again. But in order to get multiple hands it is necessary to continue looking for gestures. This means it is necessary to filter gesture recognition for the hand(s) that are already recognised. I'm doing that two ways: 1) Gestures recognised within a set distance from already recognised hands are ignored; 2) Gestures recognised within a set time from the last recognised gesture are ignored. Both of these vars, distance and time can be adjusted with setMinDistBetweenHands and setMinTimeBetweenHands respectively. Play with these to tweak tracking. Or modify them if they aren't working great ;-)

* Running masks AND point clouds simultaneously chugs. Try running one or the other. Try lowering the max number of tracked users…

* Best performance seems to be on Mac and Windows (Codeblocks)

NOTE ABOUT PORTABILITY
======================

* On Mac OSX I've compiled the OpenNI, NITE and SensorKinect drivers and then modified the dylibs so they can sit in the bin/data/openni/libs folder. These are truly portable.
* On Windows I realised my solution was never really portable, and after banging my head against the problem I've decided that either a) it's not possible and/or b) I can't do it. Either way you're gonna have to install the drivers as per the instructions below. Copying the contents of ofxOpenNI/win/copy_to_data_openni_path (ie., the folder called libs and the file OpenNI.lib that is in it) really just saves you the trouble of pointing VS or CB at this file in your OpenNI install path. I've left it like this for now as people do seem to find linking confusing at first, but if you wanna do away with it completely then simply (;-) let your compiler know where OpenNI.lib is on your hard drive. A final note: this means that all the config settings inside data/openni/config are not doing anything on Windows; go find them in your openNI install path instead!
* On Linux it's a similar story except more so: either a) I can't do it or b) I can't do it. You don't need to copy anything to the data/openni path and the config files in data/openni/data are similarly cosmetic - go find them scattered in /usr/bin/, /usr/lib/, /usr/include/ni etc 

HOW TO SETUP YOUR APPLICATION
==============================

DRIVERS
+++++++

Mac OS X:

You don't need to do any install of drivers if you don't want, just:

- Do the git magic above or somehow else get a copy of ofxOpenNI into your addons directory
- Copy 'example' folder to yourofdir/apps/yourworkingdir
- Copy the 'ofxOpenNI/mac/copy_to_data_openni_path' to your bin/data/openni directory of your example.

Windows & Linux:

- Install openNI (I used latest unstable: http://www.openni.org/downloadfiles/openni-binaries/20-latest-unstable)
- Install NITE (I used latest unstable: http://www.openni.org/downloadfiles/openni-compliant-middleware-binaries/33-latest-unstable)
- SensorKinect drivers (make sure you use https://github.com/avin2/SensorKinect modified drivers, not the standard Prime Sense drivers if you're using a Kinect...might be able to use their openNI install too, but I haven't checked)

then on Windows: 

- Copy 'example' folder to yourofdir/apps/yourworkingdir
- Copy the 'ofxOpenNI/win/copy_to_data_openni_path' to your bin/data/openni directory of your example or link as per above PORTABILITY notes.

or on Linux:
- Just copy 'example' folder to yourofdir/apps/yourworkingdir