/* rscript.h
   ========= 
   Author: R.J.Barnes
 LICENSE AND DISCLAIMER
 
 Copyright (c) 2012 The Johns Hopkins University/Applied Physics Laboratory
 
 This file is part of the Radar Software Toolkit (RST).
 
 RST is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with RST.  If not, see <http://www.gnu.org/licenses/>.
 
 
 
*/




#ifndef _RSCRIPT_H
#define _RSCRIPT_H

struct ScriptData {
  int depth;
  int sze;
  int max;
  int stp;
  char *buf;

  struct {
    int (*func)(char *,int,void *);
    void *data;
  } text;
 
};

struct ScriptData *ScriptMake();
void ScriptFree(struct ScriptData *ptr);
 
int ScriptSetText(struct ScriptData *ptr,
	       int (*text)(char *,int,void *),void *data);

int ScriptDecode(struct ScriptData *ptr,char *buf,int sze);
  
#endif
