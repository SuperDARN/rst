# Makefile for aacgmdlm DLM
# =======================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I${IDL_IPATH}
SRC = aacgmdlm.c 
OBJS = aacgmdlm.o
DLMFILE = aacgmdlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = aacgmdlm
SLIB= -laacgm_v2.1 -ligrf_v2.1 -laacgm.1 -lrtime.1

include $(MAKEDLM).$(SYSTEM)
