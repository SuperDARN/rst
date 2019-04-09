# Makefile for cat_fit
# ====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = cat_fit.o
SRC=hlpstr.h errstr.h cat_fit.c
DSTPATH = $(BINPATH)
OUTPUT = cat_fit
LIBS=-lfit.1 -lcfit.1 -lradar.1 -loldfit.1 -lrscan.1 -lopt.1 -ldmap.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
