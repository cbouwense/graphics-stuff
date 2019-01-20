#include "main.h" 

int main() {

  fb_fd = open("/dev/fb0", O_RDWR);

  // Get variable screen info
  ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

  vinfo.grayscale = 0; vinfo.bits_per_pixel = 32;

  ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo); ioctl(fb_fd, FBIOGET_VSCREENINFO,
		  &vinfo); ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

  long screensize = finfo.smem_len;

  // Map the buffer to memory, returns ptr to memory
  fbp = mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd,
		  (off_t)0);

  /* Tests */

  Color red = makeColor(0xFF, 0x00, 0x00); Pixel p1 = makePixel(1000, 500,
		  red); Pixel p2 = makePixel(1500, 500, red); Pixel p3 =
	  makePixel(1000, 750, red); Line l1 = makeLine(p1, p2, red); Line l2 =
	  makeLine(p2, p3, red); Line l3 = makeLine(p3, p1, red); Triangle t1 =
	  makeTriangle(l1, l2, l3, red);
 
  drawTriangle(t1);

  return 0; }

/*********************/
/* Drawing utilities */
/*********************/


void drawPixel(Pixel p) { long loc = (p->x+(&vinfo)->xoffset) *
	((&vinfo)->bits_per_pixel/8) + (p->y+(&vinfo)->yoffset) *
		(&finfo)->line_length; *((uint32_t*)(fbp + loc)) =
		pixelColor(p->c); }

void drawLine(Line l) {
  
  Pixel p1 = l->p1; Pixel p2 = l->p2;

  // Fill source and destination
  drawPixel(p1); drawPixel(p2);

  int rise = p2->y - p1->y; int run = p2->x - p1->x;
 
  int riseParity = (rise < 0) ? -1 : 1; int runParity = (run < 0) ? -1 : 1;
  
  if (rise == 0) { 
    for (int i = p1->x; i != p2->x; i += runParity) {
      drawPixel(makePixel(i, p1->y, l->c)); 
    } 
  } 
  else if (run == 0) {
    printf("found run to be 0\n");
    for (int i = p1->y; i != p2->y; i += riseParity) {
      drawPixel(makePixel(p1->x, i, l->c));
    }
  }
  else {
    int slopeGcd = gcd(rise, run);
    if (slopeGcd != 0 && slopeGcd != INFINITY) {
      rise /= slopeGcd;
      run /= slopeGcd;
    }

    int x = p1->x;
    int y = p1->y;
  
    // While we haven't gotten to the dest yet
    while (!isAdjacent(makePixel(x, y, NULL), p2)) {
      for (int i = 0; i < abs(rise); i++) {
        for (int j = 0; j < abs(run); j++) {
          drawPixel(makePixel(x+(j*runParity), y+(i*riseParity), l->c));
        }
      }
      y += rise;
      x += run;
    }
  }
}	

void drawTriangle(Triangle t) {
  drawLine(t->l1);
  drawLine(t->l2);
  drawLine(t->l3);
}

void fillTriangle(Triangle t) {
  Pixel p1 = t->l1->p1;
  Pixel p2 = t->l1->p2;
  Pixel p3 = t->l2->p2;

  Pixel nextP1P2 = p1;
  Pixel nextP1P3 = p1;  
  
  float slopeP1P2 = slope(p1, p2);
  float slopeP1P3 = slope(p1, p3);
  /*
  do {
    if (!isAdjacent(nextP1P2, p2)) {
      printf("nextP1 not adjacent to p2\n");
      nextP1P2 = nextPixelInSlope(nextP1P2, slopeP1P2);
    }
    if (!isAdjacent(nextP1P3, p3)) {
      printf("nextP1 not adjacent to p3\n");
      nextP1P3 = nextPixelInSlope(nextP1P3, slopeP1P3);
    }
    drawLine(makeLine(nextP1P2, nextP1P3, t->c));
  } while(!isAdjacent(nextP1P2, p2) || !isAdjacent(nextP1P3, p3));
  */
}

Triangle rotateTriangle(Triangle t, float rad, Line axis, int direction) {
  return NULL;     
}

/*****************/
/* Miscellaneous */
/*****************/

float dist(Pixel p1, Pixel p2) {
  return sqrt(pow(p2->x - p1->x, 2.0) + pow(p2->y - p1->y, 2.0));
}

int pixelInLine(Pixel p, Line l) {
  return slope(p, l->p1) == slope(p, l->p2);
}

float pixelToLineDist(Pixel p, Line l) {
  float lineSlope = slope(l->p1, l->p2);
  
  if (lineSlope == 0.0) {
    return fabsf(l->p1->y - p->y);
  }
  else if (lineSlope == INFINITY || lineSlope == -INFINITY) {
    return fabsf(l->p1->x - p->x);
  }
  
  float distSlope = -1 / lineSlope;
  printf("slope: %f\ndistSlope: %f\n", lineSlope, distSlope); 
  /*
  Pixel probe = minSlope(;
  while (!pixelInLine(p, l
  */
}

Color makeColor(uint8_t r, uint8_t g, uint8_t b) {
  Color c = (Color)malloc(sizeof(struct color));
  c->r = r;
  c->g = g;
  c->b = b;
  return c;
}

Pixel makePixel(int x, int y, Color c) {
  Pixel p = (Pixel)malloc(sizeof(struct pixel));
  p->x = x;
  p->y = y;
  p->c = c;
  return p;
}

Line makeLine(Pixel p1, Pixel p2, Color c) {
  Line l = (Line)malloc(sizeof(struct line));
  l->p1 = p1;
  l->p2 = p2;
  l->c = c;
  return l;
}

Triangle makeTriangle(Line l1, Line l2, Line l3, Color c) {
  Triangle t = (Triangle)malloc(sizeof(struct triangle));
  t->l1 = l1;
  t->l2 = l2;
  t->l3 = l3;
  t->c = c;
  return t;
}

float slope(Pixel p1, Pixel p2) {
  if (p1->x == p2->x) {
    return INFINITY;
  }
  else {
    return (float)(p2->y - p1->y) / (float)(p2->x - p1->x);
  }
}

// Calculate the greatest common divisor
int gcd(int a, int b) {
  if (a < 0) {
    a = -a;
  }
  if (b < 0) {
    b = -b;
  }
  printf("getting the gcd of %d and %d\n", a, b);
  int res = 0;
  for (int i = 1; i <= a && i <= b; i++) {
    if (a % i == 0 && b % i == 0) {
      res = i;
    }
  }
  printf("found gcd to be %d\n", res); 
  return res;
}

// Checks if destination px is adjacent to source px
int isAdjacent(Pixel p1, Pixel p2) {
  return (abs(p1->x - p2->x) <= 1) && (abs(p1->y - p2->y) <= 1);
}

// Convert rbg value to pixel value
uint32_t pixelColor(Color c) {
  if (c)
    return (c->r<<(&vinfo)->red.offset) | (c->g<<(&vinfo)->green.offset) | (c->b<<(&vinfo)->blue.offset);
  return 0;
}

void delay(int number_of_seconds) 
{ 
  int milli_seconds = 1000 * number_of_seconds; 
  
  // Storing start time 
  clock_t start_time = clock(); 
  	
  // Looping till required time is acheived 
  while (clock() < start_time + milli_seconds) {} 
}
