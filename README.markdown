THIS IS THE EXPERIMENTAL NEW APIÉsome or most of below may not apply!!!

OFXOPENNI FOR MAC & WIN
=======================
The ofxOpenNI module is a wrapper for the openNI + NITE + SensorKinect libraries/middleware for openFrameworks. 

Tested and working on Mac OSX (10.6.8), Linux (Ubuntu 10.10 64 & 32), Windows 7 (VS2010 and Codeblocks)

Latest changes (09/01/20112):
* Updated to latest drivers (OpenNI 1.5.2.7 unstable, NITE 1.5.2.7 , SensorKinect (Avin) 5.1.0.25) for Mac portable dylibs.
* Tested with Win32 latest unstable and linux 64 latest unstable drivers and is working (NOTE: Win 7 64 bit users I may need to add a 64 bit version of the lib)...will test soon
* Added a weird hack to make sure the ofRootPath is set before calling an init on the Context - for some reason on of007 I need to do this?!?
* Deprecated support for of062 - all example projects are for of007: Xcode, Codeblocks (Win and Linux64), VS2010
* Fixed a bug in the example where my demo of MaskPixels was making everything go black...for some reason glEnableBlend(GL_DST_COLOR, GL_ZERO) is not behaving like I expect
* Restructured the examples folder, including going back to having the config folder already in the bin/data/openni path

Previous changes:
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

Please make your pull requests on the Develop or Experimental branches!!!

NOTES ON THE EXAMPLE(S)
=======================

I've tried to include a bit of every feature in the example. As a consequence it's become a bit of a beast. Some things to note:

* Multiple hand tracking is not supported natively in openNI. The way it works is that a GestureGenerator works out when your waving, raising or clicking (pushing your hand forward). Based on the coordinates of the detected gesture, hand tracking is instantiated for that hand. In openNI examples you would then turn off gesture recognition and just track the hand. When the hand is lost you turn on gesture recognition again. But in order to get multiple hands it is necessary to continue looking for gestures. This means it is necessary to filter gesture recognition for the hand(s) that are already recognised. I'm doing that two ways: 1) Gestures recognised within a set distance from already recognised hands are ignored; 2) Gestures recognised within a set time from the last recognised gesture are ignored. Both of these vars, distance and time can be adjusted with setMinDistBetweenHands and setMinTimeBetweenHands respectively. Play with these to tweak tracking. Or modify them if they aren't working great ;-)

* Running masks AND point clouds simultaneously chugs. Try running one or the other. Try lowering the max number of tracked usersÉ

* Best performance seems to be on Mac and Windows (Codeblocks)

NOTE ABOUT PORTABILITY
======================

* On Mac OSX I've compiled the OpenNI, NITE and SensorKinect drivers and then modified the dylibs so they can sit in the bin/data/openni/libs folder. These are truly portable.
* On Windows I realised my solution was never really portable, and after banging my head against the problem I've decided that either a) it's not possible and/or b) I can't do it. Either way you're gonna have to install the drivers as per the instructions below. Copying the contents of ofxOpenNI/win/copy_to_data_openni_path (ie., the folder called libs and the file OpenNI.lib that is in it) really just saves you the trouble of pointing VS or CB at this file in your OpenNI install path. I've left it like this for now as people do seem to find linking confusing at first, but if you wanna do away with it completely then simply (;-) let your compiler know where OpenNI.lib is on your hard drive. A final note: this means that all the config settings inside data/openni/config are not doing anything on Windows; go find them in your openNI install path instead!
* On Linux it's a similar story except more so: either a) I can't do it or b) I can't do it. You don't need to copy anything to the data/openni path and the config files in data/openni/data are similarly cosmetic - go find them scattered in /usr/bin/, /usr/lib/, /usr/include/ni etc 

DRIVERS & GETTING THE EXAMPLES TO WORK
======================================

Mac OS X:

You don't need to do any install of drivers if you don't want, just:

