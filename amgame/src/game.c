#include <game.h>

typedef struct{
    int x;
    int y;
} Square; //定义小方块对象
// Operating system is a C program!
int main(const char *args) {
    ioe_init();
    puts("mainargs = \"");
    puts(args);  // make run mainargs=xxx
    puts("\"\n");

    Square square=(Square){.x=10,.y=10};
    printf("squre.x:%d\n", square.x);
    puts("Press any key to see its key code...\n");
    while (1) {
        splash();
        print_key();
    }
    return 0;
}
