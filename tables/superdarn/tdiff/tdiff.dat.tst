# Filename: tdiff.dat.tst
# Description: Sample phase calibration values (tdiff)
#
# References
#-------------------------------------------------------------------------------
# 0: Ponomarenko, et al. (2015), doi:10.1186/s40623-015-0310-3.
# 1: Burrell, et al. (2016), doi:10.1002/2016RS006089.
# 2: Ponomarenko, et al. (2018), doi:10.1029/2018rs006638.
# 3: Chisham, (2018), doi:10.1029/2017rs006492.
# 4: Chisham, et al. (2021), doi:10.1016/j.polar.2021.100638.
#-------------------------------------------------------------------------------
#
# Values
#-------------------------------------------------------------------------------
# M: Reference number from the section above
# C: Radar channel number
# FBAND_MIN/MAX: Transmission frequency band limits in kHz
# SDATE/EDATE: Starting/ending date (YYYYMMDD)
# STIME/ETIME: Starting/ending time of day (HH:MM:SS)
# TDIFF: Estimated tdiff (microsec)
# TDIFF_ERR: Uncertainty in tdiff (microsec)
# NPNTS: Number of points used in calibration (0, 1, 2) or median (3, 4)
# VAL: Has tdiff estimate been validated? 1=True, 0=False
#-------------------------------------------------------------------------------
#
# Comments
#-------------------------------------------------------------------------------
# Inline comments provide more information about reasons behind gaps in data or
# changes in tdiff estimates
#-------------------------------------------------------------------------------
#M C FBAND_MIN FBAND_MAX SDATE STIME EDATE ETIME TDIFF TDIFF_ERR NPNTS VAL
0  1  9900  9985 19951113 00:00:00 19951207 00:00:00 0.013 0.001 36533 1
0  2  9350 11275 19951115 00:00:00 19951207 00:00:00 0.019 0.003  6788 1
0  1 12370 12415 19951115 00:00:00 19951207 00:00:00 0.030 0.001  2491 1
0  1 15010 15080 19951115 00:00:00 19951207 00:00:00 0.057 0.005  1129 1
# Hardware tdiff change after 1995-12-07 00:00:00
0  1  9900  9985 19951207 09:00:00 19960217 23:59:59 0.125 0.003  4807 1
# Instrument changes alter tdiff on evening of 1996-02-17
1  2 19415 19680 19970421 00:00:00 19990306 15:25:00 0.178 0.001   280 1
# Test values (EGT)
4  1  8000 20000 19990306 00:00:00 20991231 23:59:59 0.178 0.001   280 1
4  2  8000 20000 19990306 00:00:00 20991231 23:59:59 0.150 0.001   280 1
