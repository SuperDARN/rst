# Linux 

## Library Requirements

### CDF (Common Data Format) Library 

You will also need the CDF (Common Data Format) library which can be downloaded from NASA.
You can find the latest release at: http://cdf.gsfc.nasa.gov/  
For macOS it is also available through macports, as are all listed dependencies  
For Opensuse you will need to make a symlink lcurses library to libncuses (recommended) or modify the CDF Makefile. 
* (Recommended) To make a symlink from lncurses library to lcurses; run the following commands in the terminal:

        ln -s /usr/lib64/libncurses.so /usr/lib64/libcurses.so
        ln -s /usr/lib64/libncurses.a /usr/lib64/libcurses.a

* To modify the Makefile change the following:  
`CURSESLIB_linux_gnu=-lcurses` to `CURSESLIB_linux_gnu=-lncurses`  
`CURSESLIB_linux_gnu32=-lcurses` to `CURSESLIB_linux_gnu32=-lncurses`  
`CURSESLIB_linux_gnu64=-lcurses` to `CURSESLIB_linux_gnu64=-lncurses`  

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
 			
To install the above libraries on a Debian distribution use ``. 
For example:
```Bash

```

### Mint 

Mint 18.1 	|
--------- 	|
libc6-dev 	|
libncurses5-dev |
libnetcdf-dev 	|
libpng16-dev 	|
libx11-dev 	|
libxext-dev 	|
netcdf 		|
netcdf-devel 	|

To install the above libraries on a Debian distribution use ``. 
For example:
```Bash

```

### OpenSuse

 OpenSUSE 42.2 	| Leap 15.0 	|
 ------------- 	|-----------	| 
 gcc 		|		| 
 hdf5-devel 	|	    	|
 libpng16-devel |		|
 libX11-devel 	|		|
 libXext6 	|		|
 libXext-devel 	|		|
 netpbm 	|		|
 		|		|
 ncurses-devel 	|
 zlib-devel 	|

To install the above libraries on a OpenSuse distribution use `sudo zypper`. 
For example:
```Bash
sudo zypper install gcc
```

### Ubuntu 

   16.04 		 |     18.04   |
 ------------------------|-------------| 		
 libhdf5-serial-dev	 |  libhdf5-serial-dev	 | 
 libncurses-dev 	 |  libncurses-dev 	 |
 libnetcdf-dev 		 |  libnetcdf-dev 		 |
 libpng12-dev 		 |  libpng-dev 		 |
 libx11-dev 		 |  libx11-dev 		 |
 libxext-dev 		 |  libxext-dev 		 |
 netpbm (10.77.03_2+x11) |  netpbm (10.77.03_2+x11) |

To install the above libraries on a OpenSuse distribution use `sudo zypper`. 
For example:
```Bash
sudo apt install gcc
```

## Installation Steps

1. Obtaining RST software:
	a) via **GitHub**: 
	```Bash
	git clone https://github.com/superdarn/rst/
	```
	b) via **Download**: [RST zip]()

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

### Compiling Old Documentation
**Warning documentation may be outdated**

To compile the html documentation, run `make.doc` from the command line. You may need
to modify the `URLBASE` environment variable in `$RSTPATH/.profile/rst.bash` for the
links in the html pages to function correctly.  Online documentation is available at:

https://superdarn.github.io/rst/index.html

### Troouble Shooting

#### Without IDL 

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


