# Makefile for Site Library
# =========================
# by J.Spaleta & R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn

SRC = site.c build.c 
OBJS = site.o build.o
INC=${IPATH}/superdarn
LINK="1"
DSTPATH=$(LIBPATH)
OUTPUT = site

include $(MAKELIB).$(SYSTEM)
