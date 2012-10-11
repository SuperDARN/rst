# Makefile for rposdlm DLM
# =======================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC = rposdlm.c 
OBJS = rposdlm.o
DLMFILE=rposdlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = rposdlm
SLIB= -lrpos.1 -lradar.1 -lrtime.1 -lrcnv.1

include $(MAKEDLM).$(SYSTEM)
