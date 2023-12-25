#include <game.h>


// Operating system is a C program!
int main(const char *args) {
    ioe_init();
    puts("mainargs = \"");
    puts(args);  // make run mainargs=xxx
    puts("\"\n");

    AM_TIMER_RTC_T rtc = io_read(AM_TIMER_RTC);
    printf("%d:%d:%d\n", rtc.year, rtc.month, rtc.day);
    uint64_t t0;
    Square square = (Square){.x = 10, .y = 10,.vx=1,.vy=1};
    printf("squre.x:%d\n", square.x);
    puts("Press any key to see its key code...\n");
    flush();
    while (1) {
        t0 = uptime();
        read_key(&square);
        // if (read_key(&square))
        //     flush();
        screen_update(&square);
        while((uptime()-t0)<1000/FPS)
            ;
        printf("render time:%d\n", uptime() - t0);
        flush();
        // print_key();
    }
    return 0;
}
