/* sndidl.h
   ========
   Author E.G.Thomas
*/


#ifndef _SNDIDL_H
#define _SNDIDL_H


#define MAX_RANGE 300


struct SndIDLData {
  struct {
    UCHAR major;
    UCHAR minor;
  } radar_revision;

  short cp;
  short stid;

  struct {
    short yr;
    short mo;
    short dy;
    short hr;
    short mt;
    short sc;
    IDL_LONG us;
  } time;

  short nave;
  short lagfr;
  short smsep;

  struct {
    float search;
    float mean;
  } noise;

  short bmnum;
  float bmazm;
  short scan;
  short rxrise;

  struct {
    short sc;
    IDL_LONG us;
  } intt;

  short nrang;
  short frang;
  short rsep;
  short xcf;
  short tfreq;

  float sky_noise;

  struct {
    IDL_LONG major;
    IDL_LONG minor;
  } snd_revision;

  UCHAR qflg[MAX_RANGE]; 
  UCHAR gflg[MAX_RANGE]; 
  float v[MAX_RANGE];
  float v_e[MAX_RANGE];
  float w_l[MAX_RANGE];
  float p_l[MAX_RANGE]; 
  UCHAR x_qflg[MAX_RANGE];
  float phi0[MAX_RANGE];
  float phi0_e[MAX_RANGE];
};


void IDLCopySndDataToIDL(int nrang, struct SndData *snd,
                         struct SndIDLData *isnd);

void IDLCopySndDataFromIDL(int nrang, struct SndIDLData *isnd,
                           struct SndData *snd);

struct SndIDLData *IDLMakeSndData(IDL_VPTR *vptr);

#endif
