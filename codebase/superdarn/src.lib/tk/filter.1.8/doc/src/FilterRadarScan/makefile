# Makefile for FilterRadarScan
# ============================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
IGNVER=1
OBJS = FilterRadarScan.o
OUTPUT = FilterRadarScan
LIBS= -lfilter.1  -lcfit.1 -lfit.1 -lrscan.1 -lrpos.1 -lradar.1 -lrtime.1 -ldmap.1 -lrcnv.1 -laacgm.1 -ligrf.1  
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
