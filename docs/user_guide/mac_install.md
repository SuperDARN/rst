# Mac 

This guide is intended to provide installation instructions on **macOS** for Apple-branded desktops and laptops. It begins with a list of dependencies required with instructions for installing RST. This process was designed for OS **Sierra** and above, but should still work on any machine which can run XCode. If you encounter any problems with installing RST, please create an [issue](https://github.com/superdarn/rst/issues/new) describing your problem and the error message you receive. The community will then help you solve the problem and add it into our troubleshooting section to help other users. 

## Library Requirements

> Warning! sudo privileges needed to install the various libraries. 

If you do not have `sudo` privileges please contact the system administrator of your system to install the follow libraries for your distribution.  

We will be installing the dependencies using a Mac compatible package manager, such as Macports or Homebrew. Make sure you have one of those installed.

Note that the names of the following dependencies have been known to change slightly depending on the date and the particular package manager used. A quick google search can often tell you the new name if it has changed - just replace the name in the install commands if it has changed.

[Macports install guide](https://www.macports.org/install.php)

### Homebrew Install (Recommended)

#### For Intel-based (x86, i3, i5, i7, etc)
If you are on an Intel-based (x86) Mac device, you can install homebrew with the following terminal command:
###
	/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

#### For ARM-based Macs (M1 or above)
If you are on an ARM-based Mac device (M1 or above), you must install the x86 version of Homebrew in your `/usr/local` directory. First, if you do not already have installed, install Rosetta (Apple x86 compatibility layer) by running the following in a terminal window:
###
	softwareupdate --install-rosetta

After Rosetta is installed, close all active terminal windows, right-click "get info" on the terminal application (in the /Applications/ directory) and check "Open using Rosetta". After you have finished installing RST, you may safely uncheck this box (RST will continue to work in a non-Rosetta window).

Install the x86 version of Homebrew in a Rosetta-enabled window:
###
	/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

## Installing pre-requesites
### Macports
    
    sudo port install libhdf5 libnetcdf libcurses libpng16 libx11 netpbm (10.77.03_2+x11)

### Homebrew
    
    sudo brew install hdf5 netcdf ncurses libpng netpbm

### ARM-based Macs

You specifically need the x86 version of `netcdf`. Sometimes, Homebrew may try to install the ARM-based version if you have two versions of Homebrew installed (ARM in `/opt/homebrew`, x86 in `/usr/local`), even if you're in a Rosetta window. To force `netcdf` to install an x86 version in the correct place, run:
###
	/usr/local/bin/brew install netcdf

## XCode

All Macs need XCode to get X11 and run RST properly. You can download this normally from the [Mac App Store](https://apps.apple.com/ca/app/xcode/id497799835?mt=12). Nothing different is required for x86 or ARM Macs.

## CDF 

> Note
    **Make sure you successfully installed the ncurses library for your distribution first.** 
	
You can find the latest release at: [http://cdf.gsfc.nasa.gov/](https://spdf.gsfc.nasa.gov/pub/software/cdf/dist/latest/macosx/)

For macOS, it is also available through MacPorts, as are all listed dependencies. However, if the MacPorts installation errors or you do not use MacPorts, you may follow the instructions below.

From the above site, navigate to the `/cdf/dist/latest/macosx` directory and download the pkg file `CDFX_X_X-binary_signed.pkg`, where `X_X_X` is the version number of the latest release. Double click the file where you downloaded it and install it like a normal Mac package.

Now go to the [Installation](#installation)

## TroubleShooting: 

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
   To change the paths to the locations of appropriate header files:
   - For XPATH, this is the location of X11, which is included with XCode. It should be `"opt/X11/"` for ARM-based Macs and `"usr/X11/"` for x86 Macs. The `/include` folder in here contains .h files (headers) that RST requires, like `png.h`.
     
   - For NETCDF_PATH, point this to where you installed netcdf. This should be `"/usr/local/Cellar/netcdf/X.X.X_X"` if you installed it with Homebrew for both types of Macs, where the X's are the version number.
     
   - For CDF_PATH, this will be where the CDF package is installed. It should be something like `"/Applications/cdf/cdf"`. The directory pointed to should contain the CDF `ReadMe.txt`.
   
   - If you have **IDL**, check to see that `IDL_IPATH` in `rst/.profile/idl.bash` is correct.
   	(Note: for users without IDL, modifying the `IDL_IPATH` environment variable is
   	not required).

2. Load the RST environment variables. Open and edit your `~/.bashrc` (or `~/.zshrc` if you're on an ARM file, or have switched from bash to zsh) to include:

        # bash profile for rst
        export RSTPATH="INSTALL LOCATION"/rst
        . $RSTPATH/.profile.bash

   where the INSTALL LOCATION is the path with the RST repository that has been copied to your
   computer.  To load the environment variables you just set up, you'll need to close 
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
