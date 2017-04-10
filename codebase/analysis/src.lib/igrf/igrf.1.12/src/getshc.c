/* getshc.c
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#include <stdio.h>
#include <stdlib.h>


int getshc(char *fname,int *nmax,double *erad,double *gh) {

  char dummy[256];
  int nn,mm,n,m;
  double g,h;
  double yr;
  FILE *fp;
  int stat;
  int i=0;
  fp=fopen(fname,"r");
  if (fp==NULL) return -1;
   
   stat=(fgets(dummy,256,fp)==NULL);
   if (stat !=0) {
     fclose(fp);
     return -1;
   }
    
   stat=(fscanf(fp,"%d %lg %lg",nmax,erad,&yr) !=3);
   if (stat !=0) {
     fclose(fp);
     return -1;
   }
   
   for (nn=1;nn<=*nmax;nn++) {
     for (mm=0;mm<=nn;mm++) {
    
       stat=(fscanf(fp,"%d %d  %lg %lg ",&n,&m,&g,&h) !=4);
       if (stat !=0) {
         fclose(fp);
         return -1;
       }
       gh[i]=g;
       i++;
       if (m !=0) {
         gh[i]=h;
         i++; 
       } 
     }
   }
   fclose(fp);
   return 0;
}


