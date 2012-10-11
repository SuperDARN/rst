# Makefile for iqplot
# ===================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)


INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
        -I$(IPATH)/superdarn 
SRC=iqplot.c
OBJS = iqplot.o

SLIB=-lpng -lm -lz
LIBS=-liqdata.1 -lradar.1 -lrtime.1 -lopt.1 -ldmap.1 -lrcnv.1 -lgrplot.1 -liplot.1 -lfontdb.1 -limagedb.1 -lsplot.1 -lrps.1 -lrfbuf.1 -lrxml.1 
XLIB=-lxwin.1

ifdef XPATH
DSTPATH = $(BINPATH)
OUTPUT = iqplot
endif


include $(MAKEBIN).$(SYSTEM)
