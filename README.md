Radar Software Toolkit
========

In addition to this code, you will need the following packages:

*IDL 8.X*

*HDF5*

- In order to compile netCDF, you'll need to have this installed.  For Ubuntu, this is a fairly
  straight forward package to install:
        sudo apt-get install libhdf5-serial-dev

*zlib*

- In order to compile netCDF, you'll also need this package.  For Ubuntu, this is a fairly
  straight forward package to install:
        sudo apt-get install zlib1g-dev

*netCDF (dev)*

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


*CDF 3.X*

- CDF can be downloaded from NASA.  You can find the lastest release at: http://cdf.gsfc.nasa.gov/


*netpbm*

*libpng (dev)*

*ncurses (dev)*

*X11 library*

- For Ubuntu 14 and 16, it seems as though you'll need to install the X11 dev library. So use
sudo apt-get instal libx11-dev to install this library

*X11 extension library*

- Also for Ubuntu 14 and 16, you'll need to install the various X11 extensions. So use
sudo apt-get install libxext-dev to install this library




It is HIGHLY recommended to setup git on your local computer as updates are easily downloaded to
your computer.


##Install notes:


1. Upon cloning the repository (with git clone or downloading zip file), load the RST environment 
   variables.  For example, this is accomplished in linux by modifying the ~/.bashrc file by
   adding:

        # bash profile for rst
        export RSTPATH="INSTALL LOCATION"/rst
        . $RSTPATH/.profile.bash

   where the INSTALL LOCATION is the path with the RST repository that has been copied to your
   computer.

   Also, you'll need to edit the CDF_PATH entry in ~/rst/.profile/base.bash and the IDL_IPATH
   entry in the ~/rst/.profile/idl.bash file.

   In order to load the environment variables you just setup, you'll need to close your current terminal and
   open a new terminal.

2. Run `make.build` from the command line.  This runs a helper script that sets up other 
compiling code.  The source code for make.build can be found in ~/rst/build/script/

3. Run `make.code superdarn rst` to compile all of the code.  This runs a script to find
all of the source codes and compile them into binaries.  A log of this compilation is 
stored in ${LOGPATH}.

Again...this may not be everything, but we'll start here now.
