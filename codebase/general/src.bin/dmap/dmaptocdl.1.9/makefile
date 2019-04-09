# Makefile for dmaptocdl
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general 
OBJS =dmaptocdl.o
SRC=hlpstr.h errstr.h dmaptocdl.c
OBJS =dmaptocdl.o
DSTPATH = $(BINPATH)
OUTPUT = dmaptocdl
LIBS= -lopt.1 -ldmap.1 -lrcnv.1
SLIB= -lm -lz

include $(MAKEBIN).$(SYSTEM)
