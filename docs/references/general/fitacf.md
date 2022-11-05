<!-- Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
author(s): Marina Schmidt 

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->
# FITACF files 

FITACF files are a fitted post-processed data product from RAWACF files (sometimes dat files).

## Naming Conventions

Currently the common naming convention for FITACF files is:

> YYYYMMDD.HH.mm.ss.<3-letter abbreviation>.fitacf

SuperDARN radars routinely change operating frequency. Some operating modes use multiple frequencies, either at the same time, or by alternating between frequencies. As a result, some files from some radars include all records for all frequencies in the same file and other radars separate out the individual frequencies into separate files, designated by a channel letter in the file name:

> YYYYMMDD.HH.mm.ss.<3-letter abbreviation>.[a-d].fitacf

For example, on 2019-02-01 the King Salmon radar (KSR) was operating simultaneously on two channels as seen from the file names `20190201.0401.00.ksr.a.fitacf` and `20190201.0401.00.ksr.b.fitacf`.
Each file contains data from a different frequency channel. 

!!! Note
Sometimes modes like `twofsound` will write data into a single file. In this case the two frequencies are marked as two separate channels, denoted using the `channel` parameter. However, it is important to note that some SuperDARN radars have stereo capability (transmitting and receiving on 2 frequencies simultaneously), which was the original intended usage of the `channel` parameter.

## Fitting Algorithms

RST includes several fitting algorithms which are used to determine the physical properties of the received backscatter (power, velocity, spectral width, elevation) from the mean autocorrelation and crosscorrelation function estimates. 
RST includes the following fitting algorithms:

