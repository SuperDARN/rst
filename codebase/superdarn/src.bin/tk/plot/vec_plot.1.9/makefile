# Makefile for vec_plot
# =====================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/superdarn

SRC = vec_plot.c hlpstr.h errstr.h plot_radial.c \
	  plot_radial.h plot_circle.c plot_circle.h

OBJS = plot_circle.o plot_radial.o vec_plot.o

DSTPATH = $(BINPATH)
OUTPUT = vec_plot
LIBS = -lmlt_v2.1 -laacgm_v2.1 -ligrf_v2.1 -lmlt.1 -laacgm.1 -lastalg.1 -ligrf.1 \
       -lrmap.1 -lpoly.1 -lrpos.1 -lradar.1 -ldmap.1 -lrtime.1 -lopt.1 \
       -lrcnv.1 -lbinplot.1 -lsplot.1 -liplot.1 -lfontdb.1 -lrps.1 -lrfbuf.1 -lrxml.1
XLIB = -lxwin.1
SLIB = -lpng -lm -lz

include $(MAKEBIN).$(SYSTEM)
