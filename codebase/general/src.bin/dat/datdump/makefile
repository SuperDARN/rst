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
LIBS= -lopt.1 -ldmap.1 -loldraw.1 -lrcnv.1 -lraw.1 -lradar.1 -lrtime.1
SLIB= -lm -lz

include $(MAKEBIN).$(SYSTEM)
