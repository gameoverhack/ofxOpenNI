OFXOPENNI FOR MAC & WIN
=======================
The ofxOpenNI module is a wrapper for the openNI + NITE + SensorKinect libraries/middleware. 

Master and Develop branches tested and working in OF 0062 on Mac and Windows [VS 2010 only :-( ]

Experimental is for OF 007 - only tested on Mac, but should work in Win too...just haven't made the VS 2010 solution yet

INSTALLATION
============
Go to your addons directory, and follow these commands if you're using git

<pre>
git clone https://github.com/roxlu/ofxOpenNI.git
cd ofxOpenNI
git checkout experimental
</pre>

OR (if you're using gameoverhack's repository)

<pre>
git clone https://github.com/gameoverhack/ofxOpenNI.git
cd ofxOpenNI
</pre>

and then for working with OF 007 (git repositiory)

<pre>
git checkout experimental
</pre>

or for working with OF 0062 (stable)

<pre>
git checkout master
</pre>

[I've setup my branche's as per ofxKinect - see  https://github.com/ofTheo/ofxKinect/ for differences between master, develop and experimental]

HOW TO SETUP YOUR APPLICATION
==============================

Drivers:

- Install openNI (I used latest unstable: http://www.openni.org/downloadfiles/openni-binaries/20-latest-unstable)
- Install NITE (I used latest unstable: http://www.openni.org/downloadfiles/openni-compliant-middleware-binaries/33-latest-unstable)
- SensorKinect drivers (make sure you use https://github.com/avin2/SensorKinect modified drivers, not the standard Prime Sense drivers if you're using a Kinect...might be able to use their openNI install too, but I haven't checked)

Mac:

- Copy 'example' folder to yourofdir/apps/yourworkingdir
- Copy the 'ofxOpenNI/mac/copy_to_data_openni_path' to your bin/data/openni dire
ctory of your example.

PC:

- Copy 'example' folder to yourofdir/apps/yourworkingdir
- Copy the 'ofxOpenNI/win/copy_to_data_openni_path' to your bin/data/openni dire
ctory of your example.
