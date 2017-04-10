/* option.h
   ======== 
   Author: R.J.Barnes
*/


/*
   See license.txt
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

