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
SLIB= -laacgm.1

include $(MAKEDLM).$(SYSTEM)
