/* fio.c
   =====
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

char *FIOMakeFile(char *pathenv,int yr,int mo,int dy,int hr,int mt,int sc,
		  char *code,char *extra,char *ext,int mode,int flag) {
 
  char pathname[128]="/";
  char *fullname=NULL;
  char name[32];
  char *path;
  int file;
  int openflag;
 
  fullname=malloc(256);
  if (fullname==NULL) return NULL;

  if (mode==0) 
    mode=S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  openflag=O_WRONLY | O_CREAT;
  if (flag==0) openflag|=O_EXCL;
  else if (flag==1) openflag|=O_TRUNC;    

  if (extra==NULL)  
    sprintf(name,"%04d%02d%02d.%02d%02d.%02d.%s",yr,mo,dy,hr,mt,sc,code);
  else 
    sprintf(name,"%04d%02d%02d.%02d%02d.%02d.%s.%s",
            yr,mo,dy,hr,mt,sc,code,extra);
 
  if (pathenv !=NULL) strncpy(pathname,pathenv,80);
  path = strtok(pathname, ":");	
  file = -1;
  while ((file == -1) && (path != NULL)) {
    sprintf(fullname,"%s/%s.%s",path,name,ext);      
    file = open(fullname,openflag,mode);
    path = strtok(NULL, ":");    
  }
   
  if (file != -1) {
    close(file);
    return fullname;
  }

  free(fullname);
  return NULL;
}
