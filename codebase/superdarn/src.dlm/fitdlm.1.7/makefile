# Makefile for fitdlm DLM
# =======================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC = fitdlm.c 
OBJS = fitdlm.o
DLMFILE= fitdlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = fitdlm
SLIB= -lfitidl.1 -lrprmidl.1 -lfit.1 -lradar.1 -ldmap.1 -lrtime.1 -lrcnv.1

include $(MAKEDLM).$(SYSTEM)
