# Makefile for padppmx
# ===================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h padppmx.c

INCLUDE=-I$(IPATH)/base
OBJS = padppmx.o
DSTPATH = $(BINPATH)
OUTPUT = padppmx
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm
include $(MAKEBIN).$(SYSTEM)
