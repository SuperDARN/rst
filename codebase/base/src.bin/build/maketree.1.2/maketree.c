/* maketree.c
   ==========
   Author: R.J.Barnes
 
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>



int maketree(char *path,char *patn) {
  struct dirent *direntp=NULL;
  DIR *dirp;
  int s; 
  int flg=0;
  struct stat dstat;
  char spath[4096];

  fprintf(stdout,"%s\n",path);

  dirp=opendir(path);
  if (dirp==NULL) return 0;
  
  while ((direntp=readdir(dirp)) !=NULL) {
     if (direntp->d_name[0]=='.') continue;
     /* do pattern matching here */
     flg=(fnmatch(patn,direntp->d_name,0)==0);
     if (flg==1) break;
  }
  if (flg==1) {
    closedir(dirp);
    return 0;
  }

  rewinddir(dirp);
  while ((direntp=readdir(dirp)) !=NULL) {
    if (direntp->d_name[0]=='.') continue;
     sprintf(spath,"%s/%s",path,direntp->d_name);
     s=stat(spath,&dstat); 
    
     if ((s==0) && (S_ISDIR(dstat.st_mode))) {
       /* recurse here */
       maketree(spath,patn);
     }
  }
  closedir(dirp);
  return 0;
}

int main(int argc,char *argv[]) {
  maketree(argv[1],argv[2]);
  return 0;
}
 


  




