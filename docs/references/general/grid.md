<!-- Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
author(s): Marina Schmidt 

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->
# GRID files 

GRID files are post-processed data produced from FITACF files.

## Naming Conventions

### Single radar GRID files

Currently the common naming convention for a single radar GRID file is:

> YYYYMMDD.HH.mm.ss.<3-letter abbreviation>.grid

Some radars provide separate data files for each channel. In this case, the channel is specified after the 3-letter station ID:

> YYYYMMDD.HH.mm.ss.<3-letter abbreviation>.[a-d].grid

See [fitacf](fitacf.md) documentation for more information on this naming convention. 

If a GRID file is produced for 24-hours using the `-c` option in `make_grid` (see [`make_grid` tutorial](../../user_guide/make_grid.md)) then the common naming convention is:

> YYYYYMMDD.<3-letter abbreviation>.grid

### Combined "GRD" files for each hemisphere

After GRID files are produced for separate radars, they may then be combined with other radars' GRID files to make a GRD file. This file has the same format as a GRID file, but includes data from multiple radars. 
See [combine_grid](../../user_guide/make_grid.md) on how to make GRD files. 

Combined GRD files are produced for the northern and southern hemispheres separately and usually contain 24 hours of data. One naming convention for 24-hr GRD files could be:

> YYYYMMDD.north.grd  
> YYYYMMDD.south.grd  

## Fields

GRID files contain a record that contains scalar and vector fields. 

### Scalars

| Field name       | Units      | Data Type    | Description                                                |
| :----------      | :-----:    | :-------:    | :---                                                       |
| *start.year*  | **None**   | ***short***  | Start Year of the integration period                       |
| *start.month*  | **None**   | ***short***  | Start Month number (not padded)  of the integration period |
| *start.day*  | **None**   | ***short***  | Start Day (not padded) of the integration period           |
| *start.hour*  | *hrs*      | ***short***  | Start Hour of the integration period                       |
| *start.minute*  | *min*      | ***short***  | Start Minute of the integration period                     |
| *start.second*  | *s*        | ***short***  | Start Seconds of the integration period                    |
| *end.year*    | **None**   | ***short***  | End Year of the integration period                         |
| *end.month*    | **None**   | ***short***  | End Month number (not padded) of the integration period    |
| *end.day*    | **None**   | ***short***  | End Day (not padded) of the integration period             |
| *end.hour*    | *hrs*      | ***short***  | End Hour of the integration period                         |
| *end.minute*    | *min*      | ***short***  | End Minute of the integration period                       |
| *end.second*    | *s*        | ***short***  | End Seconds of the integration period                      |


### Vectors 

!!! Note
    Let the number of radars in a given GRID/GRD record be defined as *numstid* and
    Let the number of gridded velocity vectors in a given record be defined as *numv*. 

| Field name  | Units           | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:         | :-------:      | :---:       | :---                                                                        |
| *stid*      |  **None**       |  *[numstid]*     | ***short*** | A list of of numeric station IDs that provided data for the record |
| *channel*   |  **None**       |  *[numstid]*     | ***short*** | A list of channel numbers associated to the station id the record |
| *nvec*      | **None** | *[numstid]*  | ***short*** | Number of velocity vectors for each station|
| *freq*      | *kHz* | *[numstid]* | ***float*** | Transmitted frequency for each radar |
| *major.revision* | **None** | *[numstid]* | ***short*** | Major `make_grid` version number per station number |
| *minor.revision* | **None** | *[numstid]* | ***short*** | Minor `make_grid` version number per station number |
| *program.id*     | **None** | *[numstid]* | ***short*** | Control program ID per station number 
| *noise.mean*      | **None** | *[numstid]* | ***float*** | Mean noise for each radar |
| *noise.sd*        | **None** | *[numstid]* | ***float*** | Noise Standard deviation per station number |
| *gsct*            | **None** | *[numstid]* | ***short*** | A list of groundscatter flags associated to each radars data for the given record |
| *v.min*           | *m/s* | *[numstid]* | ***float*** | Minimum velocity threshold |
| *v.max*           | *m/s* | *[numstid]* | ***float*** | Velocity maximum threshold per station number |
| *p.min*           | **None** | *[numstid]* | ***float*** | Power minimum threshold per station number |
| *p.max*           | **None** | *[numstid]* | ***float*** | Power maximum threshold per station number |
| *w.min*           | *m/s*  | *[numstid]* | ***float*** | Spectral width minimum threshold per station number |
| *w.max*           | *m/s*  | *[numstid]* | ***float*** | Spectral width maximum threshold per station number |
| *ve.min*          | *m/s*  | *[numstid]* | ***float*** | Velocity error minimum threshold per station number |
| *ve.max*          | *m/s*  | *[numstid]* | ***float*** | Velocity error maximum threshold per station number |
| *vector.mlat*     | *degrees* | *[numv]* | ***float*** | Magnetic Latitude per vector point |
| *vector.mlon*     | *degrees* | *[numv]* | ***float*** | Magnetic Longitude  per vector point |
| *vector.kvec*     | *degrees*  | *[numv]*   | ***flost*** | Magnetic Azimuth per vector point |
| *vector.stid*     | **None**  | *[numv]*   | ***short*** | Station identifier per vector point|
| *vector.channel*  | **None**  | *[numv]*   | ***short*** | Channel number per vector point |  
| *vector.index*    | **None**  | *[numv]*   | ***int***   | Grid indices|
| *vector.vel.median* | *m/s* | *[numv]*   | ***float*** | Velocity medians per vector point|
| *vector.vel.sd*     | *m/s*   | *[numv]*   | ***float*** | Velocity standard deviation per vector point|
| *vector.pwr.median* | *dB*  | *[numv]*   | ***float*** | Power median per vector point|
| *vector.pwr.sd*     | *dB*  | *[numv]*   | ***float*** | Power standard deviation per vector point|
| *vector.wdt.median* | *m/s* | *[numv]*   | ***float*** | Spectral Width median per vector point|
| *vector.wdt.sd      | *m/s* | *[numv]*   | ***float*** | Spectral Width median per vector point|

## File structure

GRID files typically contain up to 24 hours of data. Individual records in a GRID file contain a record for each integration time period (default 120 seconds) from one or more radars.

Partial records may occur in GRID and GRD file. See [fitacf](fittacf.md) for more information. 
