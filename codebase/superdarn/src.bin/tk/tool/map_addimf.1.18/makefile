# Makefile for map_addimf
# =======================
# Author: R.J.Barnes
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
 
INCLUDE = -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn \
          -I$(CDF_PATH)/include -I$(IPATH)/analysis

SRC = hlpstr.h ace.c ace.h map_addimf.c errstr.h istp.h locate.c locate.h \
      wind.c wind.h
OBJS = ace.o wind.o locate.o map_addimf.o

LIBS = -lcnvmap.1 -loldcnvmap.1 -lgrd.1 -loldgrd.1 -lradar.1 -lrcdf.1 \
       -lgeopack.1 -ldmap.1 -lrfile.1 -lrtime.1 -lopt.1 -lrcnv.1 \
       -L${CDF_PATH}/lib -lcdf
SLIB = -lm -lz

ifdef CDF_PATH
DSTPATH = $(BINPATH)
OUTPUT = map_addimf
endif


include $(MAKEBIN).$(SYSTEM)
