/* makecgm.c
   =========
   Author: R.J.Barnes
*/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct {
    double coef[121][3][5][2];
} sph_harm_model;


int load_coef_AACGM(char *fname) {
  char tmp[64];
  FILE *fp;
  int f,l,a,t,i;
  fp=fopen(fname,"r");
  for (f=0;f<2;f++) { 
    for (l=0;l<5;l++) {
      for (a=0;a<3;a++) { 
        for (t=0;t<121;t++) {
	  if (fscanf(fp,"%s",&tmp) !=1) {
             fclose(fp);
             return -1;
	  }
          for (i=0;(tmp[i] !=0) && (tmp[i] !='D');i++);
          if (tmp[i]=='D') tmp[i]='e';
          sph_harm_model.coef[t][a][l][f]=atof(tmp);
	}
      }
    }
  }
  fclose(fp);
  return 0;
}

int init_AACGM(int year) {
  char fname[256];
  char yrstr[32];  
  if (year==0) year=1995;
  sprintf(yrstr,"%4.4d",year);  
  strcpy(fname,getenv("AACGM_DAT_PREFIX"));  
  if (strlen(fname)==0) return -1;
  strcat(fname,yrstr);
  strcat(fname,".asc");
  return load_coef_AACGM(fname);
}

int main() {
  int f,l,a,t;
  init_AACGM(0);
  fprintf(stdout,"/* coeff.c \n");
  fprintf(stdout,"   ======= */\n");
  fprintf(stdout,"\n\n");
  fprintf(stdout,"struct {\n");
  fprintf(stdout,"  double coef[121][3][5][2];\n");
  fprintf(stdout,"} sph_harm_model= {\n");
  fprintf(stdout,"  {\n"); 
  for (t=0;t<121;t++) {
    if (t!=0) fprintf(stdout,",\n");
    fprintf(stdout,"    {\n      ");
    for (a=0;a<3;a++) {
      if (a !=0) fprintf(stdout,",\n        ");
      fprintf(stdout,"{\n        ");   
      for (l=0;l<5;l++) {
        if (l !=0) fprintf(stdout,",");
        if ((l !=0) && ((l % 2)==0)) fprintf(stdout,"\n        ");
        fprintf(stdout,"{%.8le,%.8le}",
	      sph_harm_model.coef[t][a][l][0],
	      sph_harm_model.coef[t][a][l][1]);
      }
      fprintf(stdout,"\n");
      fprintf(stdout,"      }");
    }
    fprintf(stdout,"\n");
    fprintf(stdout,"    }");
  }
  fprintf(stdout,"\n");
  fprintf(stdout,"  }\n");
  fprintf(stdout,"};\n");
  return 0;
}



