# Makefile for map_grd
# ====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/superdarn 
SRC=hlpstr.h errstr.h map_grd.c
OBJS = map_grd.o

DSTPATH = $(BINPATH)
OUTPUT = map_grd

SLIB=-lm -lz
LIBS= -lcnvmap.1 -loldcnvmap.1 -loldgrd.1 -lgrd.1 -ldmap.1  \
      -lradar.1 -lrfile.1 -lrtime.1 \
      -lmlt.1 -laacgm.1 -lmlt_v2.1 -laacgm_v2.1 -ligrf_v2.1 \
      -lastalg.1 -lopt.1 -lrcnv.1 

include $(MAKEBIN).$(SYSTEM)
