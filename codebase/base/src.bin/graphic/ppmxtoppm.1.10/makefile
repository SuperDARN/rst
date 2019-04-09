# Makefile for ppmxtoppm
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h ppmxtoppm.c

INCLUDE=-I$(IPATH)/base
OBJS = ppmxtoppm.o
DSTPATH = $(BINPATH)
OUTPUT = ppmxtoppm
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm

 

include $(MAKEBIN).$(SYSTEM)
