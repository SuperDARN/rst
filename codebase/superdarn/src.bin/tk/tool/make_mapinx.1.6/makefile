# Makefile for make_mapinx
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_mapinx.o
SRC=hlpstr.h errstr.h make_mapinx.c
DSTPATH = $(BINPATH)
OUTPUT = make_mapinx
LIBS=-lcnvmap.1 -lgrd.1 -lopt.1 -lrcnv.1 -ldmap.1 -lrtime.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
