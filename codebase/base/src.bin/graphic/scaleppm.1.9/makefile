# Makefile for scaleppm
# =====================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h scaleppm.c

INCLUDE=-I$(IPATH)/base
OBJS = scaleppm.o
DSTPATH = $(BINPATH)
OUTPUT = scaleppm
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm

 

include $(MAKEBIN).$(SYSTEM)
