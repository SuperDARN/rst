# Makefile for fittoascii
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fittoascii.o
SRC=fittoascii.c
DSTPATH = $(BINPATH)
OUTPUT = fittoascii
LIBS=-lrscan.1 -lradar.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 -lrmath.1 -liqdata.1 -lrpos.1 -laacgm.1 -ligrf.1
SLIB=-lm -lz


include $(MAKEBIN).$(SYSTEM)
