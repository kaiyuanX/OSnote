/* 简单的 'h' 'j' 'k' 'l' 控制小方块移动绘制 */

#include <game.h>

#define SIZEOFBALL 15
#define SPEEDOFBALL SIZEOFBALL
#define FPS 10

int ball[2] = {7, 7};
int action = 49;
static int next_frame = 0;

static inline int uptime_ms()
{
  return io_read(AM_TIMER_UPTIME).us / 1000;
}

void wait_for_frame()
{
  int cur = uptime_ms();
  while (cur < next_frame)
  {
    cur = uptime_ms();
  }
  next_frame = cur;
  next_frame += 1000 / FPS;
}

int main(const char *args)
{
  ioe_init();
  puts("mainargs = \"");
  puts(args); // make run mainargs=xxx
  puts("\"\n");

  splash();

  draw_ball(ball[0], ball[1], SIZEOFBALL);

  while (1)
  {
    wait_for_frame();
    update_input(&action, NULL);
    update_state(&action, ball, SPEEDOFBALL);
    printf("x = %d, y = %d \n", ball[0], ball[1]);
    // splash(); // 这个语句感觉有问题，降低整个机器的性能严重
    draw_ball(ball[0], ball[1], SIZEOFBALL);
  }
}

// // Operating system is a C program!
// int main(const char *args) {
//   ioe_init();

//   puts("mainargs = \"");
//   puts(args); // make run mainargs=xxx
//   puts("\"\n");

//   splash();

//   puts("Press any key to see its key code...\n");
//   while (1) {
//     print_key();
//   }
//   return 0;
// }
