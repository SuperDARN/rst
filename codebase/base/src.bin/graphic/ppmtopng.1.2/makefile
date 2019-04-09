# Makefile for ppmtopng
# =====================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base
OBJS = ppmtopng.o
SRC=ppmtopng.c hlpstr.h errstr.h
DSTPATH = $(BINPATH)
OUTPUT = ppmtopng
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm

include $(MAKEBIN).$(SYSTEM)
