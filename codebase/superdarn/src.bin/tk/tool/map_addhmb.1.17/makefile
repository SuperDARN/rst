# Makefile for map_addhmb
# =======================
# Author: R.J.Barnes and others
#
#
include $(MAKECFG).$(SYSTEM)


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/superdarn 
SRC=hlpstr.h map_addhmb.c errstr.h
OBJS = map_addhmb.o
DSTPATH = $(BINPATH)
OUTPUT = map_addhmb

SLIB=-lm -lz
LIBS= -lcnvmap.1 -loldcnvmap.1 -lgrd.1 -loldgrd.1 -lradar.1 \
      -lmlt.1 -laacgm.1 -lmlt_v2.1 -laacgm_v2.1 -ligrf_v2.1 \
      -lastalg.1 -lrtime.1 -ldmap.1 -lrfile.1 -lopt.1 -lrcnv.1 \
	  -lhmb.1

include $(MAKEBIN).$(SYSTEM)
