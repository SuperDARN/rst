# Makefile for cat_raw
# ====================
# Author: R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = cat_raw.o
SRC=hlpstr.h errstr.h cat_raw.c
DSTPATH = $(BINPATH)
OUTPUT = cat_raw
LIBS=-lradar.1 -lraw.1 -loldraw.1 -lopt.1 -ldmap.1 -lrtime.1 -lrcnv.1  
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
