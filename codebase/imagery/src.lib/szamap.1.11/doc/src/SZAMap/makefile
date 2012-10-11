# Makefile for SZAMap
# ===================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/imagery
IGNVER=1
OBJS = SZAMap.o
OUTPUT = SZAMap
LIBS= -lszamap.1 -lsza.1 -lgmap.1 -lraster.1 -lcontour.1 -lrmap.1 -lpoly.1 -lrtime.1 -lrplot.1 -lrfbuf.1 -lrxml.1 -lrcnv.1 -laacgm.1
SLIB=-lm
include $(MAKELIB).$(SYSTEM)
