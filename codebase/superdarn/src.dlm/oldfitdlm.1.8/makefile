# Makefile for oldfitdlm DLM
# ==========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC = oldfitdlm.c 
OBJS = oldfitdlm.o
DLMFILE= oldfitdlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = oldfitdlm
SLIB= -lfitidl.1 -lrprmidl.1 -loldfit.1 -lfit.1 -lradar.1 -ldmap.1 -lrtime.1 -lrcnv.1

include $(MAKEDLM).$(SYSTEM)
