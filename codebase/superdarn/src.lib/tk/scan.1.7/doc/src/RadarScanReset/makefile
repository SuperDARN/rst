# Makefile for RadarScanReset
# ===========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
IGNVER=1
OBJS = RadarScanReset.o
OUTPUT = RadarScanReset
LIBS=-lfit.1 -lrscan.1  -ldmap.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
