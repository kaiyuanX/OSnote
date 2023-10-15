#include <game.h>

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = {
  AM_KEYS(KEYNAME)
};

void print_key() {
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);
  if (event.keycode != AM_KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}

// my work

void update_input(int *dir, int *e)
{
  AM_INPUT_KEYBRD_T event = {.keycode = AM_KEY_NONE};
  ioe_read(AM_INPUT_KEYBRD, &event);

  if (event.keycode != AM_KEY_NONE && event.keydown)
  {
    puts(key_names[event.keycode]);
    switch (event.keycode)
    {
    case 48:
      *dir = 48;
      break;
    case 49:
      *dir = 49;
      break;
    case 50:
      *dir = 50;
      break;
    case 51:
      *dir = 51;
      break;
    default:
      // handle int *e
      break;
    }
  }
}