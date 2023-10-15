#include <game.h>

#define SIDE 32
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
      // if ((x & 1) ^ (y & 1)) {
        draw_tile(x * SIDE, y * SIDE, SIDE, SIDE, 0x666666); // white
      // }
    }
  }
}

// my work

void draw_ball(int x, int y, int size)
{
  draw_tile(x - size / 2, y - size / 2, size, size, 0x000000);
}

void update_state(int *dir, int *pos, int speed)
{
  if (*dir == 48)
  {
    pos[0] -= speed;

    if (pos[0] < 0)
    {
      pos[0] = speed / 2;
    }
  }

  else if (*dir == 49)
  {
    pos[1] += speed;
    if (pos[1] > h)
    {
      pos[1] = h - speed / 2;
    }
  }

  else if (*dir == 50)
  {
    pos[1] -= speed;
    if (pos[1] < 0)
    {
      pos[1] = speed / 2;
    }
  }

  else if (*dir == 51)
  {
    pos[0] += speed;
    if (pos[0] > w)
    {
      pos[0] = w - speed / 2;
    }
  }
}