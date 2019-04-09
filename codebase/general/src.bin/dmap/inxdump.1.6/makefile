#  Makefile for inxdump
#  ====================
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general 
SRC=inxdump.c hlpstr.h
OBJS =inxdump.o
DSTPATH = $(BINPATH)
OUTPUT = inxdump

LIBS= -lopt.1 -lrtime.1 -lrcnv.1
SLIB= -lm -lz

include $(MAKEBIN).$(SYSTEM)
