# Makefile for CFitReadRadarScan
# ==============================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
IGNVER=1
OBJS = CFitReadRadarScan.o
OUTPUT = CFitReadRadarScan
LIBS=-lcfit.1 -lrscan.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
