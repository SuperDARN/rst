# Makefile for make_vec
# =====================
# Author: R.J.Barnes
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
OBJS = make_vec.o

SRC=hlpstr.h errstr.h make_vec.c
DSTPATH = $(BINPATH)
OUTPUT = make_vec
LIBS=-lgrd.1 -loldgrd.1 -lradar.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1  
SLIB=-lm -lz

 

include $(MAKEBIN).$(SYSTEM)
