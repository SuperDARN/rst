# Makefile for test_raw
# =====================
# Author: R.J.Barnes#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = test_raw.o
SRC=test_raw.c errstr.h hlpstr.h
DSTPATH = $(BINPATH)
OUTPUT = test_raw
LIBS=-loldraw.1 -lraw.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 
SLIB=-lm -lz
 

include $(MAKEBIN).$(SYSTEM)
