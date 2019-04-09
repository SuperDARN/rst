# Makefile for padpng
# ===================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h padpng.c

INCLUDE=-I$(IPATH)/base
OBJS = padpng.o
DSTPATH = $(BINPATH)
OUTPUT = padpng
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm
include $(MAKEBIN).$(SYSTEM)
