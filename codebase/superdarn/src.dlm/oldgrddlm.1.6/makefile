# Makefile for oldgrddlm DLM
# ==========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC = oldgrddlm.c 
OBJS = oldgrddlm.o
DLMFILE=oldgrddlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = oldgrddlm
SLIB= -lgrdidl.1 -lrprmidl.1 -loldgrd.1 -lgrd.1 -lradar.1 -lrfile.1 -ldmap.1 -lrtime.1 -lrcnv.1

include $(MAKEDLM).$(SYSTEM)
