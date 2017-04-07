# Makefile for test_cfit
# ======================
# Author: R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = test_cfit.o
SRC=hlpstr.h errstr.h test_cfit.c
DSTPATH = $(BINPATH)
OUTPUT = test_cfit
LIBS=-lcfit.1 -lrscan.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
