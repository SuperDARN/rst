# Makefile for fitfilter
# ===================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fitfilter.o
SRC=hlpstr.h errstr.h fitfilter.c
DSTPATH = $(BINPATH)
OUTPUT = fitfilter
LIBS=-loldfit.1 -lfilter.1 -lcfit.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
