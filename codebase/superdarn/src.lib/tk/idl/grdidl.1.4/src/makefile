# Makefile for grdidl library
# ===========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}

SRC=grdidl.c
OBJS = grdidl.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
ifdef IDL_IPATH
OUTPUT = grdidl
endif
LINK="1"

include $(MAKELIB).$(SYSTEM)
