# Makefile for make_efieldinx
# ===========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_efieldinx.o
SRC=hlpstr.h errstr.h make_efieldinx.c
DSTPATH = $(BINPATH)
OUTPUT = make_efieldinx
LIBS=-lopt.1 -ldmap.1 -lrcnv.1 -lrtime.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
