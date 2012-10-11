# Makefile for fitacfwrite
# ===================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = fitacfwrite.o 
SRC=hlpstr.h errstr.h fitacfwrite.c

DSTPATH = $(BINPATH)
OUTPUT = fitacfwrite
LIBS=-lerrlog.1 -lfit.1 -lcfit.1 -lrscan.1 -lfio.1 -lrmsgrcv.1 -ltcpipmsg.1 -lradar.1 -ldmap.1 -lrtime.1 -lrcnv.1 -lopt.1
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)

