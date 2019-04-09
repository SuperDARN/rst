# Makefile for dmapclient
# =======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general
OBJS = dmapclient.o
DSTPATH = $(BINPATH)

SRC=hlpstr.h errstr.h dmapclient.c
LIBS=-lcnx.1 -ldmap.1 -lopt.1 -lrcnv.1
OUTPUT=dmapclient
SLIB=-lz


include $(MAKEBIN).$(SYSTEM)
