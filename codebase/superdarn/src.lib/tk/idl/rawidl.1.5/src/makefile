# Makefile for rawidl library
# ===========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}

SRC=rawidl.c
OBJS = rawidl.o
INC=$(IPATH)/superdarn
DSTPATH=$(LIBPATH)
ifdef IDL_IPATH
OUTPUT = rawidl
endif
LINK="1"

include $(MAKELIB).$(SYSTEM)
