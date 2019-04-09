#  Makefile for dmapdump
#  =====================
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general 
SRC=dmapdump.c hlpstr.h
OBJS =dmapdump.o
DSTPATH = $(BINPATH)
OUTPUT = dmapdump
LIBS= -lopt.1 -ldmap.1 -lrcnv.1
SLIB= -lm -lz

include $(MAKEBIN).$(SYSTEM)
