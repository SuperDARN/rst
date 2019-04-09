/* oldfitread.h
   ============
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




#ifndef _OLDFITREAD_H
#define _OLDFITREAD_H

struct OldFitFp {
  int fitfp;
  int inxfp;
  int fit_recl;
  int inx_recl;
  struct stat fstat;
  struct stat istat;
  int blen;
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
  int (*fitread)(struct OldFitFp *ptr,struct RadarParm *prm,
                 struct FitData *fit);
};


int OldFitReadInx(struct OldFitFp *ptr,int32 *buffer,int recno);
int OldFitRead(struct OldFitFp *ptr,struct RadarParm *prm,
               struct FitData *fit);
int OldFitSeek(struct OldFitFp *ptr,
               int yr,int mo,int dy,int hr,int mt,int sc,
	       double *atme);
void OldFitClose(struct OldFitFp *ptr);
struct OldFitFp *OldFitOpenFd(int fitfp,int inxfp);

struct OldFitFp *OldFitOpen(char *fitfile,char *inxfile);

#endif


