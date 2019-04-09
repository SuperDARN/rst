# Makefile for merge_grid
# =======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
OBJS = merge_grid.o
DSTPATH = $(BINPATH)
OUTPUT = merge_grid
SRC=hlpstr.h errstr.h merge_grid.c
LIBS=-lgrd.1 -loldgrd.1 -ldmap.1 -lrfile.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lm -lz

 

include $(MAKEBIN).$(SYSTEM)
