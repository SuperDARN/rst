# Makefile for make_info
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)


SRC=hlpstr.h errstr.h make_info.c
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_info.o 
DSTPATH = $(BINPATH)
OUTPUT = make_info
LIBS= -loldraw.1 -lraw.1 -loldfit.1 -lfit.1 -lcfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lrcnv.1 -lrtime.1 -lopt.1
SLIB= -lm -lz

 

include $(MAKEBIN).$(SYSTEM)
