#include <game.h>


// Operating system is a C program!
int main(const char *args) {
    ioe_init();
    puts("mainargs = \"");
    puts(args);  // make run mainargs=xxx
    puts("\"\n");

    AM_TIMER_RTC_T rtc = io_read(AM_TIMER_RTC);
    printf("%d:%d:%d\n", rtc.year, rtc.month, rtc.day);
    AM_TIMER_UPTIME_T t0 = io_read(AM_TIMER_UPTIME);
    Square square = (Square){.x = 10, .y = 10};
    printf("squre.x:%d\n", square.x);
    puts("Press any key to see its key code...\n");
    AM_TIMER_UPTIME_T t1 = io_read(AM_TIMER_UPTIME);
    printf("cur t0 time:%d t1 time:%d\n ", t0.us,t1.us);
    flush();
    // splash();
    while (1) {
        // splash();
        // flush();
        if(read_key(&square)) flush();
        screen_update(&square);
        // print_key();
    }
    return 0;
}
