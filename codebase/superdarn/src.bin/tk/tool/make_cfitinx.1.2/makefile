# Makefile for make_cfitinx
# =========================
# Author: R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_cfitinx.o
SRC=hlpstr.h errstr.h make_cfitinx.c
DSTPATH = $(BINPATH)
OUTPUT = make_cfitinx
LIBS=-lcfit.1 -lradar.1 -lopt.1 -lrcnv.1 -lrtime.1
SLIB=-lm -lz


include $(MAKEBIN).$(SYSTEM)
