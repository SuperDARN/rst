# Makefile for oldcnvmapdlm DLM
# =============================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC = oldcnvmapdlm.c 
OBJS = oldcnvmapdlm.o
DLMFILE=oldcnvmapdlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = oldcnvmapdlm
SLIB= -lcnvmapidl.1 -lgrdidl.1 -lrprmidl.1 -loldcnvmap.1 -lcnvmap.1 -loldgrd.1 -lgrd.1 -lradar.1 -lrfile.1 -ldmap.1 -lrtime.1 -lrcnv.1

include $(MAKEDLM).$(SYSTEM)
