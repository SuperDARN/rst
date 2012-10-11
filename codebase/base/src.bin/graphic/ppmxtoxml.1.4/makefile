# Makefile for ppmxtoxml
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h ppmxtoxml.c

INCLUDE=-I$(IPATH)/base
OBJS = ppmxtoxml.o
DSTPATH = $(BINPATH)
OUTPUT = ppmxtoxml
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm

 

include $(MAKEBIN).$(SYSTEM)
