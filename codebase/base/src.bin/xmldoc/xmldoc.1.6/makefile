# Makefile for xmldoc
# ===================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base

OBJS = loadconfig.o xmldoc.o 
SRC=hlpstr.h errstr.h loadconfig.c loadconfig.h xmldoc.c xmldoc.h
DSTPATH = $(BINPATH)
OUTPUT = xmldoc
LIBS=-lrscript.1 -lrxmldoc.1 -ltagdb.1 -lrxmldb.1 -lrxml.1 -lopt.1 
SLIB= -lm

 

include $(MAKEBIN).$(SYSTEM)
