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

  drawPixel(10, 10, 0x00, 0xFF, 0x00);

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

  // Calculate slope
  float slope = (y2 - y1) / (x2 - x1);

  // Fill source and destination
  drawPixel(x1, y1, r, g, b);

}	

/*****************/
/* Miscellaneous */
/*****************/

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
