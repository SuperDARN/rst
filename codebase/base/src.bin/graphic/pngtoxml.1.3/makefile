# Makefile for pngtoxml
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base
SRC=pngtoxml.c
OBJS = pngtoxml.o
SRC=hlpstr.h errstr.h pngtoxml.c
OBJS = pngtoxml.o
DSTPATH = $(BINPATH)
OUTPUT = pngtoxml
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm

include $(MAKEBIN).$(SYSTEM)
