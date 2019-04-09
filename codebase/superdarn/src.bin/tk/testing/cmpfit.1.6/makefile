# Makefile for cmpfit
# ===================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS=fit_read_inx.o fit_open.o fit_close.o fit_read.o fit_read_current.o cmpfit.o

SRC=hlpstr.h fit.h fit_str.h lmt.h raw.h errstr.h cmpfit.c fit_open.c fit_read.c fit_read.h fit_versions.h fit_close.c fit_read_current.c fit_read_inx.c fit_read_inx.h

DSTPATH = $(BINPATH)
OUTPUT = cmpfit
LIBS= -lrcnv.1 -lrtime.1 -lopt.1
SLIB= -lm -lz

 

include $(MAKEBIN).$(SYSTEM)
