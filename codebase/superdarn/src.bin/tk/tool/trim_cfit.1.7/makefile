# Makefile for trim_cfit
# ======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = trim_cfit.o
SRC=hlpstr.h errstr.h trim_cfit.c
DSTPATH = $(BINPATH)
OUTPUT = trim_cfit
LIBS=-lcfit.1 -lrscan.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
