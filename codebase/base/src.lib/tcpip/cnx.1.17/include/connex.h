/* connex.h
   ========
   Author: R.J.Barnes
 LICENSE AND DISCLAIMER
 
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








