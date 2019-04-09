# Makefile for oldrawdlm DLM
# ==========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I${IDL_IPATH}
SRC = oldrawdlm.c 
OBJS = oldrawdlm.o
DLMFILE=oldrawdlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = oldrawdlm
SLIB= -lrawidl.1 -lrprmidl.1 -loldraw.1 -lraw.1 -lradar.1 -ldmap.1 -lrtime.1 -lrcnv.1

include $(MAKEDLM).$(SYSTEM)
