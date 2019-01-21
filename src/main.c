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

  Color red = makeColor(0xFF, 0x00, 0x00); 
  Color blue = makeColor(0x00, 0x00, 0xFF);
  Pixel p1 = makePixel(1000, 500, red); 
  Pixel p2 = makePixel(1500, 500, red); 
  Pixel p3 = makePixel(1200, 250, red); 
  Line l1 = makeLine(p1, p2, red); 
  Line l2 = makeLine(p2, p3, red); 
  Line l3 = makeLine(p3, p1, red); 
  Triangle t1 = makeTriangle(l1, l2, l3, red);

  //drawLine(makeLine(l3->p1, l3->p2, blue));

  fillTriangle(t1);


  return 0; }

/*********************/
/* Drawing utilities */
/*********************/


void drawPixel(Pixel p) { long loc = (p->x+(&vinfo)->xoffset) *
	((&vinfo)->bits_per_pixel/8) + (p->y+(&vinfo)->yoffset) *
		(&finfo)->line_length; *((uint32_t*)(fbp + loc)) =
		pixelColor(p->c); }

void drawLineLow(Line l) {
  int dx = l->p2->x - l->p1->x;
  int dy = l->p2->y - l->p1->y;
  int yi = 1;
  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }
  int D = 2*dy - dx;
  int y = l->p1->y;

  Pixel leftmost = (l->p1->x < l->p2->x) ? l->p1 : l->p2;
  Pixel rightmost = (leftmost == l->p1) ? l->p2 : l->p1;

  for (int x = leftmost->x; x < rightmost->x; x++) {
    drawPixel(makePixel(x, y, l->c)); 
    if (D > 0) {
      y += yi;
      D -= 2*dx;
    }
    D += 2*dy;
  }
}

void drawLineHigh(Line l) {
  int dx = l->p2->x - l->p1->x;
  int dy = l->p2->y - l->p1->y;
  int xi = 1;
  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }
  int D = 2*dx - dy;
  int x = l->p1->x;

  Pixel lower = (l->p1->y < l->p2->y) ? l->p1 : l->p2;
  Pixel higher = (lower == l->p1) ? l->p2 : l->p1;

  for (int y = lower->y; y < higher->y; y++) {
    drawPixel(makePixel(x, y, l->c)); 
    if (D > 0) {
      x += xi;
      D -= 2*dy;
    }
    D += 2*dx;
  }
}

void drawLine(Line l) {
  int x0 = l->p1->x;
  int y0 = l->p1->y;
  int x1 = l->p2->x;
  int y1 = l->p2->y;

  if (abs(y1 - y0) < abs(x1 - x0)) {
    if (x0 > x1) {
      drawLineLow(makeLine(l->p2, l->p1, l->c));
    }
    else {
      drawLineLow(l);
    }
  }
  else {
    if (y0 > y1) {
      drawLineHigh(makeLine(l->p2, l->p1, l->c));
    }
    else {
      drawLineHigh(l);
    }
  }  
}	

void drawTriangle(Triangle t) {
  drawLine(t->l1);
  drawLine(t->l2);
  drawLine(t->l3);
}

