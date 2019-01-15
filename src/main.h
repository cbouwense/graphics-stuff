#include <assert.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <time.h>

/***********/
/* Globals */
/***********/

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
int fb_fd;
uint8_t* fbp;      

/*********************/
/* Drawing Utilities */
/*********************/

void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b);

/*****************/
/* Miscellaneous */
/*****************/

float slope(int x1, int y1, int x2, int y2);
int isDestAdjacent(int x1, int y1, int x2, int y2);
uint32_t pixelColor(uint8_t r, uint8_t g, uint8_t b);
void delay(int number_of_seconds); 
