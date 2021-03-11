<!-- 
copyright (C) 2020 VT SuperDARN, Virginia Polytechnic Institute & State University 
author: Kevin Sterne
-->

# Map files 

Map files are post-processed data produced from grid files.

## Naming Conventions

### Map files for each hemisphere

Map files are processed by starting with the combined (or single radar) [grid file](grid.md) as an input to `map_grd`.

From here, additional processsing occurs by adding additional data sources (Heppner-Maynard Boundary, IMF) and statistical convection models (TS18) before having a spherical harmonic fitting performed.

If a map file contains only a small time amount of data, it may be named:

> YYYYMMDD.HH.MM.ss.hhhhh.map

where `hhhhh` is the hemisphere (north or south).

If a map file is produced with 24-hours of data then the common naming convenction is:

> YYYYMMDD.hhhhh.map

where the hour, minute, and second fields are dropped.

## Fields

Each record of a map file contains scalar and vector fields. 

### Scalars

The following times refer to the start and end of the integration period.

| Field name       | Units      | Data Type    | Description                                  |
| :----------      | :-----:    | :-------:    | :---                                           |
| *start.year*  | **None**   | ***short***  | Start Year |
| *start.month*  | **None**   | ***short***  | Start Month number (not padded)                            |
| *start.day*  | **None**   | ***short***  | Start Day (not padded)  |
| *start.hour*  | *hr*      | ***short***  | Start Hour   |
| *start.minute*  | *min*      | ***short***  | Start Minute |
| *start.second*  | *s*        | ***short***  | Start Seconds  |
| *end.year*    | **None**   | ***short***  | End Year |
| *end.month*    | **None**   | ***short***  | End Month (not padded) |
| *end.day*    | **None**   | ***short***  | End Day (not padded) |
| *end.hour*    | *hr*      | ***short***  | End Hour  |
| *end.minute*    | *min*      | ***short***  | End Minute |
| *end.second*    | *s*        | ***short***  | End Seconds |
| *map.major.revision* | **None**  | ***short*** | Major revision number of mapping routine |
| *map.minor.revision* | **None**  | ***short*** | Minor revision number of mapping routine |
| *source*      | **None**      | ***string*** | Binary that produced this record |
| *doping.level*  | **None**   | ***short***   | Model doping level |
| *model.wt*      | **None**   | ***short***   | Model weighting    |
| *error.wt*      | **None**   | ***short***   | Error weighting    |
| *IMF.flag*      | **None**   | ***short***   | IMF availability flag |
| *IMF.delay*     | *min*      | ***short***   | IMF delay |
| *IMF.Bx*        | *nT*       | ***double***  | IMF Bx component   |
| *IMF.By*        | *nT*       | ***double***  | IMF By component   |
| *IMF.Bz*        | *nT*       | ***double***  | IMF Bz component   |
| *IMF.Vx*        | *km/s*     | ***double***  | Solar wind speed, X-component, GSE |
| *IMF.tilt*      | *degree*  | ***double***   | Dipole tilt angle  |
| *IMF.Kp*        | **None**   | ***double***  | Kp index number |
| *model.angle*   | **None**   | ***string***  | Statistical clock angle of B |
| *model.level*   | **None**   | ***string***  | Statistical model level |
| *model.tilt*    | **None**   | ***string***  | Calculated tilt angle |
| *model.name*    | **None**   | ***string***  | Name of the statical model used (TS18, CS10, RG96, etc.) |
| *hemisphere*    | **None**   | ***short***   | Hemisphere flag (north=1) |
| *noigrf*        | **None**   | ***short***   | Flag for no-IGRF model present |
| *fit.order*     | **None**   | ***short***   | Order of spherical harmonic fit |
| *latmin*        | **None**   | ***float***   | Lower latitude boundary of data |
| *chi.sqr*       | **None**   | ***double***  | True chi-squared error          |
| *chi.sqr.dat*   | **None**   | ***double***  | Chi-squared error for data only |
| *rms.err*       | **None**   | ***double***  | RMS error                       |
| *lon.shft*      | *degree*   | ***float***   | Longitudinal pole shift         |
| *lat.shft*      | *degree*   | ***float***   | Latitudinal pole shift          |
| *mlt.start*     | *hr*       | ***double***  | MLT at start of record          |
| *mlt.end*       | *hr*       | ***double***  | MLT at end of record            |
| *mlt.av*        | *hr*       | ***double***  | MLT at middle of record         |
| *pot.drop*      | *V*        | ***double***  | Cross polar-cap potential drop  |
| *pot.drop.err*  | *V*        | ***double***  | Cross polar-cap potential drop error |
| *pot.max*       | *V*        | ***double***  | Maximum polar-cap potential     |
| *pot.max.err*   | *V*        | ***double***  | Maximum polar-cap potential error |
| *pot.min*       | *V*        | ***double***  | Minimum polar-cap potential     |
| *pot.min.err*   | *V*        | ***double***  | Minimum polar-cap potentail error |
 


### Arrays 

The array components of the map file format are listed below. The map format is a superset of the grid formt so it contains the same arrays.

