RST Build Management Scripts
============================

The RST build managment scripts are used to manage the RST software archive. They consist of shell scripts for automating the compilation of software and generating the documentation. 
The build scripts can be catagorized according to purpose; "Compilation", "Documentation", and "Basic functions"


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

Basic Functions:

get.version

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

make.code type [pattern]

Description:

Compiles source code in an RST package according to the package build rules. The script compiles the software in the package according to the ordering rules given by the package "build.txt" file. A time-stamped log is generated and stored in the log directory.

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

make.doc [-w]

Description:

Make the documentation for a package. If the "-w" option is given, then the documuntation is created for the web; otherwise a local version is created.

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

make.doc.rfc [-w] codebase project

Description:

Make the Request For Comment (RFC) documents for a project. The project name is given by "project". If the "-w" option is given, then the documuntation is created for the web; otherwise a local version is created.

make.doc.tutorial
-----------------

Syntax:

make.doc.tutorial [-w] fname

Description:

Make a tutorial. The XML tutorial file is given by "fname". If the "-w" option is given, then the documuntation is created for the web; otherwise a local version is created.

Basic Function
==============

get.version
-----------

Syntax:

get.version [path]

Description:

Get a version number. The version number is taken from either the appropriate version file or from the respository tags found in the directory "path". The version is written to standard output

