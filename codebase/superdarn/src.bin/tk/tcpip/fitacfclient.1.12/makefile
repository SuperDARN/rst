# Makefile for fitacfclient
# =========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fitacfclient.o
SRC=hlpstr.h errstr.h fitacfclient.c

LIBS=-lfitcnx.1 -lfit.1 -lcfit.1 -lrscan.1 -lradar.1 -lcnx.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lz
DSTPATH = $(BINPATH)
OUTPUT = fitacfclient

include $(MAKEBIN).$(SYSTEM)
