#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

// Convert rbg value to pixel value
uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo) {
	return (r<<vinfo->red.offset) | (g<<vinfo->green.offset) | (b<<vinfo->blue.offset);
}

void delay(int number_of_seconds) 
{ 
    // Converting time into milli_seconds 
    int milli_seconds = 1000 * number_of_seconds; 
  
    // Stroing start time 
    clock_t start_time = clock(); 
  	
    printf ("start_time: %d\n", start_time);
    printf ("start_time + milli_seconds: %d\n", start_time + milli_seconds);

    // looping till required time is not acheived 
    while (clock() < start_time + milli_seconds) 
        printf (""); 
} 

int main() {

	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;

	int fb_fd = open("/dev/fb0", O_RDWR);
	printf ("fb_fd: %d\n", fb_fd);
	printf ("errno: %d\n", errno);

	// Get variable screen info
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

	vinfo.grayscale = 0;
	vinfo.bits_per_pixel = 32;

	ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
	ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

	long screensize = finfo.smem_len;

	printf ("screensize: %ld\n", screensize);

	// Map the buffer to memory, returns ptr to memory
	uint8_t* fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);

	printf ("errno: %d\n", errno);


	// Coords of the pixel we want to set
	int x, y;
	for (x = 0; x < vinfo.xres; x++ ) {
		for (y = 0; y < vinfo.yres; y++) {
			long location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
			printf ("location: %ld\n", location);
			*((uint32_t*)(fbp + location)) = pixel_color(0xFF, 0x00, 0xFF, &vinfo);
		}
	}
	
	delay(99999999);

	return 0;
}