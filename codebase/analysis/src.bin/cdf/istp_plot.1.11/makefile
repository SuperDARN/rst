#  Makefile for istp_plot.c
#  ========================
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(CDF_PATH)/include \
        -I$(IPATH)/analysis
OBJS = wind.o ace.o imp.o geotail.o locate.o graphic.o istp_plot.o 
SRC= wind.c wind.h ace.c ace.h imp.c imp.h geotail.c geotail.h locate.c locate.h graphic.c graphic.h istp_plot.c istp.h hlpstr.h errstr.h 

LIBS=  -lopt.1 -lrtime.1 -lrcdf.1 -lgrplot.1 -liplot.1 -lfontdb.1 -lsplot.1 -lrps.1 -lrfbuf.1 -lrxml.1 -lgeopack.1 -L${CDF_PATH}/lib -lcdf
SLIB= -lpng -lm -lz 
XLIB= -lxwin.1

ifdef CDF_PATH
DSTPATH = $(BINPATH)
OUTPUT = istp_plot
endif















include $(MAKEBIN).$(SYSTEM)
