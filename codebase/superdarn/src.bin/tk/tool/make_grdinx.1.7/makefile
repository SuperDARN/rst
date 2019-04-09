# Makefile for make_grdinx
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_grdinx.o
SRC=hlpstr.h errstr.h make_grdinx.c
DSTPATH = $(BINPATH)
OUTPUT = make_grdinx
LIBS=-lgrd.1 -lopt.1 -lrcnv.1 -ldmap.1 -lrtime.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
