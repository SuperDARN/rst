# Makefile for fit_speck_removal
# =====================
# Authors: E.C.Bland / R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fit_speck_removal.o
SRC=hlpstr.h errstr.h fit_speck_removal.c
DSTPATH = $(BINPATH)
OUTPUT = fit_speck_removal
LIBS=-lradar.1 -lfit.1 -ldmap.1 -lopt.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
