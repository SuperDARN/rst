Radar Software Toolkit
========

In addition to this code, you will need the following packages:

Debian 8.7 | Mint 18.1 | OpenSUSE 42.2 | Ubuntu 16.04
---------- | --------- | ------------- | ------------
gcc | libc6-dev | gcc | libhdf5-serial-dev
libhdf5-serial-dev | libncurses5-dev | hdf5-devel | libncurses-dev
libnetcdf-dev | libnetcdf-dev | libpng16-devel | libnetcdf-dev
libncurses | libpng16-dev | libX11-devel | libpng12-dev
libpng12-dev | libx11-dev | libXext6 | libx11-dev
libx11-dev | libxext-dev | libXext-devel | libxext-dev
libxext-dev | | netcdf | netpbm
netpbm | | netcdf-devel |
 | | | ncurses-devel |
 | | | zlib-devel |

You will also need the CDF (Common Data Format) library which can be downloaded from NASA.
You can find the latest release at: http://cdf.gsfc.nasa.gov/


## Install notes:


1. Upon cloning the repository (with git clone or downloading zip file), make sure the RST
   environment variables are properly set.  In particular, make sure `CDF_PATH` in
   ~/rst/.profile/base.bash and `IDL_IPATH` in ~/rst.profile/idl.bash are correct.
   (Note: for users without access to IDL, modifying the `IDL_IPATH` environment variable is
   not required).

2. Load the RST environment variables.  For example, this is accomplished in linux by modifying
   the ~/.bashrc file by adding:

        # bash profile for rst
        export RSTPATH="INSTALL LOCATION"/rst
        . $RSTPATH/.profile.bash

   where the INSTALL LOCATION is the path with the RST repository that has been copied to your
   computer.  In order to load the environment variables you just setup, you'll need to close 
   your current terminal and open a new terminal.

3. Run `make.build` from the command line.  This runs a helper script that sets up other 
   compiling code.

4. Run `make.code superdarn rst` to compile all of the code.  This runs a script to find
   all of the source codes and compile them into binaries.  A log of this compilation is
   stored in ~/rst/log.  The source code for make.build and make.code can be found in
   ~/rst/build/script/


### Historical Version Log


- 3.3   -  Final bug fixes and updates of the 3.x series
- 3.2   -  First release with DLM support
- 3.1   -  First release of ROS 3
- 2.11  -  added support for I&Q sample analysis
- 2.10  -  bug fixes to the fitacfex library and rnk hardware.dat
- 2.09  -  fixed known errors in the documentation - gold release
- 2.08  -  further bug fixes, fitacfex and documentation system
           included. 
- 2.07  -  introduced fitacf version 2.0 and numerous bug fixes
- 2.06  -  introduction of the origin flag and the beam azimuth parameter
           together with numerous bug fixes
- 2.05  -  modification to deal with arbitrary numbers of radar beams
- 2.04  -  bug fixes from the deployment of the Radar Operating System
- 2.03  -  added ksh enviroment as an option plus more bug fixes
- 2.02  -  more bug fixes and more XML documentation completed.
- 2.01  -  various bug fixes and improvements to the IDL libraries
- 2.00  -  code adopted as official release.
- 1.07  -  numerous bug fixes.
- 1.06  -  completed IDL interfaces for grid and map data, incorporated help
           and error messages derived from XML documentation.
- 1.05  -  adopted the DataMap format for grid and map data, incorporated
           outline documentation and fixed a lot of bugs.
- 1.04  -  general bug fixes, addition of Mac OS X support
- 1.03  -  implemented the legacy IDL interfaces and incorporated the
           data tables into this release
- 1.02  -  incorporated the IDL interfaces and fixed a lot of bugs
- 1.01  -  initial revision of the code.

