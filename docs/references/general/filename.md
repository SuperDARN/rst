<!-- Copyright (C) 2020 VT SuperDARN, Virginia Polytechnic Institute and State University 
author(s): Kevin Sterne

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->

This information is sourced from the RFC: 0002 previously in the RST RFC documentation that was written by R.J. Barnes.  For the filenaming convention, it is expected the Data Standards Working Group documentation will make information presented here obsolete.

# Filename Format Definition

## Summary

This is the format for SuperDARN filenames so that the user can immediately identify start time of the data contained within a file.  This format originated in in 2004 and was applied to dmap format files that became standard on 7/1/2006.

## Introduction

The original specification (from 1990s) for SuperDARN filenames was as follows:

```
yymmddhhi[c].ttt
yy  | Two digit year (83-99)
mm  | Two digit month of year (01-12)
dd  | Two digit day of month (01-31)
hh  | Two digit hour of day (00-23)
i   | Station identifier (eg. g, k, t, w, etc.)
c   | An optional single character suffix appended if more than one file was opened during a given hour (A-Z)
ttt | A three letter filetype (fit, inx, dat, smr, etc.)
```

In 1999, the two digit year specification was replaced by a full four digit year to deal with the millenium.  A typical set of filenames would look like:

```
2001012900k.fit
2001012900kA.fit
2001012902k.fit
2002111412g.dat
```

Due to the increase in the number of radars, the single letter radar indentifier can no longer be used.  Consequently, the file format must be changed and this opportunity was used to make the filename format easier to understand.
