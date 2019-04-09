# Makefile for cnx library
# ========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)
INCLUDE=-I$(IPATH)/base -DPOSIX -D__EXTENSIONS__
SRC=connex.c msg.c err.c
OBJS = connex.o msg.o err.o 
INC=$(IPATH)/base
DSTPATH=$(LIBPATH)
LINK="1"
OUTPUT = cnx







include $(MAKELIB).$(SYSTEM)
