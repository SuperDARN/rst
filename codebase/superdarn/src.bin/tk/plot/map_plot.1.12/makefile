# Makefile for map_plot
# =====================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
          -I$(IPATH)/imagery -I$(IPATH)/superdarn -I$(IPATH)/

SRC = plot_extra.c plot_extra.h plot_model.c plot_model.h \
      plot_model_name.c plot_model_name.h hlpstr.h \
      errstr.h plot_imf_delay.c plot_imf_delay.h \
      map_plot.c plot_excluded.c plot_excluded.h \
      plot_chi.c plot_chi.h plot_maxmin.c plot_maxmin.h plot_fit.c plot_fit.h \
      plot_imf.c plot_imf.h make_vgrid.c make_vgrid.h \
      make_pgrid.c make_pgrid.h make_hmb.c make_hmb.h

OBJS = plot_chi.o plot_extra.o plot_imf_delay.o plot_imf.o \
       plot_model_name.o plot_maxmin.o plot_excluded.o plot_model.o \
       plot_fit.o make_hmb.o make_pgrid.o make_vgrid.o map_plot.o 

DSTPATH = $(BINPATH)
OUTPUT = map_plot
LIBS = -lbinplot.1 -lrpos.1 -lcnvmap.1 -loldcnvmap.1 -lshf.1 -lgrd.1 -loldgrd.1 -lradar.1 \
       -lsza.1 -lszamap.1 -ligrf.1 -lmlt_v2.1 -laacgm_v2.1 -ligrf_v2.1 \
       -laacgm.1 -lmlt.1 -lastalg.1 -lrmap.1 -lraster.1 -lrtime.1 -lpoly.1 \
       -lopt.1 -lrcnv.1 -lgrplot.1 -lcontour.1 -ldmap.1 -lrfile.1 -lrtime.1 \
       -lrxml.1 -liplot.1 -lsplot.1 -lfontdb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lrcnv.1 
XLIB = -lxwin.1
SLIB = -lpng -lm -lz

include $(MAKEBIN).$(SYSTEM)
