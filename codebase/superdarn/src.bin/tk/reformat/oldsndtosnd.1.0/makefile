# Makefile for oldsndtosnd
# ========================
# Author: E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = oldsndtosnd.o
SRC=hlpstr.h errstr.h oldsndtosnd.c
DSTPATH = $(BINPATH)
OUTPUT = oldsndtosnd
LIBS=-lradar.1 -lsnd.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)
