# Makefile for pngtoppm
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h pngtoppm.c

INCLUDE=-I$(IPATH)/base
OBJS = pngtoppm.o
DSTPATH = $(BINPATH)
OUTPUT = pngtoppm
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm

 

include $(MAKEBIN).$(SYSTEM)
