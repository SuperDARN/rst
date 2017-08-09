# Makefile for Frame Buffer Library
# =================================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base
SRC=png.c bezier.c char.c color.c dash.c image.c line.c make.c matrix.c \
    polygon.c ppm.c xml.c shape.c user.c clip.c

OBJS=png.o bezier.o char.o color.o clip.o dash.o image.o line.o make.o \
    matrix.o polygon.o ppm.o shape.o user.o xml.o

INC=$(IPATH)/base
DSTPATH=$(LIBPATH)
OUTPUT = rfbuf
LINK="1"

















include $(MAKELIB).$(SYSTEM)
