# Makefile for RadarGetName
# ======================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/superdarn 
IGNVER=1
OBJS = RadarGetName.o
OUTPUT = RadarGetName
LIBS=-lradar.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
