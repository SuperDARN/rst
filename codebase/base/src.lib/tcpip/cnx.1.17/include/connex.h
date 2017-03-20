/* connex.h
   ========
   Author: R.J.Barnes
*/

/*
   See license.txt
*/



#ifndef _CONNEX_H
#define _CONNEX_H

#define UNKNOWN_HOST 1
#define OPEN_FAIL 2
#define CONNECT_FAIL 3

extern int ConnexErr;

int ConnexOpen(char *host,int remote_port,struct timeval *tmout);
void ConnexClose(int msgin_sock);
int ConnexRead(int num,int *sock,unsigned char **buffer,int *size,int *flag,
		struct timeval *tout); 

#define MSG_ID 0x001e0849
#define MSGERR_PIPE_CLOSED 0x00
#define MSGERR_OUT_OF_SYNC 0x01
#define MSGERR_NOMEM 0x02

int ConnexWriteMem(unsigned char *outbuf,unsigned int outsize,
                   unsigned char *buffer,unsigned int size);
int ConnexReadMem(unsigned char *inbuf,
		 unsigned int insize,unsigned char **buffer);
int ConnexWriteIP(int fildes,unsigned char *buffer,unsigned int size);
int ConnexReadIP(int fildes,unsigned char **buffer);
    
#endif








