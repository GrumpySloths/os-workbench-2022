#ifndef DEBUG_H__
#define DEBUG_H__

#ifdef TEST  
#define malloc(s) mymalloc(s)
#define free(pt) myfree(pt)
#endif

#endif