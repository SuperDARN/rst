# Makefile for field_plot
# =======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I$(IPATH)/imagery -I$(IPATH)/superdarn
OBJS = text_box.o plot_time_label.o plot_source.o plot_logo.o plot_time.o plot_cell.o plot_outline.o plot_vec.o plot_refvec.o make_fov.o make_grid.o key.o geobeam.o expr.o field_plot.o 

SRC=text_box.c text_box.h hlpstr.h plot_time.c plot_time.h key.c key.h expr.c expr.h field_plot.c errstr.h plot_source.c plot_source.h plot_vec.c plot_vec.h geobeam.c geobeam.h make_grid.c make_grid.h plot_cell.c plot_cell.h make_fov.c make_fov.h plot_time_label.c plot_time_label.h plot_refvec.c plot_outline.c plot_outline.h plot_refvec.h plot_logo.c plot_logo.h

DSTPATH = $(BINPATH)
OUTPUT = field_plot
LIBS=-loldfit.1  -lstdkey.1 -lrpos.1 -lcfit.1 -lfit.1 -lrscan.1  -lradar.1 -lsza.1 -lszamap.1 -ligrf.1 -laacgm.1 -lmlt.1 -lastalg.1 -lrmap.1 -lraster.1 -lrtime.1 -lpoly.1 -ldmap.1 -lopt.1 -lrcnv.1 -lgrplot.1 -lcontour.1 -lrfile.1 -lrtime.1 -lreval.1 -liplot.1 -lsplot.1 -lfontdb.1 -limagedb.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lrcnv.1 
XLIB=-lxwin.1
SLIB= -lpng -lm -lz

include $(MAKEBIN).$(SYSTEM)
