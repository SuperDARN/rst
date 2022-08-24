/* 
 Copyright (c) 2021 University of Saskatchewan
 Author: Marina Schmidt
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 Copied code from raw_read.c in cmpraw modified for dat files


 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

 Modifications:
 
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

