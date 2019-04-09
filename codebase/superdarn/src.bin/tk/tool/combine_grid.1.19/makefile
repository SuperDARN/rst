# Makefile for combine_grid
# =========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = combine_grid.o make_grid.o
SRC=hlpstr.h errstr.h combine_grid.c make_grid.c make_grid.h
DSTPATH = $(BINPATH)
OUTPUT = combine_grid
LIBS=-lgrd.1 -loldgrd.1 -ldmap.1 -lradar.1 -lrfile.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz

 

include $(MAKEBIN).$(SYSTEM)
