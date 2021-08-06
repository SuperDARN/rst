<!-- Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
author(s): Marina Schmidt 

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->
# RAWACF files 

RAWACF files are raw files produced at radar sites. 

Sometimes they are post-processed from IQDAT files or converted from the older-format dat files (see `dattorawacf`).

## Naming Conventions

Currently the common naming convention for RAWACF files is:

> YYYYMMDD.HH.mm.ss.<3-letter abbreviation>.rawacf

SuperDARN radars routinely change operating frequency. Some operating modes use multiple frequencies, either at the same time, or by alternating between frequencies. As a result, some files from some radars include all records for all frequencies in the same file and other radars separate out the individual frequencies into separate files, designated by a channel letter in the file name:

> YYYYMMDD.HH.mm.ss.<3-letter abbreviation>.[a-d].rawacf

For example, on 2019-02-01 the King Salmon radar (KSR) was operating simultaneously on two channels as seen from the file names `20190201.0401.00.ksr.a.rawacf` and `20190201.0401.00.ksr.b.rawacf`.
Each file contains data from a different frequency channel. 

!!! Note
        Sometimes modes like `twofsound` will write data into a single file. In this case the two frequencies are marked as two separate channels, denoted using the `channel` parameter. However, it is important to note that some SuperDARN radars have stereo capability (transmitting and receiving on 2 frequencies simultaneously), which was the original intended usage of the `channel` parameter.

## Fields

RAWACF files contain a record that contains scalar and vector fields. 

### Scalars

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


### Vectors 

!!! Note
    *slist* contains the range gates that obtained data points during the integration period of the beam. 

| Field name  | Units    | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:  | :-------:      | :---:       | :---                                                                        |
| *ptab*      | *None*   | *[mppul]*      | short       | Pulse table, showing when pulses are transmitted as integer multiples of mpinc/smsep                                                              |
| *ltab*      | *None*   | *[mplgs+1][2]* | short       | Lag table, showing which pulses to use to estimate each lag                 |
| *pwr0*      | *None*     | *[nrang]*      | float       | Lag zero power, estimated from voltage samples (not fitted)  |
| *slist*     | *None*   | *[0-nrang]*    | short       | List of range gates with ACF/XCF data available |
| *acfd*      | *None*   | *[2][mplgs][0-nrang]*    | float       | Calculated ACFs                                                             |
| *xcfd*      | *None*   | *[2][mplgs][0-nrang]*    | float       | Calculated XCFs                                                             |

## File structure

RAWACF files contain typically 2 hours of data. Individual records in the RAWACF file contain the raw data for a single integration period (usually 3s or 7s, but depends on operating mode of the radar). 
In the standard operational mode (common mode) where each beam is scanned sequentially, the beam number `bmnum` will increase or decrease by 1 after each integration period. Other control programs may involve different beam sequences, including sampling the beams in a different order, or sampling a subset of the available beams.
A "scan" is a beam sequence which gets repeated. In the common mode (*normal scan*), one scan is completed when each beam has been sampled sequentially from 0 to `bmnum-1` (or `bmnum-1` to 0). Radars with more than 16 beams *may* sample a subset of beams rather than their full field of view in order to maintain a 1min scan time. 
Scans usually begin on whole-minute boundaries and last for either 1min or 2min. Custom control programs which define different scan lengths may also exist. The `scan` flag is used to indicate the beginning of each scan pattern. A value of 1 or -1 indicates the beginning of the scan, and then the value changes to 0 for the rest of the scan. When the `scan` value changes from 0 back to 1 this indicates the end of the scan. 

!!! Note 
    Different *control programs* in general have different beam patterns; `cp` will indicate the *control program* numerical value, and `combf` sometimes contains the *control program's* command/name. 
