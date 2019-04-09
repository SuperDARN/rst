# Makefile for scdoc
# ==================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h scdoc.c

INCLUDE=-I$(IPATH)/base
OBJS = scdoc.o 
DSTPATH = $(BINPATH)
OUTPUT = scdoc
LIBS=-lrscript.1 -lrxmldoc.1 -ltagdb.1 -lrxmldb.1 -lrxml.1 -lopt.1 
SLIB= -lm

 

include $(MAKEBIN).$(SYSTEM)
