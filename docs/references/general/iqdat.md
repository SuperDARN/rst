<!--
(C) copyright 2020 VT SuperDARN, Virginia Polytechnic Institute & State University
author: Kevin Sterne

Modifications:
    2022-11-28 Emma Bland (UNIS) Updated file format description
-->

# IQDat files

IQDat files are a level-zero type data product that reflect the ADC voltages sampled at the radar site.

## Naming Convention

The community standard for naming IQDAT files is:

> YYYYMMDD.HH.mm.ss.<3-letter radar code>.[a-d].iqdat

The field [a-d] is used when the data have been separated into multiple files based on a particular operating parameter (e.g. frequency, beam pattern, control program). This field may not be used if all the available data are provided in a single file.


## Fields

IQDat files are composed of records that contain scalar and vector fields.

### Scalars

| Field name              | Units    | Data Type | Description                                                                             |
| :----------             | :-----:  | :-------: | :---             |
| *radar.revision.major*  | **None** | char      | Major version number of the radar operating system.       |
| *radar.revision.minor*  | **None** | char      | Minor version number of the radar operating system.       |
| *origin.code*           | **None** | char      | Code indicating origin of the data.                       |
| *origin.time*           | **None** | string    | ASCII representation of when the data was generated.      |
| *origin.command*        | **None** | string    | The command line or control program used to generate the data.   |
| *cp*                    | **None** | short     | Control program identifier.                            |
| *stid*                  | **None** | short     | Station identifier.            |
| *time.yr*               | **None** | short     | Year.                          |
| *time.mo*               | **None** | short     | Month number (not padded)      |
| *time.dy*               | **None** | short     | Day (not padded)               |
| *time.hr*               | *hrs*    | short     | Hour                           |
| *time.mt*               | *min*    | short     | Minute.                        |
| *time.sc*               | *s*      | short     | Second.                        |
| *time.us*               | *&mu;s*  | short     | Microseconds                   |
| *txpow*                 | *kW*     | short     | Transmitted power              |
| *nave*                  | **None** | short     | Number of pulse sequences transmitted.    |
| *atten*                 | **None** | short     | Attenuation level.             |
| *lagfr*                 | *&mu;s*  | short     | Lag to first range             |
| *smsep*                 | *&mu;s*  | short     | Sample separation              |
| *ercod*                 | **None** | short     | Error code                     |
| *stat.agc*              | **None** | short     | Automatic gain control status word.   |
| *stat.lopwr*            | **None** | short     | Low power status word          |
| *noise.search*          | **None** | float     | Calculated noise from clear frequency search.  |
| *noise.mean*            | **None** | float     | Average noise across frequency band.  |
| *channel*               | **None** | short     | Channel number, used to denote different Tx/Rx channels on Stereo radars, and to denote changes in radar operating parameters between scans, e.g. alternating between 2 frequency bands scan-to-scan    |
| *bmnum*                 | **None** | short     | Beam number, zero based indexing      |
| *bmazm*                 | *degrees* | float    | Beam azimuth                   |
| *scan*                  | **None** | short     | Scan flag.                     |
| *offset*                | **None** | short     | Offset between channels for a stereo radar (zero for all others).  |
| *rxrise*                | *&mu;s*  | short     | Receiver rise time.            |
| *intt.sc*               | *s*      | short     | Whole number of seconds of integration time. |
| *intt.us*               | *&mu;s*  | short     | Fractional number of microseconds of integration time.  | 
| *txpl*                  | *&mu;s*  | short     | Transmit pulse length.         |
| *mpinc*                 | *&mu;s*  | short     | Multi-pulse increment.         | 
| *mppul*                 | **None** | short     | Number of pulses in sequence.  |
| *mplgs*                 | **None** | short     | Number of lags in sequence.    |
| *mplgexs*               | **None** | short     | Number of lags used for tauscan.  This field is stored for fit-level processing.   |
| *ifmode*                | **None** | short     | A flag indicating that the receiver voltage samples were obtained by digitizing the signal at an intermediate frequency (and then digitally mixed to baseband) instead of directly sampling the RF signals (`ifmode=0`).
| *nrang*                 | **None** | short     | Number of ranges.              |
| *frang*                 | *km*     | short     | Distance to first range.       |
| *rsep*                  | *rsep*   | short     | Range separation.              |
| *xcf*                   | **None*  | short     | XCF flag.                      |
| *tfreq*                 | *kHz*    | short     | Transmitted frequency.         |
| *mxpwr*                 | *dB*     | int       | Maximum power.                 |
| *lvmax*                 | **None** | int       | Maximum noise level allowed.   |
| *iqdata.revision.major* | **None** | int       | Major version number of the iqdata library.   |
| *iqdata.revision.minor* | **None** | int       | Minor version number of the iqdata library.   |
| *combf*                 | **None** | string    | Comment buffer.                |
| *seqnum*                | **None** | int       | Number of pulse sequences transmitted.   |
| *chnnum*                | **None** | int       | Number of channels sampled (both I and Q quadrature samples).   |
| *smpnum*                | **None** | int       | Number of samples taken per sequence.    |
| *skpnum*                | **None** | int       | Number of samples to skip before the first valid sample.   |


### Arrays

| Field name              | Units    | Dimensionality |  Data Type | Description                                                          |
| :----------             | :-----:  | :------------: | :-------:  | :---             |
| *ptab*                  | **None** | *[mppul]*      | short      | Pulse table.     |
| *ltab*                  | **None** | *[2,mplgs]*    | short      | Lag table.       |
| *tsc*                   | *s*      | *[seqnum]*     | int        | Seconds component of time past epoch of pulse sequence.   |
| *tus*                   | *&mu;s*  | *[seqnum]*     | int        | Microsecond component of time past epoch of pulse sequence.   |
| *tatten*                | **None** | *[seqnum]*     | short      | Attenuator setting for each pulse sequence.               |
| *tnoise*                | **None** | *[seqnum]*     | float      | Noise value for each pulse sequence.                      |
| *toff*                  | **None** | *[seqnum]*     | int        | Offset into the sample buffer for each pulse sequence.    |
| *tsze*                  | **None** | *[seqnum]*     | int        | Number of words stored for this pulse sequence.           |
| *tbadtr*                | **None** | *[seqnum]*     | int        |      |
| *badtr*                 | **None** | *[2\*mppul\*seqnum]* | int    |      |
| *data*                  | **None** | *[totnum]*     | int        | Array of raw I and Q samples.                             |

The extent of each dimension is determined by the radar operating parameters. For example a value of `mppul` of 7 would result in the array `ptab[7]`.

The raw sample data is arranged in multiplexed I and Q sample order; so the first *smpnum\*2* samples are the interleaved I and Q values for the first pulse sequence. If more than one channel is samples, the next *smpnum\*2* samples represent the next channel. The total number of samples, *totnum*, is equal to *2\*seqnum\*chnnum\*smpnum*.

