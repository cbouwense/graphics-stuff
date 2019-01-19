#include "main.h" 

int main() {

  fb_fd = open("/dev/fb0", O_RDWR);

  // Get variable screen info
  ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

  vinfo.grayscale = 0;
  vinfo.bits_per_pixel = 32;

  ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
  ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
  ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

  long screensize = finfo.smem_len;

  // Map the buffer to memory, returns ptr to memory
  fbp = mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);

  /* Tests */

  Color red = makeColor(0xFF, 0x00, 0x00);
  Pixel p1 = makePixel(1000, 500, red);
  Pixel p2 = makePixel(1500, 500, red);
  Pixel p3 = makePixel(1000, 750, red);
  Line l1 = makeLine(p1, p2, red);
  Line l2 = makeLine(p2, p3, red);
  Line l3 = makeLine(p3, p1, red);
  Triangle t1 = makeTriangle(l1, l2, l3, red);
  fillTriangle(t1);

  /*
  drawLine(1000, 500, 1000, 700, 0xFF, 0x00, 0x00);
  drawLine(1250, 500, 1250, 700, 0x00, 0xFF, 0x00);
  drawLine(900, 600, 1000, 800, 0x00, 0x00, 0xFF);
  drawLine(1000, 800, 1250, 800, 0x00, 0x00, 0xFF);
  drawLine(1250, 800, 1350, 600, 0x00, 0x00, 0xFF);  
  */

  return 0;
}

/*********************/
/* Drawing utilities */
/*********************/


void drawPixel(Pixel p) {
  long loc = (p->x+(&vinfo)->xoffset) * ((&vinfo)->bits_per_pixel/8) + (p->y+(&vinfo)->yoffset) * (&finfo)->line_length; 
  *((uint32_t*)(fbp + loc)) = pixelColor(p->c);
}

void drawLine(Line l) {
  
  Pixel p1 = l->p1;
  Pixel p2 = l->p2;
  // Fill source and destination
  drawPixel(p1);
  drawPixel(p2);
  // While we haven't gotten adjacent to the destination
  while (!isAdjacent(p1, p2)) {
    
    float minSlopeDiff = INFINITY;

    // Find the px with the least slope 
    Pixel nextP = minSlope(p1, p2);

    // Color the px with the min diff
    nextP->c = l->c;
    drawPixel(nextP);

    p1 = nextP;

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
  do {
    if (!isAdjacent(nextP1P2, p2)) {
      nextP1P2 = minSlope(nextP1P2, p2);
    }
    if (!isAdjacent(nextP1P3, p3)) {
      nextP1P3 = minSlope(nextP1P3, p3);
    }
    drawLine(makeLine(nextP1P2, nextP1P3, t->c));
  } while(!isAdjacent(nextP1P2, p2) || !isAdjacent(nextP1P3, p3));
}

void rotateTriangle(Triangle t, float rad, Line axis, int direction) {
   
}

/*****************/
/* Miscellaneous */
/*****************/

Pixel minSlope(Pixel p1, Pixel p2) {
 
  float realSlope = slope(p1, p2);

  if (p1->x < p2->x && p1->y < p2->y) {
    float slopeDiffs[3];
    Pixel tempP1 = makePixel(p1->x+1, p1->y, NULL);
    Pixel tempP2 = makePixel(p1->x, p1->y+1, NULL);
    Pixel tempP3 = makePixel(p1->x+1, p1->y+1, NULL);
    slopeDiffs[0] = fabsf(slope(tempP1, p2) - realSlope);
    slopeDiffs[1] = fabsf(slope(tempP2, p2) - realSlope);
    slopeDiffs[2] = fabsf(slope(tempP3, p2) - realSlope);
    
    if (slopeDiffs[0] < slopeDiffs[1] && slopeDiffs[0] < slopeDiffs[2]) {
      return tempP1;
    }
    else if (slopeDiffs[1] < slopeDiffs[2]) {
      return tempP2;
    }
    else {
      return tempP3;
    }
  }
  else if (p1->x > p2->x && p1->y < p2->y) {
    float slopeDiffs[3];
    Pixel tempP1 = makePixel(p1->x-1, p1->y, NULL);
    Pixel tempP2 = makePixel(p1->x, p1->y+1, NULL);
    Pixel tempP3 = makePixel(p1->x-1, p1->y+1, NULL);
    slopeDiffs[0] = fabsf(slope(tempP1, p2) - realSlope);
    slopeDiffs[1] = fabsf(slope(tempP2, p2) - realSlope);
    slopeDiffs[2] = fabsf(slope(tempP3, p2) - realSlope);
    
    if (slopeDiffs[0] < slopeDiffs[1] && slopeDiffs[0] < slopeDiffs[2]) {
      return tempP1;
    }
    else if (slopeDiffs[1] < slopeDiffs[2]) {
      return tempP2;
    }
    else {
      return tempP3;
    }
  }
  else if (p1->x > p2->x && p1->y > p2->y) {
    float slopeDiffs[3];
    Pixel tempP1 = makePixel(p1->x-1, p1->y, NULL);
    Pixel tempP2 = makePixel(p1->x, p1->y-1, NULL);
    Pixel tempP3 = makePixel(p1->x-1, p1->y-1, NULL);
    slopeDiffs[0] = fabsf(slope(tempP1, p2) - realSlope);
    slopeDiffs[1] = fabsf(slope(tempP2, p2) - realSlope);
    slopeDiffs[2] = fabsf(slope(tempP3, p2) - realSlope);
      
    if (slopeDiffs[0] < slopeDiffs[1] && slopeDiffs[0] < slopeDiffs[2]) {
      return tempP1;
    }
    else if (slopeDiffs[1] < slopeDiffs[2]) {
      return tempP2;
    }
    else {
      return tempP3;
    }
  }
  else if (p1->x < p2->x && p1->y > p2->y) {
    float slopeDiffs[3];
    Pixel tempP1 = makePixel(p1->x+1, p1->y, NULL);
    Pixel tempP2 = makePixel(p1->x, p1->y-1, NULL);
    Pixel tempP3 = makePixel(p1->x+1, p1->y-1, NULL);
    slopeDiffs[0] = fabsf(slope(tempP1, p2) - realSlope);
    slopeDiffs[1] = fabsf(slope(tempP1, p2) - realSlope);
    slopeDiffs[2] = fabsf(slope(tempP1, p2) - realSlope);
    
    if (slopeDiffs[0] < slopeDiffs[1] && slopeDiffs[0] < slopeDiffs[2]) {
      return tempP1;
    }
    else if (slopeDiffs[1] < slopeDiffs[2]) {
      return tempP2;
    }
    else {
      return tempP3;
    }
  }
  else if (p1->x < p2->x) {
    return makePixel(p1->x+1, p1->y, NULL);
  }
  else if (p1->x > p2->x) {
    return makePixel(p1->x-1, p1->y, NULL);
  }
  else if (p1->y < p2->y) {
    return makePixel(p1->x, p1->y+1, NULL);
  }
  else if (p1->y > p2->y) {
    return makePixel(p1->x, p1->y-1, NULL);
  }
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
