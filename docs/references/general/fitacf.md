<!-- Copyright (C) 2020 SuperDARN Canada, University of Saskatchewan 
author(s): Marina Schmidt 

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

Modifications:
    2022-11-23 Emma Bland (UNIS) Updated field names and descriptions
     
-->
# FITACF Files 

FITACF files are produced from RAWACF files. They contain the fitted data products determined using one of the SuperDARN ACF fitting algorithms.


## Naming Convention

The community standard for naming FITACF files is:

> YYYYMMDD.HH.mm.ss.<3-letter radar code>.[a-d].fitacf

The field [a-d] is used to separate data sampled at different frequencies into separate files. This field may not be used if all the available data are provided in a single file.



## Scalar Fields

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
| *nrang*                 | **None**         | ***short***  | Number of range gates                                                             |
| *frang*                 | *km*       | ***short***  | Distance to the first range gate                                                        |
| *rsep*                  | *km*       | ***short***  | Range separation                                                                        |
| *xcf*                   | **None**         | ***short***  | XCF flag, indicates if XCF for elevation angle estimation is available (1) or not (0)                                                                                |
| *tfreq*                 | *kHz*      | ***short***  | Transmitted frequency                                                                   |
| *mxpwr*                 | *dB*       | ***int***    | Maximum power
| *lvmax*                 | **None**         | ***int***    | Maximum noise level allowed                                                             |
| *combf*                 | **None**         | ***string*** | Control program name or command line, or comment buffer                                 |
| *algorithm*             | **None**         | ***string*** | Name of the fitting algorithm used to fit the ACFs                                |
| *fitacf.revision.major* | **None**         | ***int***    | Major version number of the FITACF algorithm                                            |
| *fitacf.revision.minor* | **None**         | ***int***    | Minor version number of the FITACF algorithm                                            |
| *noise.sky*             | **None**         | ***float***  | Sky noise                                                                               |
| *noise.lag0*            | **None**         | ***float***  | Lag zero power of noise ACF                                                             |
| *noise.vel*             | **None**         | ***float***  | Velocity from fitting the noise ACF                                                     |
| *tdiff*                 | **us**           | ***float***  | Value of time differential used when calculating elevation angles                 |
 

!!! Note
    The original intended usage of the `channel` parameter was to identify the data from the two separate frequency channels of the stereo radars (e.g. Hankasalmi). However, this parameter has sometimes been used for non-stereo radars when data from two different frequencies have been recorded in a single file.



## Vector Fields

!!! Note
    The length of the vector fields containing fitted data varies for each time record because only the range gates containing valid ACFs are recorded. The range gates with valid ACFs are listed in the *slist* field, which has length *num_pts*. The definition of a valid ACF is different for each fitting algorithm. For `lmfit2`, all ACFs are fitted so `num_pts` is always equal to the total number of range gates (*nrang*).

!!! Note
    The arrays for the XCF fits are included only if the `xcf` is set to 1. There are some differences in the XCF fields depending on which fitting algorithm is used.


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
| *x_p_s_e* | *dB* | *[num_pts]* |  ***float*** | Power error from sigma fit of XCF | 
| *x_v* | *m/s* | *[num_pts]* | ***float*** | Velocity from fit of XCF | 
| *x_v_e* | *m/s* | *[num_pts]* | ***float*** | Velocity error from fit of XCF | 
| *x_w_l* | *m/s* | *[num_pts]* | ***float*** | Spectral width from lambda fit of XCF | 
| *x_w_l_e* | *m/s* | *[num_pts]* | ***float*** | Spectral width error from lambda fit of XCF | 
| *x_w_s* | *m/s* | *[num_pts]* | ***float*** | Spectral width from sigma fit of XCF | 
| *x_w_s_e* | *m/s* | *[num_pts]* | ***float*** | Spectral width error from sigma fit of XCF | 
| *phi0* | *radians* | *[num_pts]* | ***float*** | Phase determination at lag zero of the ACF | 
| *phi0_e* | *radians* | *[num_pts]* | ***float*** | Phase determination error at lag zero of the ACF | 
| *elv* | *degrees* | *[num_pts]* | ***float*** | Elevation angle estimate recommended for scientific use |
| *elv_fitted* | *degrees* | *[num_pts]* | ***float*** | Alternative elevation angle determined from fitted phase |
| *elv_error* | *degrees* | *[num_pts]* | ***float*** | Least-square elevation angle error |
| *elv_low* | *degrees* | *[num_pts]* | ***float*** | Lowest estimate of elevation angle |
| *elv_high* | *degrees* | *[num_pts]* | ***float*** | Highest estimate of elevation angle |
| *x_sd_l* | **None** | *[num_pts]* | ***float*** | Standard deviation of lambda fit of XCF |
| *x_sd_s* | **None** | *[num_pts]* | ***float*** | Standard deviation of sigma fit of XCF |
| *x_sd_phi* | **None** | *[num_pts]* | ***float*** | Standard deviation of phase fit of XCF |