!!! Note
 - Let the number of radars in a given map record be defined as *numstid*
 - Let the number of gridded velocity vectors in a given record be defined as *numv*
 - Let the number of values for the spherical harmonic analysis be defined as *numft*
 - Let the number of vectors output by the model used be defined as *nummd*
 - Let the number of the Heppner-Maynard boundary outputs be defined as *numbd*

| Field name  | Units           | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:         | :-------:      | :---:       | :---                                                                        |
| *stid*      |  **None**       |  *[numstid]*     | ***short*** | A list of of numeric station IDs that provided data for the record |
| *channel*   |  **None**       |  *[numstid]*     | ***short*** | A list of channel numbers associated to the station id the record |
| *nvec*      | **None** | *[numstid]*  | ***short*** | Number of velocity vectors for each station |
| *freq*      | *kHz* | *[numstid]* | ***float*** | Transmitted frequency for each radar |
| *major.revision* | **None** | *[numstid]* | ***short*** | Major `make_grid` version number |
| *minor.revision* | **None** | *[numstid]* | ***short*** | Minor `make_grid` version number |
| *program.id*     | **None** | *[numstid]* | ***short*** | Control program ID | 
| *noise.mean*      | **None** | *[numstid]* | ***float*** | Mean noise |
| *noise.sd*        | **None** | *[numstid]* | ***float*** | Noise Standard deviation |
| *gsct*            | **None** | *[numstid]* | ***short*** | Groundscatter flag |
| *v.min*           | *m/s* | *[numstid]* | ***float*** | Minimum velocity threshold |
| *v.max*           | *m/s* | *[numstid]* | ***float*** | Maximum velocity threshold |
| *p.min*           | **None** | *[numstid]* | ***float*** | Minimum power threshold |
| *p.max*           | **None** | *[numstid]* | ***float*** | Maximum power threshold |
| *w.min*           | *m/s*  | *[numstid]* | ***float*** | Minimum spectral width threshold |
| *w.max*           | *m/s*  | *[numstid]* | ***float*** | Maximum spectral width threshold |
| *ve.min*          | *m/s*  | *[numstid]* | ***float*** | Velocity error minimum threshold |
| *ve.max*          | *m/s*  | *[numstid]* | ***float*** | Velocity error maximum threshold |
| *vector.mlat*     | *degrees* | *[numv]* | ***float*** | Magnetic Latitude |
| *vector.mlon*     | *degrees* | *[numv]* | ***float*** | Magnetic Longitude |
| *vector.kvect*    | *degrees* | *[numv]*   | ***float*** | Magnetic Azimuth |
| *vector.stid*     | **None**  | *[numv]*   | ***short*** | Station identifier |
| *vector.channel*  | **None**  | *[numv]*   | ***short*** | Channel number |  
| *vector.index*    | **None**  | *[numv]*   | ***int***   | Grid cell index |
| *vector.vel.median* | *m/s* | *[numv]*   | ***float*** | Weighted mean velocity magnitude |
| *vector.vel.sd*     | *m/s* | *[numv]*   | ***float*** | Velocity standard deviation |
| *vector.pwr.median* | *dB*  | *[numv]*   | ***float*** | Weighted mean power |
| *vector.pwr.sd*     | *dB*  | *[numv]*   | ***float*** | Power standard deviation |
| *vector.wdt.median* | *m/s* | *[numv]*   | ***float*** | Weighted mean spectral width |
| *vector.wdt.sd      | *m/s* | *[numv]*   | ***float*** | Standard deviation of spectral width |
| *N*               | **None** | *[numft]* | ***double*** | L value of the expansion between 0 and Lmax |
| *N+1*             | **None** | *[numft]* | ***double*** | M value of the expansion between -L and +L, negative values indicating the sin(M*phi) term |
| *N+2*             | **None** | *[numft]* | ***double*** | Value of the coefficient  |
| *N+3*             | **None** | *[numft]* | ***double*** | Stimate of the 1-sigma error of the coefficient |
| *model.mlat*      | *degrees* | [nummd] | ***float*** | Magnetic Latitudes of the model vectors |
| *model.mlon*      | *degrees* | [nummd] | ***float*** | Magnetic Longitudes of the model vectors |
| *model.kvect*     | *degrees* | [nummd] | ***float*** | Magnetic Azimuths of the model vectors |
| *model.vel.median* | *m/s*    | [nummd] | ***float*** | Velocity medians of the model vectors |
| *boundary.mlat*   | *degrees* | [numbd] | ***float*** | Magnetic Latitudes of the lower latitude boundary |
| *boundary.mlon*   | *degrees* | [numbd] | ***float*** | Magnetic Longitudes of the lower latitude boundary |

A map file does not necessarily contain a complete set of all the variables listed above. The processing is dvidied up into stages with variables added to the file as needed.  If the statistical model has been calculated the file will contain the arrays `model.mlat`, `model.mlon`, `model.kvect`, and `model.vel.median`.  If the loer boundary ha been found then the file will contain the arrays `boundary.mlat` and `boundary.mlon`.  If the spherical harmonic analysis has been performed, then the file will contain the arrays `N`, `N+1`, `N+2`, and `N+3`.

## File structure

Map files typically contain up to 24 hours of data. Individual records in a map file contain a record for each integration time period (default 120 seconds) from one or more radars depending on the input grid file.
