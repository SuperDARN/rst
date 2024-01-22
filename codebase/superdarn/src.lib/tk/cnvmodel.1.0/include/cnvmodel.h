/*
 * Copyright (C) Evan Thomas, Dartmouth
 * Author:
 *  Evan Thomas
 *
 *     RST is free software: you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Modifications
 * =============
 * 2020-11-12 Marina Schmidt switched out RST defined complex structs to complex.h
 * 2020-11-12 Marina Schmidt modified cmult
 *
 */

#ifndef _CNVMODEL_H
#define _CNVMODEL_H

/* statistical models */

#define RG96  1
#define PSR10 2
#define CS10  4
#define TS18  8
#define TS18_Kp 16

#include <complex.h>

struct model {
  char hemi[64];
  char tilt[64];
  char angle[64];
  char level[64];
  int ihem,ilev,itlt,iang;
  char str[3][128];
  float latref;
  int ltop,mtop;
  double complex *aoeff_n;
  double complex *aoeff_p;
};

extern struct model *model[2][3][6][8]; /* [hemi][tilt][lev][ang] */

/* prototypes */

double factorial(double n);
double complex cmult(double complex b, double complex c);
void slv_ylm_mod(float theta, float phi, int order, double complex *ylm_p, 
        double complex *ylm_n, double *anorm, double *plm_p, double *apcnv);
void slv_sph_kset(float latmin, int num, float *phi, float *the, 
        float *the_col, double *ele_phi, double *ele_the, struct model *mod,
        double *pot);
struct model *determine_model(float Vsw, float Bx, float By, float Bz, 
        int hemi, float tilt, float kp, int imod, int nointerp);
struct model *interp_coeffs(int ih, float tilt, float mag,
        float cang, int imod);
struct model *load_model(FILE *fp, int ihem, int ilev,
        int iang, int itlt, int imod);
int load_all_models(char *path, int imod, int ecdip);

#endif
