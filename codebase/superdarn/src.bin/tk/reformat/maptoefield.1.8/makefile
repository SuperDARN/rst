# Makefile for maptoefield
# ========================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
		-I$(IPATH)/superdarn 
SRC = hlpstr.h errstr.h maptoefield.c make_pgrid.c make_pgrid.h
OBJS = maptoefield.o make_pgrid.o

DSTPATH = $(BINPATH)
OUTPUT = maptoefield

SLIB= -lm -lz
LIBS= -lcnvmap.1 -loldcnvmap.1 -lshf.1 -lgrd.1 -loldgrd.1 -laacgm.1 \
      -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1 -laacgm_v2.1 \
      -ligrf_v2.1 -lastalg.1

include $(MAKEBIN).$(SYSTEM)
