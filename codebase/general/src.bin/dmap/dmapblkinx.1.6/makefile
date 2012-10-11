#  Makefile for dmapblkinx
#  =======================
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general 
SRC=dmapblkinx.c hlpstr.h
OBJS =dmapblkinx.o
DSTPATH = $(BINPATH)
OUTPUT = dmapblkinx
LIBS= -lopt.1 -ldmap.1 -lrcnv.1
SLIB= -lm -lz

include $(MAKEBIN).$(SYSTEM)
