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

  Pixel p1 = makePixel(1000, 500);
  Pixel p2 = makePixel(1500, 500);
  Pixel p3 = makePixel(1000, 750);

  Color red = makeColor(0xFF, 0x00, 0x00);

  fillTriangle(p1, p2, p3, red);

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


void drawPixel(Pixel p, Color c) {
  long loc = (p->x+(&vinfo)->xoffset) * ((&vinfo)->bits_per_pixel/8) + (p->y+(&vinfo)->yoffset) * (&finfo)->line_length; 
  *((uint32_t*)(fbp + loc)) = pixelColor(c);
}

void drawLine(Pixel p1, Pixel p2, Color c) {

  // Fill source and destination
  drawPixel(p1, c);
  drawPixel(p2, c);

  // While we haven't gotten adjacent to the destination
  while (!isAdjacent(p1, p2)) {
    
    float minSlopeDiff = INFINITY;

    // Find the px with the least slope 
    Pixel nextP = minSlope(p1, p2);

    // Color the px with the min diff
    drawPixel(nextP, c);

    p1 = nextP;

  }

}	

void drawTriangle(Pixel p1, Pixel p2, Pixel p3, Color c) {
  drawLine(p1, p2, c);
  drawLine(p2, p3, c);
  drawLine(p3, p1, c);
}

void fillTriangle(Pixel p1, Pixel p2, Pixel p3, Color c) {
  Pixel nextP1P2 = p1;
  Pixel nextP1P3 = p1;  
  do {
    if (!isAdjacent(nextP1P2, p2)) {
      nextP1P2 = minSlope(nextP1P2, p2);
    }
    if (!isAdjacent(nextP1P3, p3)) {
      nextP1P3 = minSlope(nextP1P3, p3);
    }
    drawLine(nextP1P2, nextP1P3, c);
  } while(!isAdjacent(nextP1P2, p2) || !isAdjacent(nextP1P3, p3));
}

/*****************/
/* Miscellaneous */
/*****************/

Pixel minSlope(Pixel p1, Pixel p2) {
 
  float realSlope = slope(p1, p2);

  if (p1->x < p2->x && p1->y < p2->y) {
    float slopeDiffs[3];
    Pixel tempP1 = makePixel(p1->x+1, p1->y);
    Pixel tempP2 = makePixel(p1->x, p1->y+1);
    Pixel tempP3 = makePixel(p1->x+1, p1->y+1);
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
    Pixel tempP1 = makePixel(p1->x-1, p1->y);
    Pixel tempP2 = makePixel(p1->x, p1->y+1);
    Pixel tempP3 = makePixel(p1->x-1, p1->y+1);
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
    Pixel tempP1 = makePixel(p1->x-1, p1->y);
    Pixel tempP2 = makePixel(p1->x, p1->y-1);
    Pixel tempP3 = makePixel(p1->x-1, p1->y-1);
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
    Pixel tempP1 = makePixel(p1->x+1, p1->y);
    Pixel tempP2 = makePixel(p1->x, p1->y-1);
    Pixel tempP3 = makePixel(p1->x+1, p1->y-1);
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
    return makePixel(p1->x+1, p1->y);
  }
  else if (p1->x > p2->x) {
    return makePixel(p1->x-1, p1->y);
  }
  else if (p1->y < p2->y) {
    return makePixel(p1->x, p1->y+1);
  }
  else if (p1->y > p2->y) {
    return makePixel(p1->x, p1->y-1);
  }
}

Color makeColor(uint8_t r, uint8_t g, uint8_t b) {
  Color c = (Color)malloc(sizeof(struct color));
  c->r = r;
  c->g = g;
  c->b = b;
  return c;
}

Pixel makePixel(int x, int y) {
  Pixel p = (Pixel)malloc(sizeof(struct pixel));
  p->x = x;
  p->y = y;
  return p;
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
  return (c->r<<(&vinfo)->red.offset) | (c->g<<(&vinfo)->green.offset) | (c->b<<(&vinfo)->blue.offset);
}

void delay(int number_of_seconds) 
{ 
  int milli_seconds = 1000 * number_of_seconds; 
  
  // Storing start time 
  clock_t start_time = clock(); 
  	
  // Looping till required time is acheived 
  while (clock() < start_time + milli_seconds) {} 
}
