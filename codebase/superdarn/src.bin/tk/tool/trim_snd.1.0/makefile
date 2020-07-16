# Makefile for trim_snd
# =====================
# Author: E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = trim_snd.o
SRC=hlpstr.h errstr.h trim_snd.c
DSTPATH = $(BINPATH)
OUTPUT = trim_snd
LIBS=-lradar.1 -lsnd.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)
