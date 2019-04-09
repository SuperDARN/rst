# Makefile for ppmxtopng
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h ppmxtopng.c

INCLUDE=-I$(IPATH)/base
OBJS = ppmxtopng.o
DSTPATH = $(BINPATH)
OUTPUT = ppmxtopng
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm

 

include $(MAKEBIN).$(SYSTEM)
