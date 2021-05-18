/* rconvert.h
   ==========
   Author: R.J.Barnes

(c) 2011 The Johns Hopkins University/Applied Physics Laboratory & Others
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


#ifndef _RCONVERT_H
#define _RCONVERT_H

int ConvertBitOrder();
void ConvertToLong(unsigned char *cnv,int64 *val);
void ConvertToInt(unsigned char *cnv,int32 *val);
void ConvertToShort(unsigned char *cnv,int16 *val);
void ConvertToULong(unsigned char *cnv,uint64 *val);
void ConvertToUInt(unsigned char *cnv,uint32 *val);
void ConvertToUShort(unsigned char *cnv,uint16 *val);
void ConvertToDouble(unsigned char *cnv,double *val);
void ConvertToFloat(unsigned char *cnv,float *val);
void ConvertFromFloat(float val,unsigned char *cnv);
void ConvertFromDouble(double val,unsigned char *cnv);
void ConvertFromLong(int64 val,unsigned char *cnv);
void ConvertFromInt(int32 val,unsigned char *cnv);
void ConvertFromShort(int16 val,unsigned char *cnv);
void ConvertFromULong(uint64 val,unsigned char *cnv);
void ConvertFromUInt(uint32 val,unsigned char *cnv);
void ConvertFromUShort(uint16 val,unsigned char *cnv);
void ConvertBlock(unsigned char *cnv,int *pattern);
int ConvertFreadLong(FILE *fp,int64 *val);
int ConvertFreadInt(FILE *fp,int32 *val);
int ConvertFreadShort(FILE *fp,int16 *val);
int ConvertFreadULong(FILE *fp,uint64 *val);
int ConvertFreadUInt(FILE *fp,uint32 *val);
int ConvertFreadUShort(FILE *fp,uint16 *val);
int ConvertFreadDouble(FILE *fp,double *val);
int ConvertFreadFloat(FILE *fp,float *val);
int ConvertFwriteFloat(FILE *fp,float val);
int ConvertFwriteDouble(FILE *fp,double val);
int ConvertFwriteLong(FILE *fp,int64 val);
int ConvertFwriteInt(FILE *fp,int32 val);
int ConvertFwriteShort(FILE *fp,int16 val);
int ConvertFwriteULong(FILE *fp,uint64 val);
int ConvertFwriteUInt(FILE *fp,uint32 val);
int ConvertFwriteUShort(FILE *fp,uint16 val);

int ConvertReadLong(int fp,int64 *val);
int ConvertReadInt(int fp,int32 *val);
int ConvertReadShort(int fp,int16 *val);
int ConvertReadULong(int fp,uint64 *val);
int ConvertReadUInt(int fp,uint32 *val);
int ConvertReadUShort(int fp,uint16 *val);
int ConvertReadDouble(int fp,double *val);
int ConvertReadFloat(int fp,float *val);
int ConvertWriteFloat(int fp,float val);
int ConvertWriteDouble(int fp,double val);
int ConvertWriteLong(int fp,int64 val);
int ConvertWriteInt(int fp,int32 val);
int ConvertWriteShort(int fp,int16 val);
int ConvertWriteULong(int fp,uint64 val);
int ConvertWriteUInt(int fp,uint32 val);
int ConvertWriteUShort(int fp,uint16 val);


int ConvertReadLongZ(gzFile file,int64 *val);
int ConvertReadIntZ(gzFile file,int32 *val);
int ConvertReadShortZ(gzFile file,int16 *val);
int ConvertReadULongZ(gzFile file,uint64 *val);
int ConvertReadUIntZ(gzFile file,uint32 *val);
int ConvertReadUShortZ(gzFile file,uint16 *val);
int ConvertReadDoubleZ(gzFile file,double *val);
int ConvertReadFloatZ(gzFile file,float *val);
int ConvertWriteFloatZ(gzFile file,float val);
int ConvertWriteDoubleZ(gzFile file,double val);
int ConvertWriteLongZ(gzFile file,int64 val);
int ConvertWriteIntZ(gzFile file,int32 val);
int ConvertWriteShortZ(gzFile file,int16 val);
int ConvertWriteULongZ(gzFile file,uint64 val);
int ConvertWriteUIntZ(gzFile file,uint32 val);
int ConvertWriteUShortZ(gzFile file,uint16 val);

#endif


