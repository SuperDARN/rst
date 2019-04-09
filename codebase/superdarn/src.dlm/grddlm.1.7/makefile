# Makefile for grddlm DLM
# =======================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC = grddlm.c 
OBJS = grddlm.o
DLMFILE=grddlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = grddlm
SLIB= -lgrdidl.1 -lrprmidl.1 -lgrd.1 -lradar.1 -ldmap.1 -lrtime.1 -lrcnv.1

include $(MAKEDLM).$(SYSTEM)
