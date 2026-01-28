#ifndef MY_CHAR_H
#define MY_CHAR_H

#include <asm/ioctl.h>

#define MY_CHAR_MAGIC 'K'
#define MY_CHAR_IOCTL_GET_MAXLEN _IOR(MY_CHAR_MAGIC,0,int*)
#define MY_CHAR_IOCTL_GET_CURLEN _IOR(MY_CHAR_MAGIC,1,int*)


#endif
