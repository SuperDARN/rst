<!-- Copyright (C) 2020 VT SuperDARN, Virginia Polytechnic Institute and State University
author(s): Kevin Sterne

Disclaimer: License under GNU v3.0, the file is found in the root directory under LICENSE 

-->

# Compressed FitACF (cFit) data format

The cFit format is a compressed format for storing a limited sub-set of data from the output of the FitACF algorithm.  The format does not store all of the radar operation parameters or the full set of derived values of the algorithm. Instead it stores sufficient parameters to characterize the operation of the radar, as well as the power, velocity, and spectral width parameters. The advantage of cFit files is that they are considerably smaller than fit or fitacf files. Typically a cFit file is a few megabytes in length, whereas a fit file can be over 100 megabytes.


## The Format

A cFit file is divided into blocks or records. Each record has the same format but can have varying length depending on the amount of scatter observed by the radar. Each record is divided into a header and a data section. The structure of a record is given below:

| Byte Offset | Size (Bytes) | Type | Content |
| :---        |  :----:      | :--- | :---   |
| 0      | 8  | 64-bit double float  | Time in seconds form UNIX epoch (00:00:00 UTC, Jan. 1, 1970) |
| 8      | 2  | 16-bit short integer | Station identifier |
| 10     | 2  | 16-bit short integer | Scan flag          |
| 12     | 2  | 16-bit short integer | Program identifier |
| 14     | 2  | 16-bit short integer | Beam number        |
| 16     | 4  | 32-bit float         | Beam azimuth       |
| 20     | 2  | 16-bit short integer | Channel            |
| 22     | 2  | 16-bit short integer | Integral component of integration time in seconds |
| 24     | 4  | 32-bit integer   | Fractional component of integration time in microseconds |
| 28     | 2  | 16-bit short integer | Distance to first range in kilometers |
| 30     | 2  | 16-bit short integer | Range separation in kilometers |
| 32     | 2  | 16-bit short integer | Receiver rise time in microseconds |
| 34     | 2  | 16-bit short integer | Operating frequency in kiloHertz |
| 36     | 4  | 32-bit integer       | Noise level |
| 40     | 2  | 16-bit short integer | Attenuation |
| 42     | 2  | 16-bit short integer | Number of averages |
| 44     | 2  | 16-bit short integer | Number of range gates |
| 46     | 1  | byte                 | Number of stored ranges |
| 47     | m  | m bytes              | Range table    |
| 47+m   | n  | n bytes              | Data table     |

The remainder of the block consists of the data values for the stored ranges.  The range table indicates which range gate a data value corresponds to.  For example, if the first entry in the range table contains a value of 23, that indicates that the first set of data values in the data table came from range gate 23.

Each entry in the data table has the same format shown below:

| Byte Offset | Size (Bytes) | Type  | Content |
| :----       |  :------:    | :---- | :-----  |
| 0      | 1   | byte          | Ground scatter flag    |
| 1      | 4   | 32-bit float  | Lag-zero power         |
| 5      | 4   | 32-bit float  | Lag-zero power error   |
| 9      | 4   | 32-bit float  | Velocity               |
| 13     | 4   | 32-bit float  | Lambda power           |
| 17     | 4   | 32-bit float  | Lambda spectral width  |
| 21     | 4   | 32-bit float  | Velocity error         |
| 25     | 4   | 32-bit float  | Lambda power error     |
| 29     | 4   | 32-bit float  | Lambda spectral width error |


## References

- This information is sourced from the RFC: 0007 previously in the RST RFC documentation that was written by R.J. Barnes.

## History (from RFC)

- 2007/03/26 Update to reflect changes to file format, R.J. Barnes
- 2004/07/27 Initial revision,  R.J. Barnes

