<!-- Copyright (C) 2020 VT SuperDARN, Virginia Polytechnic Institute and State University 
author(s): Kevin Sterne

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->

This information is sourced from the RFC: 0002 previously in the RST RFC documentation that was written by R.J. Barnes.

# Filename Format Definition

## Summary

This is the format for SuperDARN filenames so that the user can immediately identify start time of the data contained within a file.  This format originated in in 2004 and was applied to dmap format files that became standard on 7/1/2006.

## Introduction

The original specification (from 1990s) for SuperDARN filenames was as follows:

```
*yy*mmddhhi[c].ttt*
*yy*  | Two digit year (83-99)
*mm*  | Two digit month of year (01-12)
*dd*  | Two digit day of month (01-31)
*hh*  | Two digit hour of day (00-23)
*i*   | Station identifier (eg. g, k, t, w, etc.)
*c*   | An optional single character suffix appended if more than one file was opened during a given hour (A-Z)
*ttt* | A three letter filetype (fit, inx, dat, smr, etc.)
```

In 1999, the two digit year specification was replaced by a full four digit year to deal with the millennium:
```
*yyyymmddhhi[c].ttt*
```

A typical set of filenames would look like:
```
2001012900k.fit
2001012900kA.fit
2001012902k.fit
2002111412g.dat
```

Due to the increase in the number of radars, the single letter radar identifier can no longer be used.  Consequently, the file format must be changed and this opportunity was used to make the filename format easier to understand.

## "Proposed" Filename Format
(Proposed as this appears to have been widely adapted in post 7/1/2006)

The filename format proposed is as follows:

```
*yyyymmdd.hhMM.ss.iii.ttt[ttt]*
*yyyy*    | Four digit year (eg 2004)
*mm*      | Two digit month of year (01-12)
*dd*      | Two digit day of month (01-31)
*hh*      | Two digit hour of day (00-23)
*MM*      | Two digit minute of hour (00-59)
*ss*      | Two digit second of minute (00-59)
*iii*     | Radar identifier string (eg. kap, sas)
*ttt*     | Filename type (eg. raw, fit, fitacf, rawacf)
```

A typical set of filenames in the new format would look like:
```
20010129.0000.00.kap.fitacf
20010129.0021.07.kap.fitacf
20010129.0200.00.kap.fitacf
20021114.1200.00.gbr.rawacf
```

### Concatenated Files

Often the user will need to concatenate together multiple files when working on longer periods of data.  The RST software allow the user to pick any arbitrary name for the concatenated files, but for convenience it is recommended that the following format is used:
```
*yyyymmdd.hhMM.ss.iii.ttt[ttt].C*
*yyyy*    | Four digit year (eg 2004)
*mm*      | Two digit month of year (01-12)
*dd*      | Two digit day of month (01-31)
*hh*      | Two digit hour of day (00-23)
*MM*      | Two digit minute of hour (00-59)
*ss*      | Two digit second of minute (00-59)
*LL*      | Length in hours of the interval of data
*iii*     | Radar identifier string (eg. kap, sas)
*ttt*     | Filename type (eg. raw, fit, fitacf, rawacf)
```

### Daily Files

For convenience, a special shortened version of a filename can be used for files containing an entire day of data. This short hand version is much easier to type:
```
*yyyymmdd.iii.ttt[ttt]*
*yyyy*    | Four digit year (eg 2004)
*mm*      | Two digit month of year (01-12)
*dd*      | Two digit day of month (01-31)
*iii*     | Radar identifier string (eg. kap, sas)
*ttt*     | Filename type (eg. raw, fit, fitacf, rawacf)
```

A typical set of filenames in this format would look like:
```
20010129.kap.grd
20010129.kap.grd
20010129.kap.grd
20021114.gbr.grd
```

### Global and Hemispheric Files

In some cases a SuperDARN data files contains data from the entire array or from one hemisphere.  in this case, the station identifiers suffix is omitted.  For files containing either one hemisphere or another this should be indicated by including the additional suffix "N", "north", "S", or "south" as shown below.  Either the full filename specification or the shortened daily file format can be used as appropriate:
```
*yyyymmdd[.hhMM.ss][.LL].[HHHH].ttt[ttt]*
*yyyy*    | Four digit year (eg 2004)
*mm*      | Two digit month of year (01-12)
*dd*      | Two digit day of month (01-31)
*hh*      | Two digit hour of day (00-23)
*MM*      | Two digit minute of hour (00-59)
*ss*      | Two digit second of minute (00-59)
*LL*      | Length in hours of the interval of data
*HHHH*    | Hemisphere type (eg. north, N, south, or S)
*ttt*     | Filename type (eg. grd, map)
```

A typical set of filenames in this format would look like:
```
20010129.grd
20010129.0030.00.map
20010129.0030.00.12.map
20010129.N.map
20010129.north.map
20010129.S.map
```

## References

-[Specification for Radar Identifier](radar_id.md)

## History (from RFC)

-2004/06/01 Initial Revision, RJB
-2004/06/08 Extended definitions to deal with whole day, hemispheric or global files
