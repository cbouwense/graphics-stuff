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

#define CLOCKWISE 1
#define COUNTER_CLOCKWISE -1

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
  Color c;
};
typedef struct pixel* Pixel;

struct line {
  Pixel p1, p2;
  Color c;
};
typedef struct line* Line;

struct triangle {
  Line l1, l2, l3;
  Color c;
};
typedef struct triangle* Triangle;

int fb_fd;
uint8_t* fbp;      

/*********************/
/* Drawing Utilities */
/*********************/

void drawPixel(Pixel p);
void drawLine(Line l);
void drawTriangle(Triangle t);
void fillTriangle(Triangle t);
Triangle rotateTriangle(Triangle t, float rad, Line axis, int direction);

/*****************/
/* Miscellaneous */
/*****************/


float dist(Pixel p1, Pixel p2);
int pixelInLine(Pixel p, Line l);
float pixelToLineDist(Pixel p, Line l);
Color makeColor(uint8_t r, uint8_t g, uint8_t b);
Pixel makePixel(int x, int y, Color c);
Line makeLine(Pixel p1, Pixel p2, Color c);
Triangle makeTriangle(Line l1, Line l2, Line l3, Color c);

Pixel minSlope(Pixel p1, Pixel p2);
float slope(Pixel p1, Pixel p2);
int isAdjacent(Pixel p1, Pixel p2);
uint32_t pixelColor(Color c);
void delay(int number_of_seconds); 
