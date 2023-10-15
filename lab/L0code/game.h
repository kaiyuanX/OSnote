#include <am.h>
#include <amdev.h>
#include <klib.h>
#include <klib-macros.h>

void splash();
void print_key();
static inline void puts(const char *s) {
  for (; *s; s++) putch(*s);
}

//
void draw_ball(int x, int y, int size);

void update_input(int *dir, int *e);
void update_state(int *dir, int *pos, int speed);