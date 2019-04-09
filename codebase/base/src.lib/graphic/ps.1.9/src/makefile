# Makefile for PostScript Library
# ===============================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base

SRC = matrix.c color.c clip.c dash.c state.c make.c bezier.c line.c polygon.c \
     shape.c text.c image.c plot.c 

OBJS = matrix.o color.o dash.o clip.o state.o make.o bezier.o line.o \
       polygon.o shape.o text.o image.o plot.o
INC=$(IPATH)/base
DSTPATH=$(LIBPATH)

OUTPUT = rps
LINK="1"

















include $(MAKELIB).$(SYSTEM)
