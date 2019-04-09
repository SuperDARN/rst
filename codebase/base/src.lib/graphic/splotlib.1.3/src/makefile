# Makefile for splotlib Library
# =============================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base

SRC = make.c bezier.c line.c shape.c polygon.c \
      text.c image.c clip.c plot.c \
      document.c

OBJS = make.o bezier.o line.o shape.o polygon.o \
      text.o image.o clip.o plot.o \
      document.o 

INC = $(IPATH)/base

DSTPATH=$(LIBPATH)
OUTPUT = splot
DOCNAME=splot.doc.xml
LINK="1"


















include $(MAKELIB).$(SYSTEM)
