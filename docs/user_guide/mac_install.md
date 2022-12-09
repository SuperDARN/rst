# Mac 

This guide is intended to provide installation instructions on **macOS** for Apple branded desktops and laptops. It begins with a list of dependencies required with instructions for installing RST. This process was designed for OS **Sierra** and above, but should still work on any machine which can run XCode. If you encounter any problems with installing RST, please create an [issue](https://github.com/superdarn/rst/issues/new) describing your problem and the error message you receive. The community will then help you solve the problem and add it into our troubleshooting section to help other users. 

Table of Contents: 
-------------------

1. [Library Requirements](#library-requirements) 
    1. [Macports](#macports)
    2. [Homebrew](#homebrew)
    3. [CDF](#cdf)
2. [Installation](#installation) 
3. [Troubleshooting](#troubleshooting)

## Library Requirements

> Warning! sudo privileges needed to install the various libraries. 

If you do not have `sudo` privileges please contact the system administrator of your system to install the follow libraries for your distribution.  

We will be installing the dependencies using a Mac compatible package manager, such as Macports or Homebrew. Make sure you have one of those installed.

Note that the names of the following dependencies have been known to change slightly depending on the date and the particular package manager used. A quick google search can often tell you the new name if it has changed - just replace the name in the install commands if it has changed.

[Macports install guide](https://www.macports.org/install.php)

[Homebrew install guide](https://docs.brew.sh/Installation)

You also need the CDF library, see below.


   Dependencies 	 |
 ------------------------| 		
 libhdf5		 |
 libncurses		 |
 libpng16		 |
 libx11			 | 		
 netpbm 		 |
 netcdf			 |

Installation line:

### Macports
    
    sudo port install libhdf5 libnetcdf libcurses libpng16 libx11 netpbm (10.77.03_2+x11)

### Homebrew
    
    sudo brew install hdf5 netcdf ncurses libpng netpbm
    
For X11, make sure you have XCode installed from the macOS App store

Now install the [CDF](#cdf)

### CDF 

!!! Note
    **Make sure you successfully installed the ncurses library for your distribution first.** 
You can find the latest release at: [http://cdf.gsfc.nasa.gov/](http://cdf.gsfc.nasa.gov)
For macOS it is also available through MacPorts, as are all listed dependencies. However, if the MacPorts installation errors or you do not use MacPorts, you may follow the instructions below.

From the above site, navigate to the `macosx` directory and download the pre-compiled binary file `cdf[XX_X]-setup_universal_binary.tar.gz`, where `XX_X` is the version number of the latest release. 

Unpack the binary using the following steps:


1. `tar -xzvf cdf[XX_X]-setup_universal_binary.tar.gz`
2. double-click on the unpacked binary to be led through installation instructions.  

Alternatively, you may follow the instructions given on the [linux page](linux_install.md), replacing:

`make OS=linux ENV=gnu all` with `make OS=macosx ENV=gnu all`.  

You can now delete the `cdfXX_X-dist` directory or binary and the `tar.gz` archive.

Now go to the [Installation](#installation)

#### TroubleShooting: 

> If you find any problems/solutions, please create a [github issue](https://github.com/superdarn/rst/issues/new) so the community can help you or add it to the documentation

**Problem**

Error: curses.h not found

**Solution**

* (Recommended) To make a symlink from lncurses library to lcurses, run the following commands in the terminal:

        ln -s /usr/lib64/libncurses.so /usr/lib64/libcurses.so
        ln -s /usr/lib64/libncurses.a /usr/lib64/libcurses.a

* To modify the Makefile change the following:  
`CURSESLIB_linux_gnu=-lcurses` to `CURSESLIB_linux_gnu=-lncurses`  
`CURSESLIB_linux_gnu32=-lcurses` to `CURSESLIB_linux_gnu32=-lncurses`  
`CURSESLIB_linux_gnu64=-lcurses` to `CURSESLIB_linux_gnu64=-lncurses`  


## Installation

1. Obtaining RST software:
    - Download the official release with a citable DOI from [Zenodo](https://doi.org/10.5281/zenodo.801458) (**recommended for most users**)
    - Download the official release from [Github](https://github.com/SuperDARN/rst/releases)
    - Clone from Github (for developers): ```git clone https://github.com/superdarn/rst/```

2. Check RST environment variables:
   Open `rst/.profile.bash` using your preferred text editor:
	```	
      	OSTYPE="darwin"
       	SYSTEM="darwin"
	```

   Open `rst/.profile/base.bash` to check paths are correctly set:

   `XPATH, NETCDF_PATH, CDF_PATH` 
   To check if the paths are set correctly locate the following header files:
   For NETCDF_PATH `locate netcdf.h`
   For CDF_PATH `locate cdf.h`
   
   - If you have **IDL**, check to see that `IDL_IPATH` in `rst/.profile/idl.bash` is correct.
   	(Note: for users without IDL, modifying the `IDL_IPATH` environment variable is
   	not required).

2. Load the RST environment variables. Open and edit your `~/.bashrc` file to include:

        # bash profile for rst
        export RSTPATH="INSTALL LOCATION"/rst
        . $RSTPATH/.profile.bash

   where the INSTALL LOCATION is the path with the RST repository that has been copied to your
   computer.  In order to load the environment variables you just setup, you'll need to close 
   your current terminal and open a new terminal, or from the command line type:
   
       source ~/.bashrc

3. Run `make.build` from the command line.  You may need to change directory to `$RSTPATH/build/script`.
   This runs a helper script that sets up other compiling code.

4. In the same directory run `make.code` to compile all of the code.
   This runs a script to find all of the source codes and compile them into binaries.
   A log of this compilation is stored in `$RSTPATH/log`.

### Compiling HTML Documentation

To compile the html documentation, run `make.doc` from the command line. You may need
to modify the `URLBASE` environment variable in `$RSTPATH/.profile/rst.bash` for the
links in the html pages to function correctly.  Online documentation is available at:

https://superdarn.github.io/rst/index.html

## Troubleshooting

> If you find any problems/solutions, please make a [github issue](https://github.com/superdarn/rst/issues/new) so the community can help you or add it to the documentation

### Without IDL 

**Error**

`make.code` fails with the error upon the inability to locate `idl_export.h`.


**Solution**
	 ```
	 cd $RSTPATH/codebase/superdarn/src.lib/tk
	 tar -P -czvf idl.tar.gz idl
	 rm -rf idl
	 cd $RSTPATH/build/script
	 make.code
	 ```

**Error**
If the order of make.code is executed incorrectly, you will see an error upon
the inability to locate a header file (i.e. `sza.h`).  If this happens (using
`sza.h` as an example):

**Solution**
```
find $RSTPATH -name "sza.h"
>> $RSTPATH/codebase/imagery/src.lib/sza.1.9/include/sza.h
cd $RSTPATH/codebase/imagery/src.lib/sza.1.9/src
make clean
make
cd $RSTPATH/build/script
make.code
```
