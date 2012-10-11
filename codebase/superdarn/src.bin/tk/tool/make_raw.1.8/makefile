# Makefile for make_raw
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_raw.o
SRC=hlpstr.h errstr.h make_raw.c
DSTPATH = $(BINPATH)
OUTPUT = make_raw
LIBS=-lacf.1 -lraw.1 -lradar.1 -liqdata.1 -lopt.1 -ldmap.1 -lrcnv.1 -lrtime.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
