# Makefile for fitacfclientscan
# =============================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fitacfclientscan.o downloadscan.o
SRC=hlpstr.h fitacfclientscan.c errstr.h downloadscan.c downloadscan.h
LIBS=-lfitcnx.1 -lfit.1 -lcfit.1 -lrscan.1 -lradar.1 -lcnx.1 -ldmap.1 -lrtime.1 -lrcnv.1 -lopt.1
SLIB=-lm -lz
DSTPATH = $(BINPATH)
OUTPUT = fitacfclientscan

include $(MAKEBIN).$(SYSTEM)
