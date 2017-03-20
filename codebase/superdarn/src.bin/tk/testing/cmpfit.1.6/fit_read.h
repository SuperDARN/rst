/* fit_read.h
   ==========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



struct fitfp {
  int fitfp;
  int inxfp;
  int fit_recl;
  int inx_recl;
  struct stat fstat;
  struct stat istat;
  int blen;
  int fptr;
  int iptr;
  unsigned char *fbuf;
  unsigned char *ibuf;
  int inx_srec;
  int inx_erec;
  double ctime;
  double stime;
  double etime;
  int time;
  char header[80];
  char date[32];
  char extra[256];
  char major_rev; 
  char minor_rev;
  int (*fitread)(struct fitfp *ptr,struct fitdata *fit_data);
};

int fit_read(struct fitfp *ptr,struct fitdata *fit_data);
void fit_close(struct fitfp *ptr);
struct fitfp *fit_open(char *fitfile,char *inxfile);



