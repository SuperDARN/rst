# Makefile for fov_plot
# =====================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
          -I$(IPATH)/imagery -I$(IPATH)/superdarn 
 
SRC = hlpstr.h fov_plot.c errstr.h
 
OBJS = fov_plot.o  
DSTPATH = $(BINPATH)
OUTPUT = fov_plot
LIBS = -lbinplot.1 -lrpos.1 -lradar.1 -lsza.1 -lszamap.1 -laacgm.1 -lmlt.1 -lastalg.1 \
       -ligrf.1 -lmlt_v2.1 -laacgm_v2.1 -ligrf_v2.1 -lrmap.1 -lcontour.1 \
       -lraster.1 -lrtime.1 -lpoly.1 -lopt.1 -lrcnv.1 -lrfile.1 -lrtime.1 \
       -ldmap.1 -lsplot.1 -liplot.1 -lfontdb.1 -lrps.1 -lrfbuf.1 -lrxml.1
XLIB = -lxwin.1
SLIB = -lpng -lm -lz

include $(MAKEBIN).$(SYSTEM)
