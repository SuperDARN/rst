# FITACF files 

FITACF files are a fitted post processed data product from RAWACF files (sometimes dat files).

## Naming Conventions

Currently the common naming convention for fitacf files:

> YYYY.MM.DD.HH.mm.ss.<3-letter abbreviation>.fitacf.bz2

Some radars run stereo mode and separate the different channels by adding a letter to the file name:

> YYYY.MM.DD.HH.mm.ss.<3-letter abbreviation>.[a-d].bz2

For example, King Salmon was running stereo mode on 2019-02-01 as it can be shown in the file names `20190201.0401.00.ksr.a.fitacf.bz2` and `20190201.0401.00.ksr.b.fitacf.bz2`.
Each file contains data from a different frequency. 

!!! Note
    Sometimes modes like `twofsound` will write data in the same file but separate different frequencies by channels.  

## Versions 

RST is the main software used and maintained by the SuperDARN community that includes up incoming fitting algorithms. 
Some known fitting algorithms are:

- FITACF 2.0 (outdated algorithm)
- FITACF 2.5 (current default of RST 4.3.1)
- FITACF 3.0 (new fitting algorithm)

Other fitting algorithms that are not implemented in yet is:
- lmfit2 

## Fields

FITACF files contain a record that contains scalar and vector fields. 

### Scalars

| Field name              | Units            | Data Type    | Description                                                                             |
| :----------             | :-----:          | :-------:    | :---                                                                                    |
| *radar.revision.major*  | **None**         | ***char***   | Major version number of the radar operating system                                      |
| *radar.revision.minor*  | **None**         | ***char***   | Minor version number of the radar operating system                                      |
| *origin.code*           | **None**         | ***char***   | Code indicating origin of the data                                                      |
| *origin.time*           | **None**         | ***string*** | Text representing of when the data was generated                                        |
| *origin.command*        | **None**         | ***string*** | Command line used to generate the data                                                  |
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
| *smsep*                 | *us* | ***short***  | Sample seperation                                                                       |
| *ercod*                 | **None**         | ***short***  | Error Code                                                                              |
| *stat.agc*              | **None**         | ***short***  | AGC status word                                                                         |
| *stat.lopwr*            | **None**         | ***short***  | LOPWR status word                                                                       |
| *noise.search*          | **None**         | ***float***  | Calculated noise from clear frequency search                                            |
| *noise.mean*            | **None**         | ***float***  | Average Noise from clear frequency band                                                 |
| *channel*               | **None**         | ***short***  | Channel number for variant changes in radars parameters in one scan, ex) frequency      |
| *bmnum*                 | **None**         | ***short***  | Beam number                                                                             |
| *bmazm*                 | **None**         | ***float***  | Beam Azimuth                                                                            |
| *scan*                  | **None**         | ***short***  | Scan flag, switches between 0 to 1 or 0 to -1 to indicate a new scan                    |
| *offset*                | **None**         | ***short***  | Offset between channels of stereo radars                                                |
| *rxrise*                | *us* | ***short***  | Receiver rise time                                                                      |
| *intt.sc*               | *s*        | ***short***  | whole number of seconds of integration time                                             |
| *intt.us*               | *us* | ***int***    | Factional number of microseconds of integration time                                    |
| *txpl*                  | *us* | ***short***  | Transmitted pulse length                                                                |
| *mpinc*                 | *us* | ***short***  | Multi-pulse increment                                                                   |
| *mppul*                 | **None**         | ***short***  | Number of pulses in the sequence                                                        |
| *mplgs*                 | **None**         | ***short***  | Number of lags in the sequence                                                          |
| *mplgexes*              | **None**         | ***short***  |                                                                                         |
| *ifmode*                | **None**         | ***short***  | Interferometer mode                                                                     |
| *nrang*                 | **None**         | ***short***  | Number of range range gates                                                             |
| *frang*                 | *km*       | ***short***  | Distance to the first range gate                                                        |
| *rsep*                  | *km*       | ***short***  | Range seperation                                                                        |
| *xcf*                   | **None**         | ***short***  | XCF flag                                                                                |
| *tfreq*                 | *kHz*      | ***short***  | Transmitted frequency                                                                   |
| *mxpwr*                 | *Db*       | ***int***    | Maximum power                                                                           |
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


