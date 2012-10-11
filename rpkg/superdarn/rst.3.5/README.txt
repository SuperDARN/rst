RST $project$ $package$ $version$ ($date$) 
==========================================
R.J.Barnes

Version Log
===========

3.3	Final bug fixes and updates of the 3.x series
3.2	First release with DLM support
3.1	First release of ROS 3

Introduction
------------

The RST suite contains a wide variety of data analysis and visualization
software for working with ITM data. It is the principal analysis software
package for working with SuperDARN Radar data. The software is distributed in
the form of source code, data tables and pre-compiled libraries and binaries
for various platforms.

Additional Required/Suggested Software
--------------------------------------

To provide certain features, additional software must be installed. This 
software includes:

+ IDL 8.X
+ netcdf
+ CDF 3.X
+ netpbm
+ libpng
+ zlib


To compile the X software the X and Xext development libraries must 
be installed

Features
--------

The RST software includes:

+ IGRF11 
+ AACGM2005
+ Astronomical Algorithms for accurate determination of Magnetic Local Time
+ Coordinate transforms for GEO/GSM/GSE
+ DataMap self-describing file format
+ rPlot graphics 

Installation
------------

1. Linux self-extracting archive:
   "$project$-$package$.$version$.linux.sh"

Copy the self extracting archive to the directory that you wish to install 
and execute the archive:

. $project$-$package$.$version$.linux.sh

This will create an "rst" sub-directory containing the software.  

The software requires a number of environment variables that are defined in
the file "rst/.profile.bash". Edit this file as
needed and either include or copy it into your profile.

2. OS X (Darwin) self-extracting archive:
   "$project$-$package$.$version$.darwin.sh"

Copy the self extracting archive to the directory that you wish to install
and execute the archive:

. $project$-$package$.$version$.darwin.sh

This will create an "rst" sub-directory containing the software.

The software requires a number of environment variables that are defined in
the file "rst/.profile.bash". Edit this file as
needed and either include or copy it into your profile.

3.  OS X (Darwin) software package:
   "$project$-$package$.$version$.pkg"

Click on the package and follow the installation instructions. The software
will be installed in the "/Applications/rst" directory.

The software requires a number of environment variables that are defined in
the file "/Applications/rst/.profile.bash". 
Edit this file as needed and either include or copy it into your profile.

Recompiling the code
--------------------

The software is shipped with pre-compiled libraries and binaries. If
you need to recompile the software at any time; first recompile the 
build architecture by typing:

make.build

To recompile the software type:

make.code $project$ $package$

Logs of the compilation process are stored in the "log" sub-directory.







