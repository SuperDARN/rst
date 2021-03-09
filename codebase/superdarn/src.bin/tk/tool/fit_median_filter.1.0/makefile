# Makefile for fit_median_filter 
# =====================
# Authors: E.C.Bland / R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fit_median_filter.o
SRC=hlpstr.h errstr.h fit_median_filter.c
DSTPATH = $(BINPATH)
OUTPUT = fit_median_filter
LIBS=-lradar.1 -lfit.1 -ldmap.1 -lopt.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
