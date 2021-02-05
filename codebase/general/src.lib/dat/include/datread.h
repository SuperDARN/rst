/* 
 Copyright (C) 2021 SuperDARN Canada, University of Saskatchewan
 Author: Marina Schmidt
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 Copied code from raw_read.c in cmpraw modified for dat files

 Modified

 Disclaimer:

 
*/

// dat file pointer structure
struct Datfp {
  int datfp;
  int inxfp;
  struct stat rstat;
  struct stat istat;
  double ctime;
  double stime;
  int frec;
  int rlen;
  int ptr;
  int (*datread)(struct Datfp *ptr,struct DatData *datdata);

};

// dat reader helper
int DatRead(struct Datfp *fp,struct DatData *);

// dat to read ACF data
int DatReadData(struct Datfp *fp,struct DatData *);

// Closer dat file pointer
void DatClose(struct Datfp *ptr);

// Opens dat file pointer and allocating dat structures
struct Datfp *DatOpen(char *datfile,char *inxfile);

// Converts dat structure to Dmap
int DatToDmap(struct DataMap *ptr, struct DatData *data);

