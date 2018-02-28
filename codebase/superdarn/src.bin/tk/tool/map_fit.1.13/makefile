# Makefile for map_fit
# ====================
# Author: R.J.Barnes and others
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/superdarn
SRC = hlpstr.h errstr.h map_fit.c
OBJS = map_fit.o

DSTPATH = $(BINPATH)
OUTPUT = map_fit

SLIB= -lm -lz
LIBS= -lcnvmap.1 -lshf.1 -loldcnvmap.1 -lgrd.1 -loldgrd.1 -lradar.1 \
      -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1 -laacgm.1 \
	  -laacgm_v2.1 -ligrf_v2.1 -lastalg.1

include $(MAKEBIN).$(SYSTEM)
