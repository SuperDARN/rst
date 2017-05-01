# Makefile for make_fitacf.3.0
# =====================
# Original Author: R.J.Barnes
# by R.J.Barnes
#
# Modified by Keith Kotyk
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_fitacf.3.0.o

SRC=hlpstr.h errstr.h make_fit.c

DSTPATH = $(BINPATH)
OUTPUT = make_fitacf.3.0
LIBS=-loldraw.1 -loldfit.1 -lcfit.1 -lrscan.1 -lradar.1 -lfitacf.3.0 -lraw.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
