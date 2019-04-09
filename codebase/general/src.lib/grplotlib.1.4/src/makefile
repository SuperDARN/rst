# Makefile for plot library
# =========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general
SRC=grplotraw_contour.c grplotraw.c grplotraw_key.c grplotstd_key.c \
    grplot.c grplot_key.c
OBJS = grplot_key.o grplot.o grplotstd_key.o grplotraw_key.o \
       grplotraw_contour.o grplotraw.o 
INC=$(IPATH)/general
DSTPATH=$(LIBPATH)
OUTPUT = grplot
LINK="1"

















include $(MAKELIB).$(SYSTEM)
