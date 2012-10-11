# Makefile for shellserver
# ===================
# Author: R.J.Barnes

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = shellserver.o 
SRC=hlpstr.h errstr.h shellserver.c

DSTPATH = $(BINPATH)
OUTPUT = shellserver
LIBS=-ltcpipmsg.1 -lshmem.1 -lrtime.1 -lopt.1



ifeq ($(SYSTEM),linux)
  SLIB=-lm -lrt
else
  SLIB=-lm
endif


include $(MAKEBIN).$(SYSTEM)

