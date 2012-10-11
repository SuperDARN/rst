# Makefile for trim_fit
# =====================
# Author: R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = trim_fit.o
SRC=hlpstr.h errstr.h trim_fit.c
DSTPATH = $(BINPATH)
OUTPUT = trim_fit
LIBS=-lradar.1 -loldfit.1 -lfit.1 -lcfit.1 -lrscan.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
