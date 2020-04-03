<!-- Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
author(s): Marina Schmidt 

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->
# GRID files 

FITACF files are a fitted post-processed data product from RAWACF files (sometimes dat files).

## Naming Conventions

### Single GRID files

Currently the common naming convention for a GRID file is:

> YYYYMMDD.HH.mm.ss.<3-letter abbreviation>.grid

SuperDARN radars routinely change operating frequency. Some operating modes use multiple frequencies, either at the same time, or by alternating between frequencies. As a result, some files from some radars include all records for all frequencies in the same file and other radars separate out the individual frequencies into separate files, designated by a channel letter in the file name:

> YYYYMMDD.HH.mm.ss.<3-letter abbreviation>.[a-d].grid

See [fitacf](fitacf.md) documentation for more information on this naming convention. 

If a GRID file is produced for 24-hours using the `-c` option in `make_grid` (see [`make_grid` tutorial]()) then the common naming convention is:

> YYYYYMMDD.<3-letter abbreviation>.grid

### Combined GRID files 

After GRID files are produced for seperate radars, they are sometimes further combined with other radars GRID files to meka a GRD file. Similar to GRID file just with multiple radar data included. 
See [combine_grid](docs/user_guide/) on how to make GRD files. 

The commone naming convetion for 24-hr GRD files are:

> YYYYMMDD.grd  

## Fields

GRID files contain a record that contains scalar and vector fields. 

### Scalars

| Field name       | Units      | Data Type    | Description                                                |
| :----------      | :-----:    | :-------:    | :---                                                       |
| *start.time.yr*  | **None**   | ***short***  | Start Year of the integration period                       |
| *start.time.mo*  | **None**   | ***short***  | Start Month number (not padded)  of the integration period |
| *start.time.dy*  | **None**   | ***short***  | Start Day (not padded) of the integration period           |
| *start.time.hr*  | *hrs*      | ***short***  | Start Hour of the integration period                       |
| *start.time.mt*  | *min*      | ***short***  | Start Minute of the integration period                     |
| *start.time.sc*  | *s*        | ***short***  | Start Seconds of the integration period                    |
| *end.time.yr*    | **None**   | ***short***  | End Year of the integration period                         |
| *end.time.mo*    | **None**   | ***short***  | End Month number (not padded) of the integration period    |
| *end.time.dy*    | **None**   | ***short***  | End Day (not padded) of the integration period             |
| *end.time.hr*    | *hrs*      | ***short***  | End Hour of the integration period                         |
| *end.time.mt*    | *min*      | ***short***  | End Minute of the integration period                       |
| *end.time.sc*    | *s*        | ***short***  | End Seconds of the integration period                      |


### Vectors 

!!! Note
    Let the number of radars in a given GRID/GRD record be defined as *numstid* and
    Let the number of vector points in a give GRRID/GRD record be defined as *numv*. 

| Field name  | Units           | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:         | :-------:      | :---:       | :---                                                                        |
| *stid*      |  **None**       |  *[numstid]*     | ***short*** | A list of of station id's that provided data for the record |
| *channel*   |  **None**       |  *[numstid]*     | ***short*** | A list of channel numbers associated to the station id the record |
| *nvec*      | **None** | *[numstid]*  | ***short*** | |
| *freq*      | **kHz** | *[numstid]* | ***float*** | Frequency each radar was running for that record |
| *major.revisions* | **None** | *[numstid]* | ***short*** | |
| *minor.revisions* | **None** | *[numstid]* | ***short*** | |
| *proogram.id*     | **None** | *[numstid]* | ***short*** | |
| *noise.mean*      | **None** | *[numstid]* | ***float*** | |
| *noise.sd*        | **None** | *[numstid]* | ***float*** | |
| *gsct*            | **None** | *[numstid]* | ***short*** | A list of groundscatter flags associated to each radars data for the given record |
| *v.min*           | **None** | *[numstid]* | ***float*** | |
| *v.max*           | **None** | *[numstid]* | ***float*** | |
| *p.min*           | **None** | *[numstid]* | ***float*** | |
| *p.max*           | **None** | *[numstid]* | ***float*** | |
| *w.min*           | **None** | *[numstid]* | ***float*** | |

## File structure

GRID files contain typically 24 hours of data. Individual records in a GRID file contain a record for each integration time peeriod (default 120 seconds). 

Partial records may occur in GRID and GRD file. See [fitacf](fittacf.md) for more information. 
