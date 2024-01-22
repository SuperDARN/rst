# Makefile for igrfdlm DLM
# ========================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I${IDL_IPATH}
SRC = igrfdlm.c 
OBJS = igrfdlm.o
DLMFILE = igrfdlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = igrfdlm
SLIB= -ligrf.1 -ligrf_v2.1 -lastalg.1 -lrtime.1

include $(MAKEDLM).$(SYSTEM)
