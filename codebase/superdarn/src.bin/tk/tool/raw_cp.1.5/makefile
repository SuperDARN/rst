# Makefile for raw_cp
# ===================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = raw_cp.o
SRC=hlpstr.h errstr.h raw_cp.c
DSTPATH = $(BINPATH)
OUTPUT = raw_cp
LIBS=-loldraw.1 -lraw.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lm -lz
 
include $(MAKEBIN).$(SYSTEM)
