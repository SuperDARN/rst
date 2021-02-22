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

If a GRID file is produced for 24-hours using the `-c` option in `make_grid` (see [`make_grid` tutorial](../../user_guide/make_grid.md)) or by combining individual GRID files with `combine_grid`, then the common naming convention is:

> YYYYYMMDD.<3-letter abbreviation>.grid

### Combined GRID files for each hemisphere

After GRID files are produced for separate radars, they may then be combined with other radars' GRID files for eventual processing into convection map files.
See [combine_grid](../../user_guide/make_grid.md) for instructions on how to combine multiple GRID files.

For further Map Potential processing, separate GRID files should be produced for the Northern and Southern Hemispheres. One naming convention for 24-hr GRID files could be:

> YYYYMMDD.north.grid  
> YYYYMMDD.south.grid  

## Fields

GRID files contain a record that contains scalar and vector fields. 

### Scalars

The following times refer to the start and end of the integration period.


| Field name       | Units      | Data Type    | Description                                  |
| :----------      | :-----:    | :-------:    | :---                                           |
| *start.year*     | **None**   | ***short***  | Start Year |
| *start.month*    | **None**   | ***short***  | Start Month number (not padded)                            |
| *start.day*      | **None**   | ***short***  | Start Day (not padded)  |
| *start.hour*     | *hrs*      | ***short***  | Start Hour   |
| *start.minute*   | *min*      | ***short***  | Start Minute |
| *start.second*   | *s*        | ***short***  | Start Seconds  |
| *end.year*       | **None**   | ***short***  | End Year |
| *end.month*      | **None**   | ***short***  | End Month (not padded) |
| *end.day*    | **None**   | ***short***  | End Day (not padded) |
| *end.hour*    | *hrs*      | ***short***  | End Hour  |
| *end.minute*    | *min*      | ***short***  | End Minute |
| *end.second*    | *s*        | ***short***  | End Seconds |


### Vectors 

!!! Note
    Let the number of radars in a given GRID record be defined as *numstid*, and let the number of gridded velocity vectors in a given record be defined as *numv*. 

| Field name  | Units           | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:         | :-------:      | :---:       | :---                                                                        |
| *stid*      |  **None**       |  *[numstid]*     | ***short*** | A list of of numeric station IDs that provided data for the record |
| *channel*   |  **None**       |  *[numstid]*     | ***short*** | A list of channel numbers associated to the station id the record |
| *nvec*      | **None** | *[numstid]*  | ***short*** | Number of velocity vectors for each station|
| *freq*      | *kHz* | *[numstid]* | ***float*** | Transmitted frequency for each radar |
| *major.revision* | **None** | *[numstid]* | ***short*** | Major `make_grid` version number                    |
| *minor.revision* | **None** | *[numstid]* | ***short*** | Minor `make_grid` version number |
| *program.id*     | **None** | *[numstid]* | ***short*** | Control program ID | 
| *noise.mean*      | **None** | *[numstid]* | ***float*** | Mean noise |
| *noise.sd*        | **None** | *[numstid]* | ***float*** | Noise Standard deviation |
| *gsct*            | **None** | *[numstid]* | ***short*** | Groundscatter flag |
| *v.min*           | *m/s* | *[numstid]* | ***float*** | Minimum velocity threshold |
| *v.max*           | *m/s* | *[numstid]* | ***float*** | Velocity maximum threshold |
| *p.min*           | **None** | *[numstid]* | ***float*** | Power minimum threshold |
| *p.max*           | **None** | *[numstid]* | ***float*** | Power maximum threshold |
| *w.min*           | *m/s*  | *[numstid]* | ***float*** | Spectral width minimum threshold |
| *w.max*           | *m/s*  | *[numstid]* | ***float*** | Spectral width maximum threshold |
| *ve.min*          | *m/s*  | *[numstid]* | ***float*** | Velocity error minimum threshold |
| *ve.max*          | *m/s*  | *[numstid]* | ***float*** | Velocity error maximum threshold |
| *vector.mlat*     | *degrees* | *[numv]* | ***float*** | Magnetic Latitude |
| *vector.mlon*     | *degrees* | *[numv]* | ***float*** | Magnetic Longitude |
| *vector.kvect*    | *degrees* | *[numv]*   | ***float*** | Magnetic Azimuth |
| *vector.stid*     | **None**  | *[numv]*   | ***short*** | Station identifier |
| *vector.channel*  | **None**  | *[numv]*   | ***short*** | Channel number |  
| *vector.index*    | **None**  | *[numv]*   | ***int***   | Grid cell index |
| *vector.vel.median* | *m/s* | *[numv]*   | ***float*** | Weighted mean velocity magnitude |
| *vector.vel.sd*     | *m/s*   | *[numv]*   | ***float*** | Velocity standard deviation |
| *vector.pwr.median* | *dB*  | *[numv]*   | ***float*** | Weighted mean power|
| *vector.pwr.sd*     | *dB*  | *[numv]*   | ***float*** | Power standard deviation|
| *vector.wdt.median* | *m/s* | *[numv]*   | ***float*** | Weighted mean spectral width|
| *vector.wdt.sd      | *m/s* | *[numv]*   | ***float*** | Standard deviation of spectral width|

## File structure

GRID files typically contain up to 24 hours of data. Individual records in a GRID file contain a record for each integration time period (default 120 seconds) from one or more radars.
