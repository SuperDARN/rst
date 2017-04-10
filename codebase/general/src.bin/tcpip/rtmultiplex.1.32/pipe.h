/* pipe.h
   ======
   Author: R.J.Barnes
*/

/*
   See license.txt
*/




int pipewrite(int fildes,unsigned char *buffer,unsigned int size);
int piperead(int fildes,unsigned char **buffer);
