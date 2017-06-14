# Makefile for RadarLoad
# ======================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/superdarn 
IGNVER=1
OBJS = RadarLoad.o
OUTPUT = RadarLoad
LIBS=-lradar.1 -lrtime.1 -lrcnv.1 
SLIB=-lm
 
include $(MAKELIB).$(SYSTEM)
