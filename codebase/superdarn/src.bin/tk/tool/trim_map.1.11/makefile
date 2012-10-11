# Makefile for trim_map
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I$(USR_IPATH)/analysis
SRC=hlpstr.h errstr.h trim_map.c
OBJS = trim_map.o

DSTPATH = $(BINPATH)
OUTPUT = trim_map

LIBS=-lcnvmap.1 -loldcnvmap.1 -lgrd.1 -loldgrd.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1
SLIB=-lm -lz

 

include $(MAKEBIN).$(SYSTEM)
