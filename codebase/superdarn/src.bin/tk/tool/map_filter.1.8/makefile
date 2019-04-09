# Makefile for map_filter
# =======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = map_filter.o 
SRC= map_filter.c hlpstr.h
DSTPATH = $(BINPATH)
OUTPUT = map_filter
LIBS= -lcnvmap.1 -loldcnvmap.1 -lgrd.1 -loldgrd.1 -ldmap.1 -lrfile.1 -lrtime.1 -lrcnv.1 -lopt.1  
SLIB= -lm -lz

 

include $(MAKEBIN).$(SYSTEM)
