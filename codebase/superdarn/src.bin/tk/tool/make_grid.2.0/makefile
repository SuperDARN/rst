# Makefile for make_grid
# ======================
# Author: R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/analysis -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS = make_grid.o
SRC=hlpstr.h errstr.h make_grid.c
DSTPATH = $(BINPATH)
OUTPUT = make_grid
LIBS=-loldfit.1 -lgtabw.1 -loldgtabw.1 -lgtable.1 -lrpos.1 -lfilter.1 -lcfit.1 -lfit.1 -lrscan.1 -lradar.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 -laacgm.1 -ligrf.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1 -lchannel.1
SLIB=-lm -lz

include $(MAKEBIN).$(SYSTEM)
