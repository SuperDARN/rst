# Makefile for rpos library
# ==========================
# by R.J.Barnes
#
# Disclaimer:  RST is licensed under GPL v3.0. Please visit 
#              <https://www.gnu.org/licenses/> to see the full license
#
# Modifications:
# 2021-09-27 Angeline G. Burrell: Added a routine to calculate virtual height
#                                 using elevation angles.

include $(MAKECFG).$(SYSTEM)

INCLUDE=-I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis \
	-I$(IPATH)/superdarn

SRC =	cnvtcoord.c \
	calcvector.c \
	invmag.c \
	geodtgc.h \
	elvheight.c

OBJS =	cnvtcoord.o \
	calcvector.o \
	invmag.o \
	elvheight.o

INC=$(IPATH)/superdarn

DSTPATH=$(LIBPATH)

OUTPUT = rpos
LINK="1"

include $(MAKELIB).$(SYSTEM)
