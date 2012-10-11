# Makefile for cmpraw
# ===================
# Author: R.J.Barnes
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
OBJS=raw_read.o raw_read_current.o raw_open.o raw_close.o cmpraw.o 
SRC=raw_read_current.c hlpstr.h lmt.h raw.h errstr.h raw_close.c cmpraw.c raw_versions.h raw_open.c raw_read.c raw_read.h
DSTPATH = $(BINPATH)
OUTPUT = cmpraw
LIBS= -lrcnv.1 -lrtime.1 -lopt.1
SLIB= -lm -lz

 

include $(MAKEBIN).$(SYSTEM)
