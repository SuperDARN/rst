Radar Software Toolkit
========

In addition to this code, you will need the following packages:

*Debian 8.7*

- gcc
- libhdf5-serial-dev
- libnetcdf-dev
- libncurses
- libpng12-dev
- libx11-dev
- libxext-dev
- netpbm

*Mint 18.1*

- libc6-dev
- libncurses5-dev
- libnetcdf-dev
- libpng16-dev
- libx11-dev
- libxext-dev

*OpenSUSE 42.2*

- gcc
- hdf5-devel
- libpng16-devel
- libX11-devel
- libXext6
- libXext-devel
- netcdf
- netcdf-devel
- ncurses-devel
- zlib-devel

*Ubuntu 16.04*

- libhdf5-serial-dev
- libncurses-dev
- libnetcdf-dev
- libpng12-dev
- libx11-dev
- libxext-dev
- netpbm

You will also need the CDF (Common Data Format) library which can be downloaded from NASA.
You can find the latest release at: http://cdf.gsfc.nasa.gov/


##Install notes:


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

