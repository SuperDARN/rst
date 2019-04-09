# Makefile for grid_filter
# ========================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h grid_filter.c

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = grid_filter.o 
SRC=grid_filter.c hlpstr.h
DSTPATH = $(BINPATH)
OUTPUT = grid_filter
LIBS= -lgrd.1 -loldgrd.1 -lrfile.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB= -lm -lz

 

include $(MAKEBIN).$(SYSTEM)
