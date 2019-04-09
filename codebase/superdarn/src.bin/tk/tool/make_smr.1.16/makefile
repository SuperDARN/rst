# Makefile for make_smr
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_smr.o
SRC=hlpstr.h errstr.h make_smr.c
DSTPATH = $(BINPATH)
OUTPUT = make_smr
LIBS=-loldfit.1 -lfit.1 -lsmr.1 -lcfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
