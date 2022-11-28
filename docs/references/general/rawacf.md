<!-- Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
author(s): Marina Schmidt 

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

Modifications:
    2022-11-28 Emma Bland (UNIS) Updated file format description

-->
# RAWACF files 

RAWACF files are raw files produced at radar sites. 

Sometimes they are post-processed from IQDAT files or converted from the older-format dat files (see `dattorawacf`).

## Naming Convention

The community standard for naming RAWACF files is:

> YYYYMMDD.HH.mm.ss.<3-letter radar code>.[a-d].rawacf

The field [a-d] is used to separate data sampled at different frequencies into separate files. This field may not be used if all the available data are provided in a single file.


## Scalar Fields


| Field name              | Units    | Data Type | Description                                                                             |
| :----------             | :-----:  | :-------: | :---                                                                                    |
| *radar.revision.major*  | *None*   | char      | Major version number of the radar operating system                                      |
| *radar.revision.minor*  | *None*   | char      | Minor version number of the radar operating system                                      |
| *origin.code*           | *None*   | char      | Code indicating origin of the data (0 if generated at the radar)                        |
| *origin.time*           | *None*   | string    | Text indicating when the file was generated                                             |
| *origin.command*        | *None*   | string    | Command line routine used to generate the file                                          |
| *cp*                    | *None*   | short     | [Control program identifier](http://superdarn.thayer.dartmouth.edu/WG-sched/cpids.html) |
| *stid*                  | *None*   | short     | Station identifier                                                                      |
| *time.yr*               | *None*   | short     | Year                                                                                    |
| *time.mo*               | *None*   | short     | Month number (not padded)                                                               |
| *time.dy*               | *None*   | short     | Day (not padded)                                                                        |
| *time.hr*               | *hrs*    | short     | Hour                                                                                    |
| *time.mt*               | *min*    | short     | Minute                                                                                  |
| *time.sc*               | *s*      | short     | Seconds                                                                                 |
| *time.us*               | *us*     | int       | Micro-seconds                                                                           |
| *txpow*                 | *kW*     | short     | Transmitted power                                                                       |
| *nave*                  | *None*   | short     | Number of pulse sequences transmitted                                                   |
| *atten*                 | *None*   | short     | Attenuation level                                                                       |
| *lagfr*                 | *us*     | short     | Lag to first range                                                                      |
| *smsep*                 | *us*     | short     | Sample separation                                                                       |
| *ercod*                 | *None*   | short     | Error Code                                                                              |
| *stat.agc*              | *None*   | short     | Automatic gain control status word                                                                         |
| *stat.lopwr*            | *None*   | short     | Low power status word                                                                       |
| *noise.search*          | *None*   | float     | Calculated noise from clear frequency search                                            |
| *noise.mean*            | *None*   | float     | Average noise from clear frequency band                                                 |
| *channel*               | *None*   | short     | Channel number, used to denote different Tx/Rx channels on Stereo radars, and to denote changes in radar operating parameters between scans, e.g. alternating between 2 frequencies scan-to-scan      |
| *bmnum*                 | *None*   | short     | Beam number, zero based indexing                                                        |
| *bmazm*                 | *degrees*   | float     | Beam azimuth                                                                            |
| *scan*                  | *None*   | short     | Scan flag, switches between 0 to +/-1. For some radars, the sign indicates scan direction. |
| *offset*                | *us*     | short     | Offset between channels for a stereo radar                                              |
| *rxrise*                | *us*     | short     | Receiver rise time                                                                      |
| *intt.sc*               | *s*      | short     | Number of seconds of integration time                                                   |
| *intt.us*               | *us*     | int       | Number of microseconds of integration time                                              |
| *txpl*                  | *us*     | short     | Transmitted pulse length                                                                |
| *mpinc*                 | *us*     | short     | Multi-pulse increment (lag separation)                                                                  |
| *mppul*                 | *None*   | short     | Number of pulses in the sequence                                                        |
| *mplgs*                 | *None*   | short     | Number of lags in the lag table                                              |
| *mplgexs*               | *None*   | short     | Number of lags used for tauscan. This field originates from `fitex` algorithm and is not used in RAWACF file format. | 
| *ifmode*                | *None*   | short     | A flag indicating that the receiver voltage samples were obtained by digitizing the signal at an intermediate frequency (and then digitally mixed to baseband) instead of directly sampling the RF signals (`ifmode=0`).         |
| *nrang*                 | *None*   | short     | Number of range gates                                                             |
| *frang*                 | *km*     | short     | Distance to the first range gate                                                        |
| *rsep*                  | *km*     | short     | Range separation                                                                        |
| *xcf*                   | *None*   | short     | XCF flag, indicating whether the power, velocity, spectral width and phi0 parameters fitted from the cross-correlation function are available (xcf=1) or not available (xcf=0)   |
| *tfreq*                 | *kHz*    | short     | Transmitted frequency                                                                   |
| *mxpwr*                 | *dB*     | int       | Maximum power                                                                           |
| *lvmax*                 | *None*   | int       | Maximum noise level allowed                                                             |
| *combf*                 | *None*   | string    | Comment buffer, usually contains the control program name                            |
| *rawacf.revision.major* | *None*   | int       | Major version number of the RAWACF algorithm                                            |
| *rawacf.revision.minor* | *None*   | int       | Minor version number of the RAWACF algorithm                                            |
| *thr*                   | *None*   | float     | Threshold factor                                                                        |

!!! Note
    The original intended usage of the `channel` parameter was to identify the data from the two separate frequency channels of the stereo radars (e.g. Hankasalmi). However, this parameter has sometimes been used for non-stereo radars when data from two different frequencies have been recorded in a single file.

## Vector Fields

| Field name  | Units    | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:  | :-------:      | :---:       | :---                                                                        |
| *ptab*      | *None*   | *[mppul]*      | short       | Pulse table, showing when pulses are transmitted as integer multiples of mpinc/smsep                                                              |
| *ltab*      | *None*   | *[mplgs+1][2]* | short       | Lag table, showing which pulses to use to estimate each lag                 |
| *pwr0*      | *None*     | *[nrang]*      | float       | Lag zero power, estimated from voltage samples (not fitted)  |
| *slist*     | *None*   | *[0-nrang]*    | short       | List of range gates with ACF/XCF data available |
| *acfd*      | *None*   | *[2][mplgs][0-nrang]*    | float       | Calculated ACFs                                                             |
| *xcfd*      | *None*   | *[2][mplgs][0-nrang]*    | float       | Calculated XCFs                                                             |


