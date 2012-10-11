# Makefile for iqwrite
# ====================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = iqwrite.o 
SRC=hlpstr.h errstr.h iqwrite.c

DSTPATH = $(BINPATH)
OUTPUT = iqwrite
LIBS=-lerrlog.1 -liqdata.1 -lfio.1 -lrmsgrcv.1 -ltcpipmsg.1 -lradar.1 -ldmap.1 -lrtime.1 -lrcnv.1 -lopt.1 -lshmem.1

ifeq ($(SYSTEM),linux)
  SLIB=-lm -lz -lrt
else
  SLIB=-lm -lz
endif




include $(MAKEBIN).$(SYSTEM)

