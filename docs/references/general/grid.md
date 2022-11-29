<!-- Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
author(s): Marina Schmidt 

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

Modifications:
    2022-11-28 Emma Bland (UNIS) Updated file format description

-->
# GRID files 

GRID files are post-processed data produced from FITACF files, in which the fitted data products are placed on a magnetic latitude/longitude grid of equal-area cells spanning 1 degree of magnetic latitude. They may contain data from one radar or from multiple radars. For more information, see the [`make_grid` tutorial](../../user_guide/make_grid.md).

## Naming Convention

The community standard for naming GRID files containing data from only one radar is:

> YYYYMMDD.<3-letter abbreviation>.grd

When the gridded data from multiple radars have been combined, the files are named as:

> YYYYMMDD.north.grd

> YYYYMMDD.south.grd

*In both cases, the grid files contain 24-hours of data.*


## Scalar Fields

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


## Vector Fields

In the table below, *numstid* is the number of radars included in that grid record, and *numv* is the total number of gridded velocity vectors.


| Field name  | Units           | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:         | :-------:      | :---:       | :---                                                                        |
| *stid*      |  **None**       |  *[numstid]*     | ***short*** | A list of numeric station IDs that provided data for the record |
| *channel*   |  **None**       |  *[numstid]*     | ***short*** | A list of channel numbers associated with the station id |
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


