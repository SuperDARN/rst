# Makefile for make_rawinx
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_iqinx.o
SRC=hlpstr.h errstr.h make_iqinx.c
DSTPATH = $(BINPATH)
OUTPUT = make_iqinx
LIBS=-liqdata.1 -lradar.1 -lopt.1 -lrcnv.1 -ldmap.1 -lrtime.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
