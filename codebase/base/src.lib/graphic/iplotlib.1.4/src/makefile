# Makefile for iplotlib Library
# ============================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base

SRC = color.c dash.c matrix.c make.c bezier.c line.c shape.c polygon.c \
      text.c image.c imagename.c embed.c include.c info.c clip.c plot.c \
      document.c

OBJS = color.o dash.o matrix.o make.o bezier.o line.o shape.o polygon.o \
      text.o image.o imagename.o embed.o include.o info.o clip.o plot.o \
      document.o

INC = $(IPATH)/base

DSTPATH=$(LIBPATH)
OUTPUT = iplot
DOCNAME=plot.doc.xml
LINK="1"


















include $(MAKELIB).$(SYSTEM)
