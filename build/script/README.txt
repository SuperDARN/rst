RST Build Management Scripts
============================

The RST build managment scripts are used to manage the RST software archive. They consist of shell scripts for automating the compilation of software, managing the revision control system, generating the documentation and for creating and managing software packages. 
The build scripts can be catagorized according to purpose; "Compilation", "Documentation", "Package Management", and "Basic functions"


Compilations:

make.build
make.code
make.hdr
make.help
make.version

Documentation:

make.doc
make.doc.bin
make.doc.frame
make.doc.lib
make.doc.rfc
make.doc.tutorial

Package Managment:

make.id
assemble.pkg
compile.pkg
make.rst
setup.pkg
tag.version
update.pkg

Basic Functions:

get.version
pkg.git
pkg.id
pkg.version
rpkg.se
status.pkg

Compilation
===========

make.build
----------

Syntax:

make.build

Description:

Compiles the RST core build software used to compile the rest of the RST source code. The core build software includes the compilation build software and the XML processing software used to generate the documentation.

make.code
---------

Syntax:

make.code project package type [pattern]

Description:

Compiles source code in an RST package according to the package build rules. The project and the associated package are given by "project" and "package" respectively. The script compiles the software in the package acording to the ordering rules given by the package "build.txt" file. A time-stamped log is generated an stored in the log directory.
The "type" option will compile only code of a given type, either "bin" or "lib". The optional "pattern" can be used to compile only a sub-set of code whose module pathnames contain the given string. 

make.hdr
--------

Syntax:

make.hdr source destination

Description:

Creates symbolic links for library header files in the appropriate sub-directory of the header directory "include". The header files are searched for in the directory "source" and the symbolic links are created in "destination".

make.help
---------

Syntax:

make.help 

Description:

Create the help and error message headers for a software binary using the associated XML documentation file. The documenation file is searched for in the current directory and the two headers; "errstr.h" and "hlpstr.h" are written to the current directory.

make.version
------------

Syntax:

make.version [path]

Description:

Create the version header. The version number is taken from either the appropriate version file or from the respository tags found in the directory "path". The header is written to the current directory.

Documentation
=============

make.doc
--------

Syntax:

make.doc [-w] project package

Description:

Make the documentation for a package. The project and package name are given by "project" and "package" respectively. If the "-w" option is given, then the documuntation is created for the web; otherwise a local version is created.

make.doc.bin
------------

Syntax:

make.doc.bin [-w] fname

Description:

Make the documentation for a software binary. The XML documentation file is given by "fname". If the "-w" option is given, then the documuntation is created for the web; otherwise a local version is created.

make.doc.frame
--------------

Syntax:

make.doc.frame [-w]

Description:

Make the framework of the documentation. If the "-w" option is given, then the documuntation is created for the web; otherwise a local version is created.
The script creates the supporting pages for navigating the documentation, including the contents, and index pages.

make.doc.lib
------------

Syntax:

make.doc.lib [-w]

Description:


Make the documentation for a software library. The XML documentation file is given by "fname". If the "-w" option is given, then the documuntation is created for the web; otherwise a local version is created.

make.doc.rfc
------------

Syntax:

make.doc.rfc [-w] project

Description:

Make the Request For Comment (RFC) documents for a project. The project name is given by "project". If the "-w" option is given, then the documuntation is created for the web; otherwise a local version is created.

make.doc.tutorial
-----------------

Syntax:

make.doc.tutorial [-w] fname

Description:

Make a tutorial. The XML tutorial file is given by "fname". If the "-w" option is given, then the documuntation is created for the web; otherwise a local version is created.

Package Management
==================

make.id
-------

Syntax:

make.id project package

Description:

Generate a list of commit identifiers for each element in a package. The project and package name are given by "project" and "package" respectively. The module pathname and the identfier of the most recent commit are written to the package "id.txt" file. 
The "id.txt" file is used to identify the code that should be checked out of the software repositories to create a particular version of a package. This file must be updated whenever a new version of a package is created. 

assemble.pkg
------------

Syntax:

assemble.pkg project package

Description:

Prepare a package directory. The project and package name are given by "project" and "package" respectively. A master package contains the most up to date version of the software and includes the associated software repository. The software is checked out of the repository and assembled in a directory.


compile.pkg
-----------

Syntax:

compile.pkg packagedir system

Description:

Create a self-extracting distribution package from a package directory. The script compiles the code and then assembles the self-extracting archive.

setup.pkg
----------

Syntax:

setup.pkg project package

Description:

Initialize a software package repository. The project and package name are given by "project" and "package" respectively.
The script initialises a package repository bases on the framework directory and assigns a version number of "1.1"

tag.version
-----------

Syntax:

tag.version [git-options]
tag.version -init [git-options]
tag.version -major [git-options]

Description:

Update a version tag for a repository. By default, the script finds the existing tag version number and monotonically increases the minor version number. A new tag of the form "version.X.YY" is created and the remaining command line options given by "git-options" are passed to the "git tag" command.
If the "-init" command line flag is given, the version number will be initialized to "1.1". If the repository already has a version number, then this option will fail.
If the "-major" command line flag is given, the major version number is incremented and the minor version number reset to "1".

update.pkg
----------

Syntax:

update.pkg project package [git-options]

Description:

Update the contents of a software package repository. The project and package name are given by "project" and "package" respectively.
The script recreates the package "id.txt" file to include the most recent repository commits and commits the updated "id.txt" to the package repository.

Basic Function
==============

get.version
-----------

Syntax:

get.version [path]

Description:

Get a version number. The version number is taken from either the appropriate version file or from the respository tags found in the directory "path". The version is written to standard output

pkg.git
-------

Syntax:

pkg.git project package [git-options]

Description:

Apply a git command to the git repository associated with a software package. The project and package name are given by "project" and "package" respectively. The remaining command line options, as defined by "git-options" are passed onto git directly.

pkg.id
------

Syntax:

pkg.id project package

Description:

Generate a list of commit identifiers for each element in a package. The project and package name are given by "project" and "package" respectively. The module pathname and the identfier of the most recent commit are written to standard output.

pkg.version
-----------

Syntax:

pkg.version project package

Description:

Generate a list of version numbers for each element in a package. The project and package name are given by "project" and "package" respectively. The module pathname and the most recent version tag are written to standard output.

rpkg.se
-------

Syntax:

rpkg.se

Description:

Self-extracting header script. The package archive is appended to this script to create the full self-extracting archive.

status.pkg
----------

Syntax:

status.pkg project package

Description:

Report the commit status for each element in a package. The project and package name are given by "project" and "package" respectively.
This script shows the statu for each element repository in a package.



Packaging sequence
------------------


If any changes have been made to the contents of a package, update "module.txt" to add or remove components. Then edit "build.txt" to make sure that the build order is complete. 
Update the package id file:

update.pkg <project> <package>

Edit any README or LICENSE files as needed, then commit changes in the package directory. Then use "git tag -a" to tag the version number of the package.

The create the package directory:

assemble.pkg <project> <package>

Move this to the deployment platform and then assemble the package with 

compile.pkg <packagedir> <system>


