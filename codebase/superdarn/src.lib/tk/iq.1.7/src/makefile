# Makefile for iqdata
# ==================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC=iq.c iqread.c iqwrite.c iqseek.c iqinx.c
OBJS = iq.o iqread.o iqwrite.o iqseek.o iqinx.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)

OUTPUT = iqdata
LINK="1"


include $(MAKELIB).$(SYSTEM)
