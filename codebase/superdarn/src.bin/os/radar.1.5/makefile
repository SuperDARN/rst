# Makefile for radar
# ===================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = radar.o 
SRC=hlpstr.h errstr.h radar.c

DSTPATH = $(BINPATH)
OUTPUT = radar
LIBS=-ltsg.1 -ltcpipmsg.1 -lrtime.1 -lopt.1
SLIB=-lm

include $(MAKEBIN).$(SYSTEM)

