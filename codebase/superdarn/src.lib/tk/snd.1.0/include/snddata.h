/* snddata.h
   =========
   Author: E.G.Thomas
*/


#ifndef _SNDDATA_H
#define _SNDDATA_H


#define SND_MAJOR_REVISION 1
#define SND_MINOR_REVISION 1


struct SndData {

  struct {
    char major;
    char minor;
  } radar_revision;

  struct {
    char code;
    char *time;
    char *command;
  } origin;

  int16 cp;
  int16 stid;
  
  struct {
    int16 yr;
    int16 mo;
    int16 dy;
    int16 hr;
    int16 mt;
    int16 sc;
    int32 us;
  } time;

  int16 nave;
  int16 lagfr;
  int16 smsep;

  struct {
    float search;
    float mean;
  } noise;

  int16 channel;
  int16 bmnum;
  float bmazm;
  int16 scan;
  int16 rxrise;

  struct {
    int16 sc;
    int32 us;
  } intt;

  int16 nrang;
  int16 frang;
  int16 rsep;
  int16 xcf;
  int16 tfreq;

  double sky_noise;

  char *combf;

  struct {
    int major;
    int minor;
  } fit_revision;

  struct {
    int major;
    int minor;
  } snd_revision;

  struct SndRange *rng;
};


struct SndRange { /* fitted parameters for a single range */
  int qflg;
  int gsct;
  double v;
  double v_err;
  double p_l;
  double w_l;
  int x_qflg;
  double phi0;
  double phi0_err;
};


struct SndData *SndMake();
void SndFree(struct SndData *ptr);
int SndSetOriginTime(struct SndData *ptr,char *str);
int SndSetOriginCommand(struct SndData *ptr,char *str);
int SndSetCombf(struct SndData *ptr,char *str);
int SndSetRng(struct SndData *ptr,int nrang);

#endif
