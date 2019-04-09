# Makefile for fitcnx library
# ============================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC=fitcnx.c decodemsg.c decodemsg.h cnxstr.h
OBJS = fitcnx.o decodemsg.o 
INC=$(IPATH)/superdarn
LINK=1
DSTPATH=$(LIBPATH)
OUTPUT = fitcnx

include $(MAKELIB).$(SYSTEM)
