# Makefile for make_snd
# =====================
# Author: E.G.Thomas
# by E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_snd.o
SRC=hlpstr.h errstr.h make_snd.c
DSTPATH = $(BINPATH)
OUTPUT = make_snd
LIBS= -lsnd.1 -lfit.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)
