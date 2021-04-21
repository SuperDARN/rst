/* rscriptHTML.h
   ============= 
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



#ifndef _RSCRIPTHTML_H
#define _RSCRIPTHTML_H

struct ScriptHTML {
  struct ScriptData *ptr;

  struct {
    int num;
    char **txt;
  } search;

 struct {
    int num;
    char **txt;
  } replace;

 struct {
    int num;
    char **txt;
  } ignore;

 struct {
    int num;
    char **txt;
  } remove;

  struct {
    int num;
    char **txt;
  } post;

  int iflg; /* ignore flag */
  int pflg; /* post processing flag */

  struct {
    int (*func)(char *,int,void *);
    void *data;
  } text;

  struct {
    int (*func)(char *,int,void *);
    void *data;
  } ssi;

 struct {
    int (*func)(char *,int,char *,void *);
    void *data;
  } postp;

};


struct ScriptHTML *ScriptHTMLMake(struct ScriptData *script);
 
void ScriptHTMLFree(struct ScriptHTML *ptr);
  
int ScriptHTMLSetText(struct ScriptHTML *ptr,
             int (*text)(char *,int,void *),void *data);
 
int ScriptHTMLSetSsi(struct ScriptHTML *ptr,
             int (*text)(char *,int,void *),void *data);

int ScriptHTMLSetPostp(struct ScriptHTML *ptr,
             int (*text)(char *,int,char *,void *),void *data);
 
int ScriptHTMLSetSearch(struct ScriptHTML *ptr,int num,char **search);

int ScriptHTMLSetReplace(struct ScriptHTML *ptr,int num,char **replace);
 
int ScriptHTMLSetIgnore(struct ScriptHTML *ptr,int num,char **ignore);
 
int ScriptHTMLSetRemove(struct ScriptHTML *ptr,int num,char **remove);
  
int ScriptHTMLSetPost(struct ScriptHTML *ptr,int num,char **post);

int ScriptHTMLSetSearchCount(struct ScriptHTML *ptr,char **search);

int ScriptHTMLSetReplaceCount(struct ScriptHTML *ptr,char **replace);
 
int ScriptHTMLSetIgnoreCount(struct ScriptHTML *ptr,char **ignore);
 
int ScriptHTMLSetRemoveCount(struct ScriptHTML *ptr,char **remove);
  
int ScriptHTMLSetPostCount(struct ScriptHTML *ptr,char **post);
 
int cmp_tag(char *token,char *str);

int cmp_tags(char *token,int num,char *str[]);
 
int HTMLWriter(char *buf,int sze,void *data);

#endif
 
