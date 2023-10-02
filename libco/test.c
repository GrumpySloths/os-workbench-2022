#include <stdio.h>

#define LOCAL_MACHINE

#ifdef LOCAL_MACHINE
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

int main(void)
{
    debug("hello world\n");

    return 0;
}