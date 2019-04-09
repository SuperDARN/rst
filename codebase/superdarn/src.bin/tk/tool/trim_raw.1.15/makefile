# Makefile for trim_raw
# =====================
# Author: R.J.Barnes#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = trim_raw.o
SRC=hlpstr.h errstr.h trim_raw.c
DSTPATH = $(BINPATH)
OUTPUT = trim_raw
LIBS=-lradar.1 -loldraw.1 -lraw.1 -lopt.1 -ldmap.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
