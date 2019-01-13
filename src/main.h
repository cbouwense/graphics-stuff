#include <assert.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <time.h>

uint32_t pixelColor(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo* vinfo); 

void delay(int number_of_seconds);

void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t* fbp,
	       struct fb_var_screeninfo* vinfo,
	       struct fb_fix_screeninfo* finfo);
