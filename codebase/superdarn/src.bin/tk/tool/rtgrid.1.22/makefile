# Makefile for rtgrid
# ===================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/analysis -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -DPOSIX -D__EXTENSIONS__

OBJS = rtgrid.o loginfo.o 
SRC=hlpstr.h errstr.h rtgrid.c loginfo.c loginfo.h output.h

LIBS=-lfitcnx.1 -lfit.1 -lcfit.1 -lcnx.1 -lgtabw.1 -loldgtabw.1 -lgtable.1 -lfilter.1 -lrscan.1 -lrpos.1 -lradar.1 -laacgm.1 -ligrf.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1
SLIB=-lm -lz
DSTPATH = $(BINPATH)
OUTPUT = rtgrid

include $(MAKEBIN).$(SYSTEM)
