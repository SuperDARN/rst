# Makefile for padxml
# ===================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h padxml.c

INCLUDE=-I$(IPATH)/base
OBJS = padxml.o
DSTPATH = $(BINPATH)
OUTPUT = padxml
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm
include $(MAKEBIN).$(SYSTEM)
