<!-- Copyright (C) 2020 VT SuperDARN, Virginia Polytechnic Institute and State University 
author(s): Kevin Sterne

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->

This information is sourced from the RFC: 0006 previously in the RST RFC documentation that was written by R.J. Barnes.

# DataMap Self-Describing Format

## Summary

A description of the DataMap self-describing format

## Introduction

The DataMap format was developed as a self-describing format to replace the existing SuperDARN binary formats.  Althought many self-describing formats already existed, none
was suitable for use in the operational environment of the radar sites.  The format was designed to be the simplest possible implementation of a self-describing format that
placed the minimum number of restrictions on the user and developer.  Although DataMap was originally developed as a simple file format, the same encoding method can also be 
applied to any stream of data.  In fact, the DataMap format is currently used to encode the real-time data stream from the radar sites.

## The Format

DataMap files or streams are comprised of blocks or records.  Each block represents a single packet of information.  A block is comprised of two types of variables that 
store the data, scalars and arrays.  A scalar variable contains a signle discrete value while an array variable contains multiple values with multiple dimensions.  There is 
no restriction on the contents of a block, and different blocks can contain different scalars and arrays, although in most cases the same scalars and arrays will appear 
in each block.

### Block Format

A block is comprised of a header followed by the scalar and array variables.  The block header consists of an encoding identifier and the total block size.  The encoding 
identifier is a unique 32-bit integer that indicates how the block was constructed.  This value is used to differentiate between possible future changes to the DataMap 
format, currently only one encoding exists.  The second part of the header is the block size, also stored as a 32-bit integer; this size represents the total size of the 
block including both the header and the subsequent data elements.





## References

None

## History (from RFC)

2004/06/22 Iniital Revision, RJB

