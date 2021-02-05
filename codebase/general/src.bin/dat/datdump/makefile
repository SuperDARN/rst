# Copyright  (C) SuperDARN Canada, University of Saskatchewan
# Author: Marina Schmidt 
#
# Modification:
#
# Disclaimer 
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn 
SRC=datdump.c hlpstr.h
OBJS =datdump.o
DSTPATH = $(BINPATH)
OUTPUT = datdump
LIBS= -g -lopt.1 -ldmap.1 -ldat.1 -lrcnv.1 -lrtime.1
SLIB= -lm -lz

include $(MAKEBIN).$(SYSTEM)