| Field name  | Units           | Dimensionality | Data Type   | Description                                                                 |
| :---------- | :-----:         | :-------:      | :---:       | :---                                                                        |
| *ptab*      | **None**        | *[mppul]*      | ***short*** | Pulse table                                                                 |
| *ltab*      | **None**        | *[ltab][2]*    | ***short*** | Lag table                                                                   |
| *pwr0*      | *Db*      | *[nrang]*      | ***float*** | Lag zero power                                                              |
| *slist*     | **None**        | *[num_pts]*    | ***short*** | List of range gates that obtained data points during the integration period |
| *nlag*      | **None**        | *[num_pts]*    | ***short*** | Number of points in the fit                                                 |
| *qflg*      | **None**        | *[num_pts]*    | ***char***  | Quality of fit flag for ACF                                                 |
| *gflg*      | **None**        | *[num_pts]*    | ***char***  | Ground scatter flag for ACF, 1 - ground scatter, 0 - other scatter          |
| *p_l*       | *Db*      | *[num_pts]*    | ***float*** | Power from lambda fit of ACF                                                |
| *p_l_e*     | *Db*      | *[num_pts]*    | ***float*** | Power error from lambda fit of ACF                                          |
| *p_s*       | *Db*      | *[num_pts]*    | ***float*** | Power from sigma fit of ACF                                                 |
| *p_s_e*     | *Db*      | *[num_pts]*    | ***float*** | Power error from sigma fit of ACF                                           |
| *v*         | *m/s* | *[num_pts]*    | ***float*** | velocity from ACF                                                           |
| *v_e* | *m/s* | *[num_pts]* |  ***float*** |Velocity error from ACF |
| *w_l* | *m/s* | *[num_pts]* | ***float*** | Spectral width from lambda fit of ACF | 
| *w_l_e* | *m/s* | *[num_pts]* | ***float*** | Spectral width error from lambda fit of ACF |
| *w_s* | *m/s* | *[num_pts]* | ***float*** | Spectral width from sigma fit of ACF | 
| *w_s_e* | *m/s* | *[num_pts]* | ***float*** | Spectral width error from sigma fit of ACF | 
| *sd_l* | **None** | *[num_pts]* | ***float*** | Standard deviation of lambda fit | 
| *sd_s* | **None** | *[num_pts]* | ***float*** | Standard deviation of sigma fit | 
| *sd_phi* | **None** | *[num_pts]* | ***float*** | Standard deviation of phase fit of ACF | 
| *x_qflg* | **None** | *[num_pts]* | ***char*** | Quality flag of XCF | 
| *x_gflg* | **None** | *[num_pts]* | ***char*** | Ground scatter flag of XCF | 
| *x_p_l* | *Db* | *[num_pts]* | ***float*** | Power from lamdba of XCF | 
| *x_p_l_e* | *Db* | *[num_pts]* | ***float*** | Power error from lambda fit of XCF | 
| *x_p_s* | *Db* | *[num_pts]* | ***float*** | Power from sigma fit of XCF | 
| *x_p_s_e* | *Db* | *[num_pts]* |  ***float*** | Power  error from sigma fit  of XCF | 
| *x_v* | *m/s* | *[num_pts]* | ***float*** | Velocity from XCF | 
| *x_v_e* | *m/s* | *[num_pts]* | ***float*** | Velocity error from XCF | 
| *x_w_l* | *m/s* | *[num_pts]* | ***float*** | Spectral width from lamdba fit of XCF | 
| *x_w_l_e* | *m/s* | *[num_pts]* | ***float*** | Spectral width error from lamdbad fit of XCF | 
| *x_w_s* | *m/s* | *[num_pts]* | ***float*** | Spectral wdith from sigma fit of XCF | 
| *x_w_s_e* | *m/s* | *[num_pts]* | ***float*** | Spectral wdith error from sigma fit of XCF | 
| *phi0* | **None** | *[num_pts]* | ***float*** | Phase determination at lag zero of the ACF | 
| *phi0_e* | **None** | *[num_pts]* | ***float*** | Phase determination error at lag zero of the ACF | 
| *elv* | *degrees* | *[num_pts]* | ***float*** | Elevation angle estimate |
| *elv_low* | *degrees* | *[num_pts]* | ***float*** | Lowest estimated of elevation angle |
| *elv_high* | *degrees* | *[num_pts]* | ***float*** | Highest estimate of elevation angle |
| *x_sd_l* | **None** | *[num_pts]* | ***float*** | Standard deviation of lambda fit of XCF|
| *x_sd_s* | **None** | *[num_pts]* | ***float*** | Standard deviation of sigma fit of XCF |
| *x_sd_phi* | **None** | *[num_pts]* | ***float*** | Standard deviation of phase fit of XCF |

## File structure

FITACF files are comprised of records that record data nominally every 3 seconds for each integration time.
Nominally each integration time will change the beam, `bmnum` indicates the beam number. However, some control programs like *camping mode* will stay on one beam for a whole scan. 
A scan is determined with the `scan` flag by the value. 1 or -1 inidicates the beginning of scan then the value changes to 0 for the rest the scan. When the `scan` value changes from 0 to 1 this indicates the end of the scan. Nominally scans try to fit in a 60 second window. Older FITACF files may show scan times of 120 seconds. Certain *control programs* could change the scan length as well especially newer upcoming radars. During  *normal scan* beams will scan increasing/decreasing from 0
to 15, however, radars that have more than 16 beams will sometimes start at other beams to meet the 3 second integration time and 60 second scan time. 

!!! Note 
    *Control programs* can change the beam pattern, `cp` will inidicate the *control program* numerical value and `combf` sometimes contains the *control programs* command/name. 

Uncommonly FITACF records can appear partial meaning there are missing **vector** fields. For example: 
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
This is a legacy behviour of RST that happens because of potentially "bad" data or no data was collected making `slist` empty. 
Currently this is ongoing investigation. 
