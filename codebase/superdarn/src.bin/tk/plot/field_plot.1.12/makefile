# Makefile for field_plot
# =======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
        -I$(IPATH)/imagery -I$(IPATH)/superdarn

OBJS = plot_outline.o plot_refvec.o expr.o field_plot.o

SRC = hlpstr.h expr.c expr.h \
      field_plot.c errstr.h plot_refvec.c plot_outline.c \
      plot_outline.h plot_refvec.h

DSTPATH = $(BINPATH)
OUTPUT = field_plot
LIBS = -loldfit.1 -lstdkey.1 -lbinplot.1 -lrpos.1 -lsnd.1 -lcfit.1 -lfit.1 \
       -lrscan.1 -lradar.1 -lsza.1 -lszamap.1 -ligrf.1 -laacgm.1 -lmlt.1 \
       -lmlt_v2.1 -laacgm_v2.1 -ligrf_v2.1 -lastalg.1 -lrmap.1 -lraster.1 \
       -lrtime.1 -lpoly.1 -ldmap.1 -lopt.1 -lrcnv.1 -lgrplot.1 -lcontour.1 \
       -lrfile.1 -lrtime.1 -lreval.1 -liplot.1 -lsplot.1 -lfontdb.1 \
       -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1
XLIB = -lxwin.1
SLIB = -lpng -lm -lz

include $(MAKEBIN).$(SYSTEM)
