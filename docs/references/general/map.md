<!-- 
copyright (C) 2020 VT SuperDARN, Virginia Polytechnic Institute & State University 
author: Kevin Sterne

Modifications:
    2022-11-28 Emma Bland (UNIS) Updated file format description
    
-->

# Map files 

Map files are post-processed data produced from grid files. They include all the information included in the original grid file, plus the additional information related to the convection mapping. Map files are generated using a multi-step process described in the [`map_grid` tutorial](../../user_guide/make_grid.md).

## Naming Convention

Separate map files should be created for each hemisphere. The community standard for naming these files is:

> YYYYMMDD.north.map

> YYYYMMDD.south.map

## Scalar Fields

Start and end times refer to the start and end of the integration period (usually 2 minutes).


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
| *model.name*    | **None**   | ***string***  | Name of the statistical model used (TS18, CS10, RG96, etc.) |
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
| *pot.min.err*   | *V*        | ***double***  | Minimum polar-cap potential error |
 


## Vector fields 

In the table below, the following values describe the dimensionality of the map file fields: 

- *numstid*: the number of radars included in that grid record
- *numv*: the total number of gridded velocity vectors
- *numft*: the number of values used in the spherical harmonic analysis
- *nummd*: the number of vectors output by the statistical convection model
- *numbd*: the number of points in the Heppner-Maynard boundary


A map file will not necessarily contain a complete set of all the variables listed below. The processing is divided into stages and new fields are added to the file as appropriate. For example, the `boundary.mlat` and `boundary.mlon` fields are added by `map_addhmb`, and the `model.mlat`, `model.mlon`, `model.kvect`, and `model.vel.median` are added by `map_addmodel`.



| Field name  | Units           | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:         | :-------:      | :---:       | :---                                                                        |
| *stid*      |  **None**       |  *[numstid]*     | ***short*** | A list of numeric station IDs that provided data for the record |
| *channel*   |  **None**       |  *[numstid]*     | ***short*** | A list of channel numbers associated with the station id |
| *nvec*      | **None** | *[numstid]*  | ***short*** | Number of velocity vectors for each station |
| *freq*      | *kHz* | *[numstid]* | ***float*** | Transmitted frequency for each radar |
| *major.revision* | **None** | *[numstid]* | ***short*** | Major `make_grid` version number |
| *minor.revision* | **None** | *[numstid]* | ***short*** | Minor `make_grid` version number |
| *program.id*     | **None** | *[numstid]* | ***short*** | Control program ID | 
| *noise.mean*      | **None** | *[numstid]* | ***float*** | Mean sky noise |
| *noise.sd*        | **None** | *[numstid]* | ***float*** | Sky noise standard deviation |
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
| *vector.srng*     | *km*      | *[numv]*   | ***float*** | Slant range |
| *vector.stid*     | **None**  | *[numv]*   | ***short*** | Station identifier |
| *vector.channel*  | **None**  | *[numv]*   | ***short*** | Channel number |
| *vector.index*    | **None**  | *[numv]*   | ***int***   | Grid cell index |
| *vector.vel.median* | *m/s* | *[numv]*   | ***float*** | Weighted mean velocity magnitude |
| *vector.vel.sd*     | *m/s* | *[numv]*   | ***float*** | Velocity standard deviation |
| *vector.pwr.median* | *dB*  | *[numv]*   | ***float*** | Weighted mean power |
| *vector.pwr.sd*     | *dB*  | *[numv]*   | ***float*** | Power standard deviation |
| *vector.wdt.median* | *m/s* | *[numv]*   | ***float*** | Weighted mean spectral width |
| *vector.wdt.sd*     | *m/s* | *[numv]*   | ***float*** | Standard deviation of spectral width |
| *N*               | **None** | *[numft]* | ***double*** | L value of the expansion between 0 and Lmax |
| *N+1*             | **None** | *[numft]* | ***double*** | M value of the expansion between -L and +L, negative values indicating the sin(M*phi) term |
| *N+2*             | **None** | *[numft]* | ***double*** | Value of the coefficient  |
| *N+3*             | **None** | *[numft]* | ***double*** | Estimate of the 1-sigma error of the coefficient |
| *model.mlat*      | *degrees* | [nummd] | ***float*** | Magnetic Latitudes of the model vectors |
| *model.mlon*      | *degrees* | [nummd] | ***float*** | Magnetic Longitudes of the model vectors |
| *model.kvect*     | *degrees* | [nummd] | ***float*** | Magnetic Azimuths of the model vectors |
| *model.vel.median* | *m/s*    | [nummd] | ***float*** | Velocity medians of the model vectors |
| *boundary.mlat*   | *degrees* | [numbd] | ***float*** | Magnetic Latitudes of the lower latitude boundary |
| *boundary.mlon*   | *degrees* | [numbd] | ***float*** | Magnetic Longitudes of the lower latitude boundary |

