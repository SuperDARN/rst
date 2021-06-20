/* xmldoc.h
   ======== 
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





struct xmldoc {
  struct XMLdocdata *doc;
  struct ScriptData *script;
  struct XMLDBtable *table;

  struct {
    char *path;
    char delim;
  } tree;

  struct {
    int num;
    char **txt;
  } match;

  struct {
    int (*check)(char *,struct XMLDBbuffer *,void *data);
    void *data;
  } compare;

  struct {
    int type;
    struct XMLDBbuffer *buf;
  } sc;

  struct {
    int iflg;
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
  } map;


 struct {
    struct {
      int num;
      char **txt;
    } search;
    struct {
      int num;
      char **txt;
    } replace;
  } external;



};
