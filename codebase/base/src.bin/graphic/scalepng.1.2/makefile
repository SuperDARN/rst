# Makefile for scalepng
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h scalepng.c errstr.h

INCLUDE=-I$(IPATH)/base
OBJS = scalepng.o
DSTPATH = $(BINPATH)
OUTPUT = scalepng
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm

include $(MAKEBIN).$(SYSTEM)
