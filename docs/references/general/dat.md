<!-- Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
author(s): Marina Schmidt 

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->
# DAT files 

DAT files are a historical file format for raw SuperDARN data. They were produced at radar sites between 1992 and 2006. 

They can be converted to the RAWACF file format using RST's `dattorawacf` command.

## Naming Conventions

The community standard for naming DAT files is:

> YYYYMMDDHH.<1-letter radar abbreviation>.dat

| 1 letter radar abbreviation | 3 letter abbreviation | radar name     |
| :----                       | :---                  | :---           |
| g                           | gbr                   | Goose Bay      |
| s                           | sch                   | Schefferville  |
| k                           | kap                   | Kapuskasing    |
| h                           | hal                   | Halley         |
| t                           | sas                   | Saskatoon      |
| b                           | pgr                   | Prince George  |
| a                           | kod                   | Kodiak         |
| w                           | sto                   | Stokkseyri     |
| e                           | pyk                   | Pykkvibaer     |
| f                           | han                   | Hankasalmi     |
| d                           | san                   | SANAE          |
| j                           | sys                   | Syowa South    |
| n                           | sye                   | Syowa East     |
| r                           | tig                   | TIGER          |
| p                           | ker                   | Kerguelen       |
| c                           | ksr                   | King Salmon    |
| u                           | unw                   | Unwin          |
| i                           | wal                   | Wallops Island |


DAT files usually contain 2 hours of data. If the radar restarted or changed to a different control program during a 2-hour interval, a new DAT file is created. When this occurs, a letter is appended to the filename after the 1-letter radar abbreviation.

> YYYYMMDDHH.<1-letter radar abbreviation>[a-z].dat

!!! Warning
    Do not confuse the letters in DAT-format filenames with the letters representing channels in RAWACF-format filenames. 
    
    
## Scalar Fields

| Field name             | Units     | Data Type | Description                                                                                                                                                                                      |
| :----------            | :-----:   | :-------: | :---                                                                                                                                                                                             |
| *radar.revision.major* | *None*    | char      | Major version number of the radar operating system                                                                                                                                               |
| *radar.revision.minor* | *None*    | char      | Minor version number of the radar operating system                                                                                                                                               |
| npr | *None* | char | Total number of scalar and vector fields in a DAT record   |
| *stid*                 | *None*    | short     | Station identifier                                                                                                                                                                               |
| *year*                 | *None*    | short     | Year                                                                                                                                                                                             |
| *month*                | *None*    | short     | Month number (not padded)                                                                                                                                                                        |
| *day*                  | *None*    | short     | Day (not padded)                                                                                                                                                                                 |
| *hour*                 | *hrs*     | short     | Hour                                                                                                                                                                                             |
| *minut*                | *min*     | short     | Minute                                                                                                                                                                                           |
| *sec*                  | *s*       | short     | Seconds                                                                                                                                                                                          |
| *txpow*                | *kW*      | short     | Transmitted power                                                                                                                                                                                |
| *nave*                 | *None*    | short     | Number of pulse sequences transmitted                                                                                                                                                            |
| *atten*                | *None*    | short     | Attenuation level (0-3)                                                                                                                                                                               |
| *lagfr*                | *us*      | short     | Lag to first range                                                                                                                                                                               |
| *smsep*                | *us*      | short     | Sample separation                                                                                                                                                                                |
| *ercod*                | *None*    | short     | Error Code                                                                                                                                                                                       |
| *stat.agc*             | *None*    | short     | Automatic gain control status word                                                                                                                                                               |
| *stat.lopwr*           | *None*    | short     | Low power status word                                                                                                                                                                            |
| *nbaud*                | *None*    | short     | The number of elements in the pulse code                                                                                                                                                         |
| *noise*                | *None*    | float     | Noise level measured during the clear frequency search                                                                                                                                                     |
| *noise.mean*           | *None*    | float     | Average noise across the frequency band                                                                                                                                                          |
| *channel*              | *None*    | short     | Channel number, used to denote different Tx/Rx channels on Stereo radars, and to denote changes in radar operating parameters between scans, e.g. alternating between 2 frequencies scan-to-scan |
| *bmnum*                | *None*    | short     | Beam number, zero based indexing                                                                                                                                                                 |
| *bmazm*                | *degrees* | float     | Beam azimuth                                                                                                                                                                                     |
| *scan*                 | *None*    | short     | Scan flag, indicating the scan mode.                                                                                                                                                             |
| *offset*               | *us*      | short     | Offset between channels for a stereo radar                                                                                                                                                       |
| *rxrise*               | *us*      | short     | Receiver rise time                                                                                                                                                                               |
| *intt*                 | *s*       | short     | Integration period                                                                                                                                                            |
| *txpl*                 | *us*      | short     | Transmitted pulse length                                                                                                                                                                         |
| *mpinc*                | *us*      | short     | Multi-pulse increment (lag separation)                                                                                                                                                           |
| *mppul*                | *None*    | short     | Number of pulses in the sequence                                                                                                                                                                 |
| *mplgs*                | *None*    | short     | Number of lags in the lag table                                                                                                                                                                  |
| *nrang*                | *None*    | short     | Number of range gates                                                                                                                                                                            |
| *frang*                | *km*      | short     | Distance to the first range gate                                                                                                                                                                 |
| *rsep*                 | *km*      | short     | Range separation                                                                                                                                                                                 |
| *xcf*                  | *None*    | short     | XCF flag, indicating whether the power, velocity, spectral width and phi0 parameters fitted from the cross-correlation function are available (xcf=1) or not available (xcf=0)                   |
| *tfreq*                | *kHz*     | short     | Transmitted frequency                                                                                                                                                                            |
| *mxpwr*                | *None*      | int       | Maximum power                                                                                                                                                                                    |
| *lvmax*                | *None*    | int       | Maximum noise level allowed                                                                                                                                                                      |
| usr_resL1              | *None*    | int       | User defined variable
| usr_resL2              | *None*    | int       | User defined variable                                                                                                                                                                          |
| cp                     | *None*     | short     | Control program number                                                                                                                                                                           |
| usr_resS1              | *None*    | short     | User defined variable                                                                                                                                                                            |
| usr_resS2              | *None*    | short     | User defined variable                                                                                                                                                                            |
| usr_resS3              | *None*    | short     | User defined variable                                                                                                                                                                            |
| *combf*                | *None*    | string    | Comment buffer, usually contains the control program name                                                                                                                                        |


## Vector Fields

*slist* contains the range gates with lag zero power above the threshold level.

| Field name  | Units    | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:  | :-------:      | :---:       | :---                                                                        |
| *ptab*      | *None*   | *[mppul]*      | short       | Pulse table, showing when pulses are transmitted as integer multiples of mpinc/smsep                                                              |
| *ltab*      | *None*   | *[mplgs+1][2]* | short       | Lag table, showing which pulses to use to estimate each lag                 |
| *pwr0*      | *None*     | *[nrang]*      | float       | Lag zero power, estimated from voltage samples (not fitted)  |
| *acfd*      | *None*   | *[2][mplgs][0-nrang]*    | float       | Calculated ACFs                                                             |
| *xcfd*      | *None*   | *[2][mplgs][0-nrang]*    | float       | Calculated XCFs                                                             |

