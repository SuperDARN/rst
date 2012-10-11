/* option.h
   ======== 
   Author: R.J.Barnes
*/


/*
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 RST is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/




#ifndef _OPTION_H
#define _OPTION_H

struct OptionEntry {
 char *name;
 char type;
 int set;
 void *ptr;
};

struct OptionData {
  int num;
  struct OptionEntry *ptr;
};

struct OptionText {
  int num;
  char **txt;
};

struct OptionFile {
  int argc;
  char **argv;
};

char *OptionScanFileArg(FILE *file);
struct OptionFile *OptionProcessFile(FILE *fp);
void OptionFreeFile(struct OptionFile *ptr);
int OptionProcess(int offset,int argc,char *argv[], struct OptionData *opt,
		  int (*opterr)(char *));
int OptionAdd(struct OptionData *opt,char *name,char type,void *data);

int OptionDump(FILE *fp,struct OptionData *opt);
void OptionFree(struct OptionData *opt);



void OptionPrintInfo(FILE *fp,char *str[]);

#endif

