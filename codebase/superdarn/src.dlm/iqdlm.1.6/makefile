# Makefile for iqdlm DLM
# =======================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC = iqdlm.c iqidl.c
OBJS =  iqdlm.o iqidl.o
DLMFILE=iqdlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = iqdlm
SLIB= -lrprmidl.1 -liqdata.1 -lradar.1 -ldmap.1 -lrtime.1 -lrcnv.1

include $(MAKEDLM).$(SYSTEM)
