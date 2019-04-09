# Makefile for radar library
# ========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC=raw.c rawread.c rawwrite.c rawinx.c rawseek.c
OBJS = raw.o rawread.o rawwrite.o rawinx.o rawseek.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT = raw
LINK="1"



include $(MAKELIB).$(SYSTEM)
