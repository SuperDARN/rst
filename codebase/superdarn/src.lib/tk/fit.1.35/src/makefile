# Makefile for radar library
# ========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC=fit.c fitscan.c fitcfit.c fitread.c fitwrite.c fitseek.c fitinx.c
OBJS = fit.o fitscan.o fitcfit.o fitread.o fitwrite.o fitseek.o fitinx.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
OUTPUT = fit
LINK="1"



include $(MAKELIB).$(SYSTEM)