- FITACF 2.5 (optional)
- FITACF 3.0 (optional)
- [FITEX2](https://doi.org/10.1002/rds.20031) (optional)
- [LMFIT](https://doi.org/10.1002/rds.20031) (optional)

Other fitting algorithms that are not yet implemented in RST:
- [lmfit2](https://doi.org/10.1002/2017RS006450)

## Fields

FITACF files contain a record that contains scalar and vector fields. 

### Scalars

| Field name              | Units            | Data Type    | Description                                                                             |
| :----------             | :-----:          | :-------:    | :---                                                                                    |
| *radar.revision.major*  | **None**         | ***char***   | Major version number of the radar operating system                                      |
| *radar.revision.minor*  | **None**         | ***char***   | Minor version number of the radar operating system                                      |
| *origin.code*           | **None**         | ***char***   | Code indicating origin of the file (0 if generated at the radar)                        |
| *origin.time*           | **None**         | ***string*** | Text indicating when the file was generated                                            |
| *origin.command*        | **None**         | ***string*** | Command line routine used to generate the file                                         |
| *cp*                    | **None**         | ***short***  | [Control program identifier](http://superdarn.thayer.dartmouth.edu/WG-sched/cpids.html) |
| *stid*                  | **None**         | ***short***  | Station identifier                                                                      |
| *time.yr*               | **None**         | ***short***  | Year                                                                                    |
| *time.mo*               | **None**         | ***short***  | Month number (not padded)                                                               |
| *time.dy*               | **None**         | ***short***  | Day (not padded)                                                                        |
| *time.hr*               | *hrs*          | ***short***  | Hour                                                                                    |
| *time.mt*               | *min*          | ***short***  | Minute                                                                                  |
| *time.sc*               | *s*        | ***short***  | Seconds                                                                                 |
| *time.us*               | *us* | ***int***    | Micro-seconds                                                                           |
| *txpow*                 | *kW*       | ***short***  | Transmitted power                                                                       |
| *nave*                  | **None**         | ***short***  | Number of pulse sequences transmitted                                                   |
| *atten*                 | **None**         | ***short***  | Attenuation level                                                                       |
| *lagfr*                 | *us* | ***short***  | Lag to first range                                                                      |
| *smsep*                 | *us* | ***short***  | Sample separation                                                                       |
| *ercod*                 | **None**         | ***short***  | Error Code                                                                              |
| *stat.agc*              | **None**         | ***short***  | AGC status word                                                                         |
| *stat.lopwr*            | **None**         | ***short***  | LOPWR status word                                                                       |
| *noise.search*          | **None**         | ***float***  | Calculated noise from clear frequency search                                            |
| *noise.mean*            | **None**         | ***float***  | Average noise from clear frequency band                                                 |
| *channel*               | **None**         | ***short***  | Channel number, used to denote different Tx/Rx channels on Stereo radars, and to denote changes in radar operating parameters between scans, e.g. alternating between 2 frequencies scan-to-scan      |
| *bmnum*                 | **None**         | ***short***  | Beam number, zero based indexing                                                                          |
| *bmazm*                 | **None**         | ***float***  | Beam azimuth                                                                            |
| *scan*                  | **None**         | ***short***  | Scan flag, switches between 0 to +/-1. For some radars, the sign indicates scan direction.                    |
| *offset*                |  *us* | ***short***  | Offset between channels for a stereo radar                                                       |
| *rxrise*                | *us* | ***short***  | Receiver rise time                                                                      |
| *intt.sc*               | *s*        | ***short***  | Number of seconds of integration time                                             |
| *intt.us*               | *us* | ***int***    | Number of microseconds of integration time                                    |
| *txpl*                  | *us* | ***short***  | Transmitted pulse length                                                                |
| *mpinc*                 | *us* | ***short***  | Multi-pulse increment                                                                   |
| *mppul*                 | **None**         | ***short***  | Number of pulses in the sequence                                                        |
| *mplgs*                 | **None**         | ***short***  | Number of lags estimated from the sequence                                                      |
| *mplgexs*              | **None**         | ***short***  |   Number of lags used for tauscan. This field originates from `fittex` algorithm and is not used in FITACF file format. | 
| *ifmode*                | **None**         | ***short***  |  A flag indicating that the receiver voltage samples were obtained by digitizing the signal at an intermediate frequency (and then digitally mixed to baseband) instead of mixing to baseband and then digitizing (`ifmode=0`).         |
| *nrang*                 | **None**         | ***short***  | Number of range range gates                                                             |
| *frang*                 | *km*       | ***short***  | Distance to the first range gate                                                        |
| *rsep*                  | *km*       | ***short***  | Range separation                                                                        |
| *xcf*                   | **None**         | ***short***  | XCF flag, indicates if XCF for elevation angle estimation is available (1) or not (0)                                                                                |
| *tfreq*                 | *kHz*      | ***short***  | Transmitted frequency                                                                   |
| *mxpwr*                 | *dB*       | ***int***    | Maximum power  
| *lvmax*                 | **None**         | ***int***    | Maximum noise level allowed                                                             |
| *combf*                 | **None**         | ***string*** | Control program name or command line, or comment buffer                                 |
| *fitacf.revision.major* | **None**         | ***int***    | Major version number of the FITACF algorithm                                            |
| *fitacf.revision.minor* | **None**         | ***int***    | Minor version number of the FITACF algorithm                                            |
| *noise.sky*             | **None**         | ***float***  | Sky noise                                                                               |
| *noise.lag0*            | **None**         | ***float***  | Lag zero power of noise ACF                                                             |
| *noise.vel*             | **None**         | ***float***  | Velocity from fitting the noise ACF                                                     |


### Vectors 

!!! Note
    *slist* contains the range gates that obtained data points during the integration period of the beam. The length of *slist* determines the length of the following vector fields, let this be defined as *num_pts*.

The arrays for the XCF fit will only be present in the record if an xcf is calculated.  This is indicated if the scalar `xcf` is set to 1.


| Field name  | Units           | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:         | :-------:      | :---:       | :---                                                                        |
| *ptab*      | **None**        | *[mppul]*      | ***short*** | Pulse table                                                                 |
| *ltab*      | **None**        | *[mplgs+1][2]*    | ***short*** | Lag table, showing which pulses to use to estimate each lag                                                                  |
| *pwr0*      | *dB*      | *[nrang]*      | ***float*** | Lag zero power (actually SNR), estimated from voltage samples (not fitted)                                                              |
| *slist*     | **None**        | *[num_pts]*    | ***short*** | List of range gates where a good fit to the ACF was achieved during the integration period |
| *nlag*      | **None**        | *[num_pts]*    | ***short*** | Number of points in the fit                                                 |
| *qflg*      | **None**        | *[num_pts]*    | ***char***  | Quality of fit flag for ACF                                                 |
| *gflg*      | **None**        | *[num_pts]*    | ***char***  | Ground scatter flag for ACF, 1 - ground scatter, 0 - other scatter          |
| *p_l*       | *dB*      | *[num_pts]*    | ***float*** | Power (actually SNR) from lambda fit of ACF                                                |
| *p_l_e*     | *dB*      | *[num_pts]*    | ***float*** | Power (actually SNR) error from lambda fit of ACF                                          |
| *p_s*       | *dB*      | *[num_pts]*    | ***float*** | Power (actually SNR) from sigma fit of ACF                                                 |
| *p_s_e*     | *dB*      | *[num_pts]*    | ***float*** | Power (actually SNR) error from sigma fit of ACF                                           |
| *v*         | *m/s* | *[num_pts]*    | ***float*** | Velocity from fit of ACF                                                           |
| *v_e* | *m/s* | *[num_pts]* |  ***float*** | Velocity error from fit of ACF |
| *w_l* | *m/s* | *[num_pts]* | ***float*** | Spectral width from lambda fit of ACF | 
| *w_l_e* | *m/s* | *[num_pts]* | ***float*** | Spectral width error from lambda fit of ACF |
| *w_s* | *m/s* | *[num_pts]* | ***float*** | Spectral width from sigma fit of ACF | 
| *w_s_e* | *m/s* | *[num_pts]* | ***float*** | Spectral width error from sigma fit of ACF | 
| *sd_l* | **None** | *[num_pts]* | ***float*** | Standard deviation of lambda fit | 
| *sd_s* | **None** | *[num_pts]* | ***float*** | Standard deviation of sigma fit | 
| *sd_phi* | **None** | *[num_pts]* | ***float*** | Standard deviation of phase fit of ACF | 
| *x_qflg* | **None** | *[num_pts]* | ***char*** | Quality flag of XCF | 
| *x_gflg* | **None** | *[num_pts]* | ***char*** | Ground scatter flag of XCF | 
| *x_p_l* | *dB* | *[num_pts]* | ***float*** | Power from lambda fit of XCF | 
| *x_p_l_e* | *dB* | *[num_pts]* | ***float*** | Power error from lambda fit of XCF | 
| *x_p_s* | *dB* | *[num_pts]* | ***float*** | Power from sigma fit of XCF | 
| *x_p_s_e* | *dB* | *[num_pts]* |  ***float*** | Power  error from sigma fit  of XCF | 
| *x_v* | *m/s* | *[num_pts]* | ***float*** | Velocity from fit of XCF | 
| *x_v_e* | *m/s* | *[num_pts]* | ***float*** | Velocity error from fit of XCF | 
| *x_w_l* | *m/s* | *[num_pts]* | ***float*** | Spectral width from lambda fit of XCF | 
| *x_w_l_e* | *m/s* | *[num_pts]* | ***float*** | Spectral width error from lambda fit of XCF | 
| *x_w_s* | *m/s* | *[num_pts]* | ***float*** | Spectral width from sigma fit of XCF | 
| *x_w_s_e* | *m/s* | *[num_pts]* | ***float*** | Spectral width error from sigma fit of XCF | 
| *phi0* | *radians* | *[num_pts]* | ***float*** | Phase determination at lag zero of the ACF | 
| *phi0_e* | *radians* | *[num_pts]* | ***float*** | Phase determination error at lag zero of the ACF | 
| *elv* | *degrees* | *[num_pts]* | ***float*** | Elevation angle estimate recommended for scientific use |
| *elv_fitted* | *degrees* | *[num_pts]* | ***float*** | Alternative elevation angle determined from fitted phase (FitACF v3 only) |
| *elv_error* | *degrees* | *[num_pts]* | ***float*** | Least-square elevation angle error (FitACF v3 only) |
| *elv_low* | *degrees* | *[num_pts]* | ***float*** | Lowest estimate of elevation angle (FitACF v1-2 only) |
| *elv_high* | *degrees* | *[num_pts]* | ***float*** | Highest estimate of elevation angle (FitACF v1-2 only) |
| *x_sd_l* | **None** | *[num_pts]* | ***float*** | Standard deviation of lambda fit of XCF|
| *x_sd_s* | **None** | *[num_pts]* | ***float*** | Standard deviation of sigma fit of XCF |
| *x_sd_phi* | **None** | *[num_pts]* | ***float*** | Standard deviation of phase fit of XCF |

## File structure

FITACF files contain typically 2 hours of data. Individual records in the FITACF file contain the fitted data for a single integration period (usually 3s or 7s, but depends on operating mode of the radar). 
In the standard operational mode (common mode) where each beam is scanned sequentially, the beam number `bmnum` will change each integration period. However, some control programs will stay on one beam for a whole scan (called a "camping beam"). 
A "scan" is a beam sequence which gets repeated. In the common mode (*normal scan*), one scan is completed when each beam has been sampled sequentially from 0 to `bmnum-1` (or `bmnum-1` to 0). Radars with more than 16 beams *may* sample a subset of beams rather than their full field of view in order to maintain a 1min scan time. 
Scans usually begin on whole-minute boundaries and last for either 1min or 2min. Custom control programs which define different scan lengths may also exist. The `scan` flag is used to indicate the beginning of each scan pattern. A value of 1 or -1 indicates the beginning of the scan, and then the value changes to 0 for the rest the scan. When the `scan` value changes from 0 back to 1 this indicates the end of the scan. 

!!! Note 
    Different *control programs* in general have different beam patterns; `cp` will indicate the *control program* numerical value, and `combf` sometimes contains the *control program's* command/name. 

Occasionally you may find records in a FITACF file which are missing some of the **vector** fields. For example: 
```bash
        float   "noise.sky" = 17.799486
        float   "noise.lag0" = 0.000000
        float   "noise.vel" = 0.000000
arrays:
        short   "ptab" [7]
        short   "ltab" [19][2]
        float   "pwr0" [100]
scalars:
        char    "radar.revision.major" = 1
        char    "radar.revision.minor" = 18
        char    "origin.code" = 0

```
Partial records occur when there are no range gates with a successful fit to the ACF, resulting in an empty `slist`. This is a legacy behavior of the DMAP file format which the Data Analysis Working Group is investigating. 
