<!--
(C) copyright 2019 SuperDARN Canada, University of Saskatchewan
author: Marina Schmidt, SuperDARN Canada
Modified:
        Kevin Sterne, Virginia Tech, 20210728: Addition of Ubuntu 20 dependencies

-->

# Linux 

This guide is intended to provide installation instructions on several **Linux Distributions**. It begins with a list of dependencies required for each distribution and concludes with instructions for installing RST. If you do not see you Linux distribution or version please create an [issue](https://github.com/superdarn/rst/issues/new) on GitHub page and we can help and add it into our installation guide. 

If you run into any problems with installing RST please create an [issue](https://github.com/superdarn/rst/issues/new) addressing your problem and the error message you recieve. The community will then help you with solving your problem and add it into our troubleshooting section to help other future users. 


Table of Contents: 
-------------------

1. [Library Requirements](#library-requirements)
    1. [Debian](#debian)
    2. [Mint](#mint)
    3. [OpenSuse](#opensuse)
    4. [Ubuntu](#ubuntu)
    5. [Fedora](#fedora)
    6. [CentOS](#centos)
    7. [CDF Library](#cdf-library) 
2. [Installation](#installation) 
3. [Troubleshooting](#troubleshooting)

## Library Requirements

!!! Warning
    sudo privileges needed to install the various libraries 
    If you do not have `sudo` privileges please contact the system administrator of your system to install the follow libraries for your distribution.  

### Ubuntu 

   16.04 		 |     18.04   |          20.04         |
 ------------------------|-------------|------------|
 libhdf5-serial-dev	 |  libhdf5-serial-dev | libhdf5-serial-dev  |
 libncurses-dev 	 |  libncurses-dev     | libncurses-dev      |
 libnetcdf-dev 		 |  libnetcdf-dev      | libnetcdf-dev       |
 libpng12-dev 		 |  libpng-dev         | libpng-dev          |
 libx11-dev 		 |  libx11-dev         | libx11-dev          |
 libxext-dev 		 |  libxext-dev        | libxext-dev         |
 netpbm (10.77.03_2+x11) |  netpbm (10.77.03_2+x11) | netpbm (10.77.03_2+x11) |
 &nbsp;                  |  &nbsp;             | build-essential     |

Installation line:
**Ubuntu 16.04**
    
    sudo apt-get install libhdf5-serial-dev libncurses-dev libnetcdf-dev libpng12-dev libx11-dev libxext-dev netpbm


**Ubuntu 18.04**

    sudo apt-get install libhdf5-serial-dev libncurses-dev libnetcdf-dev libpng-dev libx11-dev libxext-dev netpbm

**Ubuntu 20.04**

    sudo apt install libhdf5-serial-dev libncurses-dev libnetcdf-dev libpng-dev libx11-dev libxext-dev netpbm build-essential


Now install the [CDF library](#cdf-library) 


### Debian 

| 8.7 	    	    	|
|-----------------------| 	    	
|gcc 	   		|	
|libhdf5-serial-dev 	| 	
|libnetcdf-dev 		|	
|libncurses	 	|	
|libpng12-dev 		|	
|libx11-dev 		|	
|libxext-dev 		|
|netpbm 		|	 
 			
**Debian 8.7**

    sudo apt-get install gcc libhdf5-serial-dev libnetcdf-dev libncurses libpng12-dev libx11-dev libxext-dev netpbm

Now install the [CDF library](#cdf-library) 

### Mint 

Mint 20.2 	|
--------- 	|
libc6-dev 	|
libncurses5-dev |
libnetcdf-dev 	|
libpng-dev 	|
libx11-dev 	|
libxext-dev 	|
netcdf 		|
netcdf-devel 	|

**Mint 20.2**
    
    apt install libc6-dev libncurses5-dev libnetcdf-dev libpng-dev libx11-dev libxext-dev



Now install the [CDF library](#cdf-library) 

### OpenSuse

 OpenSUSE 42.2/Leap 15.0 |
 ---------------|
 gcc 		        | 
 hdf5-devel     |
 libpng16-devel |
 libX11-devel 	|
 libXext6 	    |
 libXext-devel 	|
 netpbm 	      |
 ncurses-devel 	|
 zlib-devel 	  |

**OpenSUSE 42.2/Leap 15.0**

    sudo zypper install gcc hdf5-devel libpng16-devel libX11-devel libXext6 libXext-devel netcdf netcdf-devel ncurses-devel zlib-devel

Now install the [CDF library](#cdf-library) 

### Fedora

   25 to 29 		         |
 --------------------| 		
 hdf5-devel	         |
 ncurses-dev 	     |
 netcdf              |
 netcdf-dev 		 |
 libpng-devel 		 |
 libx11-devel 		 |
 libxext-devel 		 |
 libext              |
 zlib-devel          |

**Fedora 25 to 29**

    sudo dnf install hdf5-devel libpng-devel libX11-devel libXext libXext-devel netcdf netcdf-devel ncurses-devel zlib-devel

Now install the [CDF library](#cdf-library) 

### CentOS

 6.10                |  7 		         |
 ----------------    |--------------------|
 libhdf5	     |  hdf5-devel	         |
 libnetcdf	     |  ncurses-devel 	     |
 libcurses	     |  netcdf-devel 		 |
 libpng12	     |  netcdf              |
 libX11		     |  libpng12-devel 	 |
 libXest	     |  libx11-devel        |
 netpbm		     |  libxext             |
 zlib		     |  libxext-devel 		 |
 _  		     |  zlib-devel          |

**CentOS 6.10**

    sudo yum install libhdf5 libnetcdf libcurses libpng12 libX11 libXest netpbm zlib

**CentOS 7**

    sudo yum install hdf5-devel libpng12-devel libX11-devel libXext libXext-devel netcdf netcdf-devel ncurses-devel zlib-devel

Now install the [CDF library](#cdf-library) 

### CDF Library 

You will also need the CDF (Common Data Format) library which can be downloaded from NASA. 
!!! Note
    **Make sure you successfully installed the ncurses library for your distribution first.** 
You can find the latest release at: [http://cdf.gsfc.nasa.gov/](http://cdf.gsfc.nasa.gov/)

For Opensuse you will need to make a symlink lcurses library to libncuses (recommended) or modify the CDF Makefile. 

**Example installation for Ubuntu/Mint**

From the above site, navigate to the `linux` directory and download the file `cdf[XX_X]-dist-cdf.tar.gz`, where `XX_X` is the version number of the latest release. 

Unpack and install the package (see README.install for more information):

    tar -xzvf cdf[XX_X]-dist-cdf.tar.gz
    cd cdfXX_X-dist
    make OS=linux ENV=gnu all
    make test
    sudo make INSTALLDIR=/usr/local/cdf install

You can now delete the `cdfXX_X-dist` directory and the `tar.gz` archive.


Now go to the [Installation](#installation)

#### TroubleShooting: 

> If you find any problems/solutions, please make a [github issue](https://github.com/superdarn/rst/issues/new) so the community can help you or add it to the documentation


**Problem**

Error: curses.h not found

**Solution**

* (Recommended) To make a symlink from lncurses library to lcurses; run the following commands in the terminal:

        ln -s /usr/lib64/libncurses.so /usr/lib64/libcurses.so
        ln -s /usr/lib64/libncurses.a /usr/lib64/libcurses.a

* To modify the Makefile change the following:  
`CURSESLIB_linux_gnu=-lcurses` to `CURSESLIB_linux_gnu=-lncurses`  
`CURSESLIB_linux_gnu32=-lcurses` to `CURSESLIB_linux_gnu32=-lncurses`  
`CURSESLIB_linux_gnu64=-lcurses` to `CURSESLIB_linux_gnu64=-lncurses`  


## Installation

1. Obtaining RST software:
    - Download the official release with a citable DOI from [Zenodo](https://doi.org/10.5281/zenodo.801458) (**recommended for most users**)
    - Clone from Github (for developers): ```git clone https://github.com/superdarn/rst/```

2. Check RST environment variables:
   Open `rst/.profile.bash` using your preferred text editor:
	```	
      	OSTYPE="linux"
       	SYSTEM="linux"
	```

   Open `rst/.profile/base.bash` to check paths are correctly set:

   `XPATH, NETCDF_PATH, CDF_PATH` 
   To check if the paths are set correctly locate the following header files:
   For NETCDF_PATH `locate netcdf.h`
   For CDF PATH `locate cdf.h`
   
   - If you have **IDL**, check to see that `IDL_IPATH` in `rst/.profile/idl.bash` is correct.
   	(Note: for users without access to IDL, modifying the `IDL_IPATH` environment variable is
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


