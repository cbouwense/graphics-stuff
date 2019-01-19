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

struct color {
  uint8_t r, g, b;
};
typedef struct color* Color;

struct pixel {
  int x, y;
};
typedef struct pixel* Pixel;

int fb_fd;
uint8_t* fbp;      

/*********************/
/* Drawing Utilities */
/*********************/

void drawPixel(Pixel p, Color c);
void drawLine(Pixel p1, Pixel p2, Color c);
void drawTriangle(Pixel p1, Pixel p2, Pixel p3, Color c);
void fillTriangle(Pixel p1, Pixel p2, Pixel p3, Color c);

/*****************/
/* Miscellaneous */
/*****************/

Pixel minSlope(Pixel p1, Pixel p2);
Color makeColor(uint8_t r, uint8_t g, uint8_t b);
Pixel makePixel(int x, int y);
float slope(Pixel p1, Pixel p2);
int isAdjacent(Pixel p1, Pixel p2);
uint32_t pixelColor(Color c);
void delay(int number_of_seconds); 
