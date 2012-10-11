# Makefile for index_file
# =======================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

SRC=hlpstr.h errstr.h index_file.c

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general
OBJS = index_file.o
DSTPATH = $(BINPATH)
OUTPUT = index_file
LIBS= -lopt.1 -lrfile.1 -lrcnv.1 -lrtime.1
SLIB= -lm -lz

 

include $(MAKEBIN).$(SYSTEM)
