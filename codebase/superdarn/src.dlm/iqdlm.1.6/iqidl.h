/* iqidl.h
   ======== 
   Author R.J.Barnes
*/

/*
   See license.txt
*/

#ifndef _IQIDL_H
#define _IQIDL_H


#define MAXNAVE 300

struct IQIDL {
  struct {
    IDL_LONG major;
    IDL_LONG minor;
  } revision;
  IDL_LONG chnnum;
  IDL_LONG smpnum;
  IDL_LONG skpnum;
  IDL_LONG seqnum;
  IDL_LONG tbadtr;
  struct {
    IDL_LONG sec;
    IDL_LONG nsec;
  } tval[MAXNAVE]; 
  short atten[MAXNAVE];
  float noise[MAXNAVE];
  IDL_LONG offset[MAXNAVE];
  IDL_LONG size[MAXNAVE];
  IDL_LONG badtr[MAXNAVE];
};

struct IQIDLInx {
  double time;
  IDL_LONG offset;
};

void IDLCopyIQToIDL(struct IQ *iq,struct IQIDL *iiq);

void IDLCopyIQFromIDL(struct IQIDL *iiq,struct IQ *iq);

struct IQIDL *IDLMakeIQ(IDL_VPTR *vptr);

struct IQIDLInx *IDLMakeIQInx(int num,IDL_VPTR *vptr);


#endif
