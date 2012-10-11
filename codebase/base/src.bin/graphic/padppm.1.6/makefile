# Makefile for padppm
# ===================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h padppm.c

INCLUDE=-I$(IPATH)/base
OBJS = padppm.o
DSTPATH = $(BINPATH)
OUTPUT = padppm
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm
include $(MAKEBIN).$(SYSTEM)
