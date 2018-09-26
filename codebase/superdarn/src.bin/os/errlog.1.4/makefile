# Makefile for errlog
# ===================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = errlog.o 
SRC=hlpstr.h errstr.h errlog.c

DSTPATH = $(BINPATH)
OUTPUT = errlog
LIBS=-ltcpipmsg.1 -lrtime.1 -lopt.1
SLIB=-lm

include $(MAKEBIN).$(SYSTEM)

