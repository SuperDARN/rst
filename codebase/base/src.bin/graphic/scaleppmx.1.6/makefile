# Makefile for scaleppmx
# ======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h scaleppmx.c errstr.h

INCLUDE=-I$(IPATH)/base
OBJS = scaleppmx.o
DSTPATH = $(BINPATH)
OUTPUT = scaleppmx
LIBS=-lrfbuf.1 -lrxml.1 -lopt.1 
SLIB= -lpng -lm

include $(MAKEBIN).$(SYSTEM)
