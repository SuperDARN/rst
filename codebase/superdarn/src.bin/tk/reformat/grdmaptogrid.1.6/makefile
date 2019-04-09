# Makefile for grdmaptogrid
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = grdmaptogrid.o
SRC=hlpstr.h errstr.h grdmaptogrid.c
DSTPATH = $(BINPATH)
OUTPUT = grdmaptogrid
LIBS=-lgrd.1 -loldgrd.1 -lopt.1 -lrcnv.1 -ldmap.1 -lrcnv.1 -lrfile.1 -lrtime.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
