VTRST3.5
========

In addition to this code, you will need the following packages:

IDL 8.X

HDF5

- In order to compile netCDF, you'll need to have this installed.  For Ubuntu, this is a fairly
  straight forward package to install:
        sudo apt-get install libhdf5-serial-dev

zlib

- In order to compile netCDF, you'll also need this package.  For Ubuntu, this is a fairly
  straight forward package to install:
        sudo apt-get install zlib1g-dev

netCDF (dev)

- (New way): netCDF can be easily downloaded in linux using package management programs such as 
  apt, rpm, yum etc.  Here, search for repository names such as "netcdf-devel" or "libnetcdf-dev".
  For Ubuntu/apt do:
       sudo apt-get install libnetcdf-dev

- (Old way, and not sure if this still works): netCDF can be downloaded from UCAR at:  www.unidata.ucar.edu/software/netcdf/.  Here go to 
  Download and then "The Latest Stable netCDF-C Release, tar.gz form"  (or .zip form if you want).
  Suppose you downloaded the tarball into your home directory /home/user. Unzip and untar the 
  netCDF package: 
       cd /home/user 
       gunzip netcdf-X.X.tar.gz 
       tar xvf netcdf-X.X.tar
  The last command will create a directory called /home/user/netcdf-X.X. Enter the directory 
  /home/user/netcdfX.X and compile the netCDF software. If you choose to install netCDF system 
  wide in /usr/local/netcdf, you will need the correct privileges. In the directory 
  /home/user/netcdf-X.X, enter (with the correct privileges) 
       ./configure --prefix=/usr/local/netcdf --disable-netcdf-4 make check install



CDF 3.X

- CDF can be downloaded from NASA.  You can find the lastest release at: http://cdf.gsfc.nasa.gov/


netpbm

libpng (dev)

ncurses (dev)

It is HIGHLY recommended to setup git on your local computer as updates are easily downloaded to
your computer.


##Install notes:


1. Upon cloning the repository (with git clone or downloading zip file), load the RST environment 
   variables.  For example, this is accomplished in linux by modifying the ~/.bashrc file by
   adding:

        # bash profile for rst
        export RSTPATH="INSTALL LOCATION"/VTRST3.5
        . $RSTPATH/.profile.bash

   where the INSTALL LOCATION is the path with the VTRST3.5 repository has been copied to on your
   computer.  

   Also, you'll need to edit the location that your netCDF package was installed at by editing the
   entry in the ~/VTRST3.5/.profile/base.bash file:

        export NETCDF_PATH="/usr/include"

   The given example is where Ubuntu 12.04 placed the netcdf.h file using the apt-get package 
   manager.  

   In order to load the environment variables you just setup, you'll need to close your current terminal and
   open a new terminal.

2. With CDF library, you will need to copy a few choice files into certain places.  Namely:

        Copy the cdf.h file into ~/VTRST3.5/codebase/analysis/src.lib/cdf/rcdf.1.5/include/
        Copy the libcdf.a and libcdf.so into ~/VTRST3.5/lib/

3. Run 'make.build' from the command line.  This runs a helper script that sets up other 
compiling code.  The source code for make.build can be found in ~/build/script/

4. Run 'make.code superdarn rst' to compile all of the code.  This runs a script to find
all of the source codes and compile them into binaries.  A log of this compilation is 
stored in ${LOGPATH}.  

5. If compilation fails at aacgmdlm.1.10 you will need to do the following:

	Copy the idl_export.h file into ~/VTRST3.5/include/superdarn


Again...this may not be everything, but we'll start here now.



##To do's:

- Need to restore functionality to dmaptoncdf.1.11.  This package was tarred up so that the code 
  could compile.  Whenever this package is in the codebase, the rst doesn't compile as it is 
  missing a .h file.  __SOLVED:__ Seems as though netcdf has been added to several install
  package managers as seen in the notes above!
