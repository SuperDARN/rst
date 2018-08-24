[![DOI](https://zenodo.org/badge/74060190.svg)](https://zenodo.org/badge/latestdoi/74060190)

Radar Software Toolkit
========

In addition to this code, you will need the following packages:

Debian 8.7 | Mint 18.1 | OpenSUSE 42.2 | Ubuntu 16.04 | macOS 10.12.4
---------- | --------- | ------------- | ------------ | --------------
gcc | libc6-dev | gcc | libhdf5-serial-dev | libhdf5
libhdf5-serial-dev | libncurses5-dev | hdf5-devel | libncurses-dev | libnetcdf
libnetcdf-dev | libnetcdf-dev | libpng16-devel | libnetcdf-dev | libcurses
libncurses | libpng16-dev | libX11-devel | libpng12-dev | libpng16
libpng12-dev | libx11-dev | libXext6 | libx11-dev | libx11
libx11-dev | libxext-dev | libXext-devel | libxext-dev | cdf
libxext-dev | | netcdf | netpbm | netpbm (10.77.03_2+x11)
netpbm | | netcdf-devel | |
 | | | ncurses-devel | |
 | | | zlib-devel | |

You will also need the CDF (Common Data Format) library which can be downloaded from NASA.
You can find the latest release at: http://cdf.gsfc.nasa.gov/
For macOS it is also available through macports, as are all listed dependencies


## Install notes:


1. Upon obtaining the software (with git clone or downloading a zip file), make sure the RST
   environment variables are properly set.   In `rst/.profile.bash`:

       OSTYPE="linux" for any linux operating system or "darwin" for macOS
       SYSTEM="linux" or "darwin" as appropriate

   In `rst/.profile/base.bash`, check to make sure these paths are appropriate:

   `XPATH, NETCDF_PATH, CDF_PATH`

   If you are running macOS and run into issues with the X11 libraries, you may
   need to add a symbolic link.

   If you have IDL, check to see that `IDL_IPATH` in `rst/.profile/idl.bash` is correct.
   (Note: for users without access to IDL, modifying the `IDL_IPATH` environment variable is
   not required).

2. Load the RST environment variables.  For example, this is accomplished in linux by modifying
   the `~/.bashrc` file by adding:

        # bash profile for rst
        export RSTPATH="INSTALL LOCATION"/rst
        . $RSTPATH/.profile.bash

   where the INSTALL LOCATION is the path with the RST repository that has been copied to your
   computer.  In order to load the environment variables you just setup, you'll need to close 
   your current terminal and open a new terminal, or from the command line type:
   
       source ~/.bashrc

3. Run `make.build` from the command line.  You may need to change directory to `$RSTPATH/build/script`.
   This runs a helper script that sets up other compiling code.

   If you are running macOS Sierra or later, you may have trouble for this step
   and step 4 if you run the commands in iTerm.app, due to a non-standard bash
   implimentation.  These can be avoided by running the installation in a
   xterm terminal like XQuartz.

4. In the same directory run `make.code` to compile all of the code.
   This runs a script to find all of the source codes and compile them into binaries.
   A log of this compilation is stored in `$RSTPATH/log`.

   4a.	 If you don't have IDL and the IDL skip in make.code fails, you will see an error
   	 upon the inability to locate `idl_export.h`.  If this happens:

	 ```
	 cd $RSTPATH/codebase/superdarn/src.lib/tk
	 tar -P -czvf idl.tar.gz idl
	 rm -rf idl
	 cd $RSTPATH/build/script
	 make.code
	 ```

   4b.	 If the order of make.code is executed incorrectly, you will see an error upon
   	 the inability to locate a header file (i.e. `sza.h`).  If this happens (using
	 `sza.h` as an example):

	 ```
	 find $RSTPATH -name "sza.h"
	 >> $RSTPATH/codebase/imagery/src.lib/sza.1.9/include/sza.h
	 cd $RSTPATH/codebase/imagery/src.lib/sza.1.9/src
	 make clean
	 make
	 cd $RSTPATH/build/script
	 make.code
	 ```

5. To compile the html documentation, run `make.doc` from the command line. You may need
   to modify the `URLBASE` environment variable in `$RSTPATH/.profile/rst.bash` for the
   links in the html pages to function correctly.  Online documentation is available at:

   https://superdarn.github.io/rst/index.html


### Historical Version Log


- 3.3   -  Final bug fixes and updates of the 3.x series (Aug 2011)
- 3.2   -  First release with DLM support (Nov 2010)
- 3.1   -  First release of ROS 3 (Jun 2010)
- 2.11  -  added support for I&Q sample analysis (Mar 2008)
- 2.10  -  bug fixes to the fitacfex library and rnk hardware.dat (May 2007)
- 2.09  -  fixed known errors in the documentation - gold release (Mar 2007)
- 2.08  -  further bug fixes, fitacfex and documentation system
           included. (Mar 2007)
- 2.07  -  introduced fitacf version 2.0 and numerous bug fixes (Feb 2007)
- 2.06  -  introduction of the origin flag and the beam azimuth parameter
           together with numerous bug fixes (Aug 2006)
- 2.05  -  modification to deal with arbitrary numbers of radar beams (Mar 2006)
- 2.04  -  bug fixes from the deployment of the Radar Operating System (Feb 2006)
- 2.03  -  added ksh enviroment as an option plus more bug fixes (Sep 2005)
- 2.02  -  more bug fixes and more XML documentation completed. (Aug 2005)
- 2.01  -  various bug fixes and improvements to the IDL libraries (Jul 2005)
- 2.00  -  code adopted as official release. (Apr 2005)
- 1.07  -  numerous bug fixes. (Apr 2005)
- 1.06  -  completed IDL interfaces for grid and map data, incorporated help
           and error messages derived from XML documentation. (Nov 2004)
- 1.05  -  adopted the DataMap format for grid and map data, incorporated
           outline documentation and fixed a lot of bugs. (Oct 2004)
- 1.04  -  general bug fixes, addition of Mac OS X support (Aug 2004)
- 1.03  -  implemented the legacy IDL interfaces and incorporated the
           data tables into this release (Aug 2004)
- 1.02  -  incorporated the IDL interfaces and fixed a lot of bugs (Aug 2004)
- 1.01  -  initial revision of the code. (Jul 2004)
