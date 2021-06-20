/* raw_read.h
   ==========
   Author: R.J.Barnes
*/


/*
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory

This file is part of the Radar Software Toolkit (RST).

RST is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.

Modifications:
*/



struct rawfp {
  int rawfp;
  int inxfp;
  struct stat rstat;
  struct stat istat;
  double ctime;
  double stime;
  int frec;
  int rlen;
  int ptr;
  int (*rawread)(struct rawfp *ptr,struct rawdata *raw_data);

};

int raw_read(struct rawfp *fp,struct rawdata *);
int raw_read_data(struct rawfp *fp,struct rawdata *);
int raw_seek(struct rawfp *ptr,int yr,int mo,int dy,int hr,int mt,int sc,
	     double *atme);
void raw_close(struct rawfp *ptr);
struct rawfp *raw_open(char *rawfile,char *inxfile);



