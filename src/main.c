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

  drawLine(1000, 500, 1000, 700, 0xFF, 0x00, 0x00);
  drawLine(1250, 500, 1250, 700, 0x00, 0xFF, 0x00);
  drawLine(900, 600, 1000, 800, 0x00, 0x00, 0xFF);
  drawLine(1000, 800, 1250, 800, 0x00, 0x00, 0xFF);
  drawLine(1250, 800, 1350, 600, 0x00, 0x00, 0xFF);  

  return 0;
}

/*********************/
/* Drawing utilities */
/*********************/

void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
  long loc = (x+(&vinfo)->xoffset) * ((&vinfo)->bits_per_pixel/8) + (y+(&vinfo)->yoffset) * (&finfo)->line_length; 
  *((uint32_t*)(fbp + loc)) = pixelColor(r, g, b);
}

void drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) {

  // Calculate slope of src and dest
  float realSlope = slope(x1, y1, x2, y2); 

  // Fill source and destination
  drawPixel(x1, y1, r, g, b);
  drawPixel(x2, y2, r, g, b);

  // While we haven't gotten adjacent to the destination
  while (!isDestAdjacent(x1, y1, x2, y2)) {
    
    float minSlopeDiff = INFINITY;
    int minSlopeX, minSlopeY;    

    // Find the px with the least slope 
    if (x1 < x2 && y1 < y2) {
      float slopeDiffs[3];
      slopeDiffs[0] = fabsf(slope(x1+1, y1, x2, y2) - realSlope);
      slopeDiffs[1] = fabsf(slope(x1, y1+1, x2, y2) - realSlope);
      slopeDiffs[2] = fabsf(slope(x1+1, y1+1, x2, y2) - realSlope);
     
      if (slopeDiffs[0] < slopeDiffs[1] && slopeDiffs[0] < slopeDiffs[2]) {
        minSlopeX = x1+1;
	minSlopeY = y1;
      }
      else if (slopeDiffs[1] < slopeDiffs[2]) {
        minSlopeX = x1;
	minSlopeY = y1+1;
      }
      else {
        minSlopeX = x1+1;
	minSlopeY = y1+1;
      }
    }
    else if (x1 > x2 && y1 < y2) {
      float slopeDiffs[3];
      slopeDiffs[0] = fabsf(slope(x1-1, y1, x2, y2) - realSlope);
      slopeDiffs[1] = fabsf(slope(x1, y1+1, x2, y2) - realSlope);
      slopeDiffs[2] = fabsf(slope(x1-1, y1+1, x2, y2) - realSlope);
      
      if (slopeDiffs[0] < slopeDiffs[1] && slopeDiffs[0] < slopeDiffs[2]) {
        minSlopeX = x1-1;
	minSlopeY = y1;
      }
      else if (slopeDiffs[1] < slopeDiffs[2]) {
        minSlopeX = x1;
	minSlopeY = y1+1;
      }
      else {
        minSlopeX = x1-1;
	minSlopeY = y1+1;
      }
    }
    else if (x1 > x2 && y1 > y2) {
      float slopeDiffs[3];
      slopeDiffs[0] = fabsf(slope(x1-1, y1, x2, y2) - realSlope);
      slopeDiffs[1] = fabsf(slope(x1, y1-1, x2, y2) - realSlope);
      slopeDiffs[2] = fabsf(slope(x1-1, y1-1, x2, y2) - realSlope);
      
      if (slopeDiffs[0] < slopeDiffs[1] && slopeDiffs[0] < slopeDiffs[2]) {
        minSlopeX = x1-1;
	minSlopeY = y1;
      }
      else if (slopeDiffs[1] < slopeDiffs[2]) {
        minSlopeX = x1;
	minSlopeY = y1-1;
      }
      else {
        minSlopeX = x1-1;
	minSlopeY = y1-1;
      }
    }
    else if (x1 < x2 && y1 > y2) {
      float slopeDiffs[3];
      slopeDiffs[0] = fabsf(slope(x1+1, y1, x2, y2) - realSlope);
      slopeDiffs[1] = fabsf(slope(x1, y1-1, x2, y2) - realSlope);
      slopeDiffs[2] = fabsf(slope(x1+1, y1-1, x2, y2) - realSlope);
      
      if (slopeDiffs[0] < slopeDiffs[1] && slopeDiffs[0] < slopeDiffs[2]) {
        minSlopeX = x1+1;
	minSlopeY = y1;
      }
      else if (slopeDiffs[1] < slopeDiffs[2]) {
        minSlopeX = x1;
	minSlopeY = y1-1;
      }
      else {
        minSlopeX = x1+1;
	minSlopeY = y1-1;
      }
    }
    else if (x1 < x2) {
      minSlopeX = x1+1;
      minSlopeY = y1;
    }
    else if (x1 > x2) {
      minSlopeX = x1-1;
      minSlopeY = y1;
    }
    else if (y1 < y2) {
      minSlopeX = x1;
      minSlopeY = y1+1;
    }
    else if (y1 > y2) {
      minSlopeX = x1;
      minSlopeY = y1-1;
    }

    x1 = minSlopeX;
    y1 = minSlopeY;

    // Color the px with the min diff
    drawPixel(x1, y1, r, g, b);

  }

}	

/*****************/
/* Miscellaneous */
/*****************/

float slope(int x1, int y1, int x2, int y2) {
  if (x1 == x2) {
    return INFINITY;
  }
  else {
    return (float)(y2 - y1) / (float)(x2 - x1);
  }
}

// Checks if destination px is adjacent to source px
int isDestAdjacent(int x1, int y1, int x2, int y2) {
  return (abs(x1 - x2) <= 1) && (abs(y1 - y2) <= 1);
}

// Convert rbg value to pixel value
uint32_t pixelColor(uint8_t r, uint8_t g, uint8_t b) {
  return (r<<(&vinfo)->red.offset) | (g<<(&vinfo)->green.offset) | (b<<(&vinfo)->blue.offset);
}

void delay(int number_of_seconds) 
{ 
  int milli_seconds = 1000 * number_of_seconds; 
  
  // Storing start time 
  clock_t start_time = clock(); 
  	
  // Looping till required time is acheived 
  while (clock() < start_time + milli_seconds) {} 
}
