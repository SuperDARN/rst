# Makefile for mltdlm DLM
# =======================
# by R.J.Barnes
#
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I${IDL_IPATH}
SRC = mltdlm.c 
OBJS = mltdlm.o
DLMFILE=mltdlm.dlm
INC=$(IPATH)/analysis
DSTPATH=${DLMPATH}
LINK="1"
OUTPUT = mltdlm
SLIB= -lmlt.1 -lastalg.1 -laacgm.1 -lrtime.1

include $(MAKEDLM).$(SYSTEM)
