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
| | ncurses-devel |
| | zlib-devel |

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

