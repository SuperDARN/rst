/* key.h
   =====
   Author: R.J.Barnes
*/



/*
   See license.txt
*/



struct key {
  int num;
  double min,max;
  unsigned int defcol;
  unsigned char *a;
  unsigned char *r;
  unsigned char *g;
  unsigned char *b;
};

int load_key(FILE *fp,struct key *key);
