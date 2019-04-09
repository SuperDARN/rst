# Makefile for trim_iq
# ====================
# Author: R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = trim_iq.o
SRC=hlpstr.h errstr.h trim_iq.c
DSTPATH = $(BINPATH)
OUTPUT = trim_iq
LIBS=-liqdata.1 -lopt.1 -ldmap.1 -lradar.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
