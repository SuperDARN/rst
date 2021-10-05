#  Makefile for the linearfit library
#  ==================================
# Author: Angeline G. Burrell, NRL (2021)
# This is a U.S. government work and not under copyright protection in the U.S.
#
# Disclaimer:  RST is licensed under GPL v3.0. Please visit 
#              <https://www.gnu.org/licenses/> to see the full license
#
# Modifications:

include $(MAKECFG).$(SYSTEM)

INCLUDE	= -I$(IPATH)/base -I$(IPATH)/general -I$(IPATH)/analysis -I../include
INC	= $(IPATH)/analysis
OUTPUT	= linearfit
DSTPATH	= $(LIBPATH)

SRC	= linearfit.c
OBJS	= $(SRC:.c=.o)

LINK	= "1"

include $(MAKELIB).$(SYSTEM)

