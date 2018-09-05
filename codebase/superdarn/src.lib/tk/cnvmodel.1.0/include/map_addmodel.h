
#ifndef _CNVMODEL_H
#define _CNVMODEL_H

/* statistical models */

#define RG96  1
#define PSR10 2
#define CS10  4
#define TS18  8
#define TS18_Kp 16

struct model {
  char hemi[64];
  char tilt[64];
  char angle[64];
  char level[64];
  int ihem,ilev,itlt,iang;
  char str[3][128];
  float latref;
  int ltop,mtop;
  struct complex *aoeff_n;
  struct complex *aoeff_p;
};

struct model *model[2][3][6][8]; /* [hemi][tilt][lev][ang] */

/* prototypes */

struct model *determine_model(float Vsw, float Bx, float By, float Bz, int hemi,
                              float tilt, float kp, int imod, int nointerp);
struct model *interp_coeffs(int ih, float tilt, float mag, float cang, int imod);
struct model *load_model(FILE *fp, int ihem, int ilev, int iang, int itlt,
                                   int imod);
int load_all_models(char *path, int imod);

#endif
