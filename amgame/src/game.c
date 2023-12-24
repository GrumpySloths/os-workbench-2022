#include <game.h>


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
        // splash();
        read_key(&square);
        screen_update(&square);
        flush();
        // print_key();
    }
    return 0;
}
