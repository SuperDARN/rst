# Makefile for map_addmodel
# =========================
# Author: R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)


INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
          -I$(IPATH)/superdarn
SRC = hlpstr.h map_addmodel.c errstr.h
OBJS = map_addmodel.o

DSTPATH = $(BINPATH)
OUTPUT = map_addmodel

SLIB = -lm -lz
LIBS = -lcnvmodel.1 -lcnvmap.1 -loldcnvmap.1 -lgrd.1 -loldgrd.1 -lradar.1 -lshf.1 -laacgm.1 \
       -lhmb.1 -lmlt.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1 \
       -lmlt_v2.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1

include $(MAKEBIN).$(SYSTEM)
