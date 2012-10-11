# Makefile for make_fitex
# =======================
# Author: R.J.Barnes
#
#

include $(MAKECFG).$(SYSTEM)


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I$(USR_IPATH)/superdarn
OBJS = make_fitex.o

SRC=hlpstr.h errstr.h make_fitex.c
DSTPATH=$(USR_BINPATH)
OUTPUT = make_fitex
LIBS=-loldraw.1 -loldfit.1 -lcfit.1 -lrscan.1 -lfitacfex.1 -lraw.1 -lfit.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrmath.1 -lrcnv.1 
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)

 

