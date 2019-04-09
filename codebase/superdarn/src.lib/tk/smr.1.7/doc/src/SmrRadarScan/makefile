# Makefile for SmrRadarScan
# =========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
IGNVER=1
OBJS = SmrRadarScan.o
OUTPUT = SmrRadarScan
LIBS=-lsmr.1 -lrscan.1  -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
