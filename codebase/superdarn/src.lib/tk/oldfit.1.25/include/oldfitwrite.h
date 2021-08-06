/* oldfitwrite.h
   ===========
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



#ifndef _OLDFITWRITE_H
#define _OLDFITWRITE_H

int OldFitHeaderWrite(int fitfp, char *text, char *name,char *version);
int OldFitInxClose(int inxfp,struct RadarParm *prm,int irec);
int OldFitInxHeaderWrite(int inxfp,struct RadarParm *prm);
int OldFitInxWrite(int inxfp,int drec,int dnum,struct RadarParm *prm);
int OldFitWrite(int fitfp,struct RadarParm *prm,struct FitData *fit,
		int *rtab);

int OldFitHeaderFwrite(FILE *fitfp, char *text, char *name,char *version);
int OldFitInxFclose(FILE *inxfp,struct RadarParm *prm,int irec);
int OldFitInxHeaderFwrite(FILE *inxfp,struct RadarParm *prm);
int OldFitInxFwrite(FILE *inxfp,int drec,int dnum,struct RadarParm *prm);
int OldFitFwrite(FILE *fitfp,struct RadarParm *prm,struct FitData *fit,
                 int *rtab);

#endif
