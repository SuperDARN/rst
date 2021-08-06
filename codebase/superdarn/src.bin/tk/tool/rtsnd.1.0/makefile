# Makefile for rtsnd
# ==================
# Author: E.G.Thomas
# by E.G.Thomas
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
        -DPOSIX -D__EXTENSIONS__

OBJS=loginfo.o rtsnd.o
SRC=hlpstr.h errstr.h rtsnd.c loginfo.c loginfo.h
LIBS=-lfitcnx.1 -lcnx.1 -lsnd.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lrtime.1 -lopt.1 -lrcnv.1  
SLIB=-lm -lz
DSTPATH = $(BINPATH)
OUTPUT = rtsnd

include $(MAKEBIN).$(SYSTEM)
