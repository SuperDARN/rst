# Makefile for rawstat
# ====================
# Author: R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = rawstat.o
SRC=rawstat.c hlpstr.h errstr.h
DSTPATH = $(BINPATH)
OUTPUT = rawstat
LIBS=-loldraw.1 -lraw.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
