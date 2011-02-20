OFXOPENNI FOR MAC & WIN
=======================
The ofxOpenNI module is a wrapper for the openNI + NITE + SensorKinect 
libraries/middleware. Master and Develop branches tested and working in OF 0062 on Mac and Windows (VS 2010 only - currently openNI only works with Micro$oft).


NB: For VS 2010 openNI libs appear to be Release only and therefore no debugging is possible...so Build the solution and then go to your example/bin/ directory and execute the openNiSample.exe outside of the IDE...

Experimental only tested on Mac, but should work on Win as well (issue is more with getting 007 compiled in VS 2010 for me).

INSTALLATION
============
Go to your addons directory, and follow these commands

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

[I've setup my branche's as per ofxKinect - see Theo's instructions
at https://github.com/ofTheo/ofxKinect/ for differences between master, develop and experimental]

HOW TO SETUP YOUR APPLICATION
==============================

Mac:

- Copy 'example' folder to yourofdir/apps/yourworkingdir
- Copy the 'ofxOpenNI/mac/copy_to_data_openni_path' to your bin/data/openni dire
ctory of your example.

PC:

- Copy 'example' folder to yourofdir/apps/yourworkingdir
- Copy the 'ofxOpenNI/win/copy_to_data_openni_path' to your bin/data/openni dire
ctory of your example.
