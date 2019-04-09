# Makefile for cfit library
# ========================== 
# by R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn
SRC = cfit.c cfitopen.c cfitclose.c cfitwrite.c cfitread.c cfitseek.c \
      cfitscan.c cfitinx.c
OBJS = cfit.o cfitopen.o cfitclose.o cfitwrite.o cfitread.o cfitseek.o \
       cfitscan.o cfitinx.o
INC=$(IPATH)/superdarn
LINK="1"
DSTPATH=$(LIBPATH)
OUTPUT = cfit

include $(MAKELIB).$(SYSTEM)
