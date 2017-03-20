/* color.c
   ======= 
   Author: R.J.Barnes
*/

/*
   See license.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rfbuffer.h"
#include "rps.h"




unsigned int PostScriptColor(int r,int g,int b) {
  return (r<<16) | (g<<8) | b;
}