void fillTriangle(Triangle t) {
  
  // Find the hypotenuese
  Line hypot = t->l1;
  
  if (dist(t->l1->p1, t->l1->p2) < dist(t->l2->p1, t->l2->p2)) {
    hypot = t->l2;
  }

  if (dist(hypot->p1, hypot->p2) < dist(t->l3->p1, t->l3->p2)) {
    hypot = t->l3;
  }

  // Find direction to draw
  Pixel rightmost = (hypot->p1->x > hypot->p2->x) ? hypot->p1 : hypot->p2;
  Pixel leftmost = (hypot->p1->x > hypot->p2->x) ? hypot->p2 : hypot->p1;
  printf("hypot: %p\nt->l1: %p\nt->l2: %p\nt->l3: %p\n", hypot, t->l1, t->l2, t->l3); 
  
  // Go left to right, shoot rays down
  if (t->l1->p1->y < hypot->p1->y && t->l1->p1->y < hypot->p2->y || 
      t->l1->p2->y < hypot->p1->y && t->l1->p2->y < hypot->p2->y ||
      t->l2->p1->y < hypot->p1->y && t->l2->p1->y < hypot->p2->y ||
      t->l2->p2->y < hypot->p1->y && t->l2->p2->y < hypot->p2->y ||
      t->l3->p1->y < hypot->p1->y && t->l3->p1->y < hypot->p2->y || 
      t->l3->p2->y < hypot->p1->y && t->l3->p2->y < hypot->p2->y) {
    
    int rise = hypot->p2->y - hypot->p1->y; 
    int run = hypot->p2->x - hypot->p1->x;
    int riseParity = (rise < 0) ? -1 : 1; 
    int slopeGcd = gcd(rise, run);
    if (slopeGcd != 0 && slopeGcd != INFINITY) {
      rise /= slopeGcd;
      run /= slopeGcd;
    }
  
    printf("rise: %d\nrun: %d\n", rise, run);

    Pixel rayBeam = makePixel(leftmost->x, leftmost->y, NULL);
    rayBeam->x++;

    int origRayBeamY;
    for (int i = rayBeam->x; i < rightmost->x; i++) {
      origRayBeamY = rayBeam->y;
	    
      // shoot ray down
      if (hypot == t->l1) {
	printf("rayBeam: (%d, %d)\n", rayBeam->x, rayBeam->y);
	printf("in l2? %d\n", pixelInLine(rayBeam, t->l2));
	printf("in l3? %d\n", pixelInLine(rayBeam, t->l3));
        while (!pixelInLine(rayBeam, t->l2) && !pixelInLine(rayBeam, t->l3)) {
          printf("drew pixel at (%d, %d)\n", rayBeam->x, rayBeam->y);
	  drawPixel(makePixel(rayBeam->x, rayBeam->y, t->c));
          rayBeam->y--;  
        }
      }
      else if (hypot == t->l2) {
        while (!pixelInLine(makePixel(rayBeam->x, rayBeam->y, NULL), t->l1) || 
	       !pixelInLine(makePixel(rayBeam->x, rayBeam->y, NULL), t->l3)) {
          drawPixel(makePixel(rayBeam->x, rayBeam->y, t->c));
        }
      }
      else {
        while (!pixelInLine(makePixel(rayBeam->x, rayBeam->y, NULL), t->l1) || 
	       !pixelInLine(makePixel(rayBeam->x, rayBeam->y, NULL), t->l2)) {
          drawPixel(makePixel(rayBeam->x, rayBeam->y, t->c));
        }
      }
      
      // Move rayBeam on the y axis if need be
      if (rayBeam->x % run == 0) {
        rayBeam->y = origRayBeamY + riseParity;  
      }

      // Move rayBeam to the right
      rayBeam->x += 1;  

    }

  }
  // Go from top to bottom, shoot rays left
  else if (t->l1->p1->x < hypot->p1->x && t->l1->p1->x < hypot->p2->x ||
           t->l1->p2->x < hypot->p1->x && t->l1->p2->x < hypot->p2->x ||
           t->l2->p1->x < hypot->p1->x && t->l2->p1->x < hypot->p2->x ||
           t->l2->p2->x < hypot->p1->x && t->l2->p2->x < hypot->p2->x ||
           t->l3->p1->x < hypot->p1->x && t->l3->p1->x < hypot->p2->x || 
           t->l3->p2->x < hypot->p1->x && t->l3->p2->x < hypot->p2->x) {
    
    // do stuff

  }
  // Go from top to bottom, shoot rays right
  else if (t->l1->p1->x > hypot->p1->x && t->l1->p1->x > hypot->p2->x ||
           t->l1->p2->x > hypot->p1->x && t->l1->p2->x > hypot->p2->x ||  
           t->l2->p1->x > hypot->p1->x && t->l2->p1->x > hypot->p2->x ||
           t->l2->p2->x > hypot->p1->x && t->l2->p2->x > hypot->p2->x ||
           t->l3->p1->x > hypot->p1->x && t->l3->p1->x > hypot->p2->x ||
           t->l3->p2->x > hypot->p1->x && t->l3->p2->x > hypot->p2->x) {
    
    // do stuff

  }
  // Go from left to right, shoot rays up
  else {

  }

  /*
  // Draw horizontal lines in that direction from hypotenuese
  int hypotParity = (hypot->p1->y < hypot->p2->y) ? 1 : -1;
  int xMod = 0;
  for (int i = 0; i < abs(hypot->p1->y - hypot->p2->y); i += hypotParity) {
    int xMod = 0;
    if (dirIsRight) {
      if (hypot == t->l1) {
        while (!pixelInLine(makePixel(hypot->p1->x+xMod, hypot->p1->y+i, NULL), t->l2) || 
	       !pixelInLine(makePixel(hypot->p1->x+xMod, hypot->p1->y+i, NULL), t->l3)) {
          endpoint = makePixel(hypot->p1->x+xMod, hypot->p1->y+1, NULL);
        }
      }
      else if (hypot == t->l2) {
        while (!pixelInLine(makePixel(hypot->p1->x+xMod, hypot->p1->y+i, NULL), t->l1) || 
	       !pixelInLine(makePixel(hypot->p1->x+xMod, hypot->p1->y+i, NULL), t->l3)) {
          endpoint = makePixel(hypot->p1->x+xMod, hypot->p1->y+1, NULL);
        }
      }
      else {
        while (!pixelInLine(makePixel(hypot->p1->x+xMod, hypot->p1->y+i, NULL), t->l1) || 
	       !pixelInLine(makePixel(hypot->p1->x+xMod, hypot->p1->y+i, NULL), t->l2)) {
          endpoint = makePixel(hypot->p1->x+xMod, hypot->p1->y+1, NULL);
        }
      }
    }
    else {

    }

  }
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
  printf("p: (%d, %d)\n", p->x, p->y);
  printf("p1: (%d, %d)\n", l->p1->x, l->p1->y);
  printf("p2: (%d, %d)\n", l->p2->x, l->p2->y);
  
  if (p->x == l->p1->x && p->y == l->p1->y ||
      p->x == l->p2->x && p->y == l->p2->y) {
    
    return 1;
  } 
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
