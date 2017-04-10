/* rcgi.h
   ====== 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




#ifndef _RCGI_H
#define _RCGI_H

struct CGIarg {
  char *name;
  char *fname;
  int type;
  struct {
    struct {
      int sze;
      char *buf;
    } raw;
    struct {
      int sze;
      char *buf;
    } txt;
  } value;
};

struct CGIdata {
  int num;
  char *buf;
  int sze;
  int stp;
  int max;
  int state;
  int cnt; 
  char *bnd;

  struct {
    int (*func)(struct CGIarg *,char *,int,void *);
    void *data;
  } text;

  struct {
    int state;
    int sze;
    char *buf;
    int stp;
    int max;
  } mime;

  struct CGIarg **arg;
};

char *CGIGetString(struct CGIarg *);
char *CGIGetRawString(struct CGIarg *);
char *CGIMakeURLString(char *inp,int sze);

struct CGIdata *CGIMake();
void CGIFree(struct CGIdata *ptr);
int CGISetText(struct CGIdata *ptr,int (*text)(struct CGIarg *,char *,
					       int,void *),void *data);
int CGIFindInBuf(char *buf,int sze,char *str);
int CGIGet(struct CGIdata *ptr);
 
#endif
