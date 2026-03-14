#ifndef FS4412_LEDS_H
#define FS4412_LEDS_H
#include <linux/ioctl.h>

#define LEDS_MAGIC 'K'

#define LED_ON _IOW(LEDS_MAGIC,0,unsigned char)
#define LED_OFF _IOW(LEDS_MAGIC,1,unsigned char)

#endif