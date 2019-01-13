#include "main.h" 

int main() {

  struct fb_fix_screeninfo finfo;
  struct fb_var_screeninfo vinfo;

  int fb_fd = open("/dev/fb0", O_RDWR);

  // Get variable screen info
  ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

  vinfo.grayscale = 0;
  vinfo.bits_per_pixel = 32;

  ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
  ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
  ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

  long screensize = finfo.smem_len;

  // Map the buffer to memory, returns ptr to memory
  uint8_t* fbp = mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);

  int x, y;
  for (x = 0; x < vinfo.xres; x++ ) {
    for (y = 0; y < vinfo.yres; y++) {
      long location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
      *((uint32_t*)(fbp + location)) = pixelColor(0x00, 0xFF, 0x00, &vinfo);
    }
  }

  return 0;
}

/*********************/
/* Drawing utilities */
/*********************/

void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t* fbp,
		    struct fb_var_screeninfo* vinfo, 
		    struct fb_fix_screeninfo* finfo) {
  long loc = (x+vinfo->xoffset) * (vinfo->bits_per_pixel/8) + (y+vinfo->yoffset) * finfo->line_length; 
  *((uint32_t*)(fbp + loc)) = pixelColor(0x00, 0xFF, 0x00, vinfo);
}

// Convert rbg value to pixel value
uint32_t pixelColor(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo* vinfo) {
  return (r<<vinfo->red.offset) | (g<<vinfo->green.offset) | (b<<vinfo->blue.offset);
}

/*****************/
/* Miscellaneous */
/*****************/

void delay(int number_of_seconds) 
{ 
  int milli_seconds = 1000 * number_of_seconds; 
  
  // Storing start time 
  clock_t start_time = clock(); 
  	
  // Looping till required time is acheived 
  while (clock() < start_time + milli_seconds) {} 
}
