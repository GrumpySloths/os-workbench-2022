#include <game.h>

#define KEYNAME(key) \
  [AM_KEY_##key] = #key,
static const char *key_names[] = {
  AM_KEYS(KEYNAME)
};

void print_key() {
  AM_INPUT_KEYBRD_T event = { .keycode = AM_KEY_NONE };
  ioe_read(AM_INPUT_KEYBRD, &event);
  if(event.keycode==AM_KEY_ESCAPE)
      halt(1);
  if (event.keycode != AM_KEY_NONE && event.keydown) {
      puts("Key pressed: ");
      puts(key_names[event.keycode]);
      puts("\n");
  }
}

void read_key(Square*square){
    AM_INPUT_KEYBRD_T event = io_read(AM_INPUT_KEYBRD);
    if(event.keycode==AM_KEY_ESCAPE)
      halt(1);
    if (event.keycode != AM_KEY_NONE && event.keydown) {
      switch(event.keycode){
        case AM_KEY_W:
            square->y--;
            break;
        case AM_KEY_S:
            square->y++;
            break;
        case AM_KEY_A:
            square->x--;
            break;
        case AM_KEY_D:
            square->x++;
            break;
        default:
            printf("key invalid,please input w/a/s/d\n");
        }
    }

}