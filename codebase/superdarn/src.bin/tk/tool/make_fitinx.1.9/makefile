# Makefile for make_fitinx
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_fitinx.o
SRC=hlpstr.h errstr.h make_fitinx.c
DSTPATH = $(BINPATH)
OUTPUT = make_fitinx
LIBS=-loldfit.1 -lfit.1 -lcfit.1 -lrscan.1 -lradar.1 -lopt.1 -lrcnv.1 -ldmap.1 -lrtime.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
