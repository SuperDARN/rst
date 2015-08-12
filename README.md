VTRST3.5
========

In addition to this code, you will need the following packages:

IDL 8.X

netcdf (dev)

CDF 3.X

CDF can be downloaded from NASA.  You can find the lastest release at: http://cdf.gsfc.nasa.gov/


netpbm

libpng (dev)

zlib

ncurses (dev)

It is HIGHLY recommended to setup git on your local computer as updates are easily downloaded to
your computer.


##Install notes (There is a more complete document somewhere, but we'll start with this):


1. Upon cloning the repository (with git clone or downloading zip file), load the RST environment 
   variables.  For example, this is accomplished in linux by modifying the ~/.bashrc file by
   adding:

        # bash profile for rst
        export RSTPATH="INSTALL LOCATION"/VTRST3.5
        . $RSTPATH/.profile.bash

   where the INSTALL LOCATION is the path with the VTRST3.5 repository has been copied to on your
   computer.  In order to load the environment, you'll need to close your current terminal and
   open a new terminal.

2. With CDF library, you will need to copy a few choice files into certain places.  Namely:

        Copy the cdf.h file into ~/VTRST3.5/codebase/analysis/src.lib/cdf/rcdf.1.5/include/
        Copy the libcdf.a and libcdf.so into ~/VTRST3.5/lib/


3. Run 'make.build' from the command line

4. Run 'make.code superdarn rst' to compile all of the code.


Again...this may not be everything, but we'll start here.



##To do's:

- Need to restore functionality to dmaptoncdf.1.11.  This package was tarred up so that the code 
  could compile.  Whenever this package is in the codebase, the rst doesn't compile as it is 
  missing a .h file.
