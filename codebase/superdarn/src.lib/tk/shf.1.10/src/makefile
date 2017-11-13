# Makefile for shf library
# ========================
# by R.J.Barnes
#
include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/superdarn -I$(IPATH)/analysis   

OBJS = cnvgrid.o evallegendre.o evalpotential.o evalefield.o \
       solvepotential.o solvevelocity.o solveefield.o \
       solve.o indexlegendre.o evalvelocity.o crdshft.o \
       svdbksb.o svdcmp.o fitvector.o fitmap.o calc_bmag.o
SRC=  cnvgrid.c evallegendre.c evalpotential.c evalefield.c \
      solveefield.c solveefield.h solvepotential.c solvepotential.h \
      solvevelocity.c  solvevelocity.h nrutil.h calc_bmag.c \
      solve.c indexlegendre.c evalvelocity.c crdshft.c \
      svdbksb.c svdbksb.h svdcmp.c svdcmp.h fitvector.c \
      fitvector.h fitmap.c shfvector.h

INC=$(IPATH)/superdarn
LINK="1"
DSTPATH=$(LIBPATH)

OUTPUT = shf
 
include $(MAKELIB).$(SYSTEM)
