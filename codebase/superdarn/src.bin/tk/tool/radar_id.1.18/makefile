# Makefile for radar_id
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = radar_id.o
SRC=hlpstr.h errstr.h radar_id.c
DSTPATH = $(BINPATH)
OUTPUT = radar_id
LNK=$(ARC)
LIBS=-lradar.1 -lopt.1 -ldmap.1 -lrcnv.1 -lrtime.1
SLIB=-lm

 

include $(MAKEBIN).$(SYSTEM)
