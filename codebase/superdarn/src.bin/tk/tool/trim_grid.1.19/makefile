# Makefile for trim_grid
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
OBJS = trim_grid.o
SRC=hlpstr.h errstr.h trim_grid.c
DSTPATH = $(BINPATH)
OUTPUT = trim_grid
LIBS=-lgrd.1 -loldgrd.1 -lradar.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1 
SLIB=-lm -lz

 

include $(MAKEBIN).$(SYSTEM)
