# Makefile for rawacfwrite
# ===================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = rawacfwrite.o 
SRC=hlpstr.h errstr.h rawacfwrite.c

DSTPATH = $(BINPATH)
OUTPUT = rawacfwrite
LIBS=-lerrlog.1 -lraw.1 -lfio.1 -lrmsgrcv.1 -ltcpipmsg.1 -lradar.1 -ldmap.1 -lrtime.1 -lrcnv.1 -lopt.1
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)