- Do the git magic above or somehow else get a copy of ofxOpenNI into your addons directory
- Copy 'example' folder to yourofdir/apps/yourworkingdir
- Copy the folder called 'lib' from 'ofxOpenNI/mac/copy_to_data_openni_path' to your 'bin/data/openni' directory of your example.

Windows & Linux:

- Install openNI (http://www.openni.org/Downloads/OpenNIModules.aspx OR http://github.com/openni)
- Install NITE (http://www.openni.org/Downloads/OpenNIModules.aspx)
- Install SensorKinect drivers (make sure you use https://github.com/avin2/SensorKinect modified drivers, not the standard Prime Sense drivers if you're using a Kinect...might be able to use their openNI install too, but I haven't checked)

then on Windows: 

- Copy 'example' folder to yourofdir/apps/yourworkingdir
- Copy the 'ofxOpenNI/win/copy_to_data_openni_path' to your bin/data/openni directory of your example or link as per above PORTABILITY notes.

or on Linux:
- Just copy 'example' folder to yourofdir/apps/yourworkingdir

DRIVERS & SETTING UP YOUR OWN PROJECTS
======================================

* The easiest thing to do is to make a copy of the included examples and then gut the testApp.h and testApp.cpp for making a new project, but below are some pointers on making your own projects. Forgive me (and remind me) if I forget some steps.

Mac OS X:

You don't need to do any install of drivers if you don't want, just:

- Do the git magic above or somehow else get a copy of ofxOpenNI into your addons directory
- Make a new project (in 062 by copying emptyExample folder from the apps/examples/ directory into your working path -> something like ofFolder/apps/dev/)
- Copy the folder called 'lib' from 'ofxOpenNI/mac/copy_to_data_openni_path' to your 'bin/data/openni' directory of your emptyExample
- In Xcode make a new Group called 'ofxOpenNI' under 'Addons' (right or control click the 'Addons' Group, select Add->New Group)
- Drag the 'src' and 'include' folders from inside the ofxOpenNI folder (in your addons directory) into the Group 'ofxOpenNI' you just made.
- Drag the folder 'lib' from bin/data/openni (the one you just copied) into the Group 'ofxOpenNI'
- Add #include "ofxOpenNI.h" at the top of your testApp.h (or in whatever class you're using ofxOpenNI)

Windows (VS2010):

- Sorry will put these up shortly!!

Windows (Codeblocks):

- Install the drivers as per above
- Do the git magic above or somehow else get a copy of ofxOpenNI into your addons directory
- Make a new project
- Copy the 'ofxOpenNI/win/copy_to_data_openni_path' to your bin/data/openni directory (OPTIONAL)
- Right-click the 'emptyExample' project folder on the left hand side of the interface and select Add Files
- Add all the files from 'src', 'include/openni', 'include/nite'
- If you're on windows, DO NOT add the libusb files in the include folder
- Go to Project->Properties->Link and add OpenNI to the libraries
- Go to Project->Properties->Search Paths and add ../../../addons/ofxOpenNI/include/openni, ../../../addons/ofxOpenNI/include/nite, ../../../addons/ofxOpenNI/src
- add the path 'bin/data/openni/lib' (if you did the OPTIONAL step above) or navigate to the lib folder of your openNI install; something like c:/Programs(x86)/openNI/lib/
- Add #include "ofxOpenNI.h" at the top of your testApp.h (or in whatever class you're using ofxOpenNI)

Linux (Codeblocks):

- Install the drivers as per above
- Do the git magic above or somehow else get a copy of ofxOpenNI into your addons directory
- Make a new project
- Right-click the 'emptyExample' project folder on the left hand side of the interface and select Add Files
- Add all the files from 'src'
- If you're on windows, DO NOT add the libusb files in the include folder
- Go to Project->Properties->Link and add OpenNI to the libraries
- Go to Project->Properties->Search Paths and add ../../../addons/ofxOpenNI/include/openni, ../../../addons/ofxOpenNI/include/nite, ../../../addons/ofxOpenNI/src
- Add #include "ofxOpenNI.h" at the top of your testApp.h (or in whatever class you're using ofxOpenNI)


