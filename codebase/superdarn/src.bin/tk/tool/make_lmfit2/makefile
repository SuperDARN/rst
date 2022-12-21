# Makefile for make_lmfit2
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_lmfit2.o

SRC=hlpstr.h errstr.h make_lmfit2.c

DSTPATH = $(BINPATH)
OUTPUT = make_lmfit2
LIBS= -loldraw.1 -loldfit.1 -lcfit.1 -lrscan.1 -lradar.1 -llmfit.2.0 -lmpfit.1 -lraw.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1
SLIB=-lm -lz -lpthread
 

include $(MAKEBIN).$(SYSTEM)
