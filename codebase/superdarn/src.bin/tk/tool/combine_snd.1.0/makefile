# Makefile for combine_snd
# ========================
# Author: E.G.Thomas
# by E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = combine_snd.o
SRC=hlpstr.h errstr.h combine_snd.c
DSTPATH = $(BINPATH)
OUTPUT = combine_snd
LIBS= -lsnd.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)
