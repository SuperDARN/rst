<!-- Copyright (C) 2020 VT SuperDARN, Virginia Polytechnic Institute and State University 
author(s): Kevin Sterne

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->

This information is sourced from the RFC: 0005 previously in the RST RFC documentation that was written by R.J. Barnes.

# Radar Identification Table

## Summary

Format of the radar identification table (radar.dat)

## Introduction

The Radar Identification Table is a  plain ASCII text file that lists the station identifiers for each radar in the network.  The file is usually called "radar.dat" and is located in the superdarn sub-directory of the data tables.

Originally this file was just a simple table listing the station number and its associated identifier letter.  Over the years more information has been added to the table.

## Format for the Radar Identification Table

Lines beginning with the '#' character are treated as comments and ignored.  All other lines represent an entry for a single radar.

The format of each line is as follows:

```
num status "name" "operator" "hardware" "id" ...
| *num*    | Radar station number (site id) |
| *status* | Status of radar: 0 under construction, 1 operational, -1 disused |
| *"name"* | Full name of the radar site |
| *"operator"* | Major operator of the facility |
| *"hardware"* | Name of the hardware configuration file |
| *"id" ...*   | One or more text strings used to identify the radar |
```

An entry in the table looks like this:
```
1 1 "Goose Bay" "Virginia Tech" "hdw.dat.gbr" "gbr" "g"
1 3 "Kapuskasing" "Virginia Tech" "hdw.dat.kap" "kap" "k"
```


## References

- [Radar Identifier](radar_id.md)

## History (from RFC)

- 2004/06/14 Initial Revision, RJB

