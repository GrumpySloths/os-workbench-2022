#include <game.h>

#define SIDE 16
static int w, h;

static void init() {
  AM_GPU_CONFIG_T info = {0};
  ioe_read(AM_GPU_CONFIG, &info);
  w = info.width;
  h = info.height;
}

static void draw_tile(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: large stack-allocated memory
  AM_GPU_FBDRAW_T event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  ioe_write(AM_GPU_FBDRAW, &event);
}

void splash() {
  init();
  for (int x = 0; x * SIDE <= w; x ++) {
    for (int y = 0; y * SIDE <= h; y++) {
      if ((x & 1) ^ (y & 1)) {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0xffffff); // white
      }
    }
  }
}

void screen_update(Square* square) { 
  // init();
  if(square->x*SIDE>w||square->x<0)
      square->vx *= -1;
  if(square->y*SIDE>h||square->y<0)
      square->vy *= -1;
  draw_tile(square->x * SIDE, square->y * SIDE, SIDE, SIDE, 0xffffff);
}
//刷新整个屏幕
void flush() {
    // printf("flush is called\n");
    init();
    for (int x = 0; x * SIDE <= w; x ++) {
      for (int y = 0; y * SIDE <= h; y++) {

        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0x000000); // white
      
    }
  }
}
//get system time(ms)
uint64_t uptime() { return io_read(AM_TIMER_UPTIME).us/1000; }