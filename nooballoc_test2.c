#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "nooballoc2.h"
#include "hexdump.h"

int test1(void)
{
    int ret;
    if ((ret = na_init())) {
        perror(NULL);
        return ret;
    }

    char *a = na_alloc(8);
    memset(a, 'A', 8);

    char *b = na_alloc(8);
    memset(b, 'B', 8);

    char *c = na_alloc(16);
    memset(c, 'C', 16);

    na_dump();
    hexdump(na_start, 0x50);

    na_free(a);

    na_dump();
    hexdump(na_start, 0x50);

    char *d = na_alloc(5);
    memset(d, 'D', 5);


    na_dump();
    hexdump(na_start, 0x50);


    na_close();

    return 0;

}

int test2(void)
{
    int ret;
    if ((ret = na_init())) {
        perror(NULL);
        return ret;
    }

    char *a = na_alloc(8);
    memset(a, 'A', 8);

    char *b = na_alloc(8);
    memset(b, 'B', 8);

    char *c = na_alloc(16);
    memset(c, 'C', 16);

    na_dump();
    hexdump(na_start, 0x50);

    na_free(b);

    na_dump();
    hexdump(na_start, 0x50);

    char *d = na_alloc(9);
    memset(d, 'D', 9);

    char *e = na_alloc(7);
    memset(e, 'E', 7);


    na_dump();
    hexdump(na_start, 0x50);

    na_free(a);
    na_free(b);
    na_free(c);
    na_free(d);
    na_dump();
    hexdump(na_start, 0x50);

    char *z = na_alloc(32);
    memset(z, 'Z', 32);

    na_free(z);
    na_dump();
    hexdump(na_start, 0x100);


    na_close();
    return 0;
}


int coalesce(void)
{
    int ret;
    if ((ret = na_init())) {
        perror(NULL);
        return ret;
    }

    char *a = na_alloc(8);
    memset(a, 'A', 8);

    char *b = na_alloc(8);
    memset(b, 'B', 8);

    char *c = na_alloc(16);
    memset(c, 'C', 16);

    na_dump();
    hexdump(na_start, 0x50);

    na_free(b);

    na_dump();
    hexdump(na_start, 0x50);

    na_free(a);

    na_dump();
    hexdump(na_start, 0x50);


    na_close();
    return 0;
}


int coalesce2(void)
{
    int ret;
    if ((ret = na_init())) {
        perror(NULL);
        return ret;
    }

    char *a = na_alloc(8);
    memset(a, 'A', 8);

    char *b = na_alloc(8);
    memset(b, 'B', 8);

    char *c = na_alloc(16);
    memset(c, 'C', 16);

    na_dump();
    hexdump(na_start, 0x50);

    na_free(b);

    na_dump();
    hexdump(na_start, 0x50);

    char *d = na_alloc(9);
    memset(d, 'D', 9);

    char *e = na_alloc(7);
    memset(e, 'E', 7);


    na_dump();
    hexdump(na_start, 0x50);

    na_free(d);
    na_free(c);
    na_free(e);
    /* na_free(a); */
    na_dump();
    hexdump(na_start, 0x50);

    char *z = na_alloc(32);
    memset(z, 'Z', 32);

    na_dump();
    hexdump(na_start, 0x100);

    na_free(z);
    na_free(a);
    na_dump();
    hexdump(na_start, 0x100);

    na_close();
    return 0;
}

int _realloc(void)
{
    int ret;
    if ((ret = na_init())) {
        perror(NULL);
        return ret;
    }

    char *a = na_alloc(10);
    memset(a, 'A', 10);

    char *b = na_alloc(8);
    memset(b, 'B', 8);

    char *c = na_alloc(16);
    memset(c, 'C', 16);

    na_dump();
    hexdump(na_start, 0x50);

    na_realloc(a, 9);

    na_dump();
    hexdump(na_start, 0x100);

    na_close();
    return 0;
}

int _realloc2(void)
{
    int ret;
    if ((ret = na_init())) {
        perror(NULL);
        return ret;
    }

    char *a = na_alloc(10);
    memset(a, 'A', 10);

    char *b = na_alloc(8);
    memset(b, 'B', 8);

    char *c = na_alloc(16);
    memset(c, 'C', 16);

    na_dump();
    hexdump(na_start, 0x50);

    na_realloc(a, 11);

    na_dump();
    hexdump(na_start, 0x100);

    na_close();
    return 0;
}

// realloc a chunk when the next chunk is empty and the size of both chunks
// is big enough to fit request should coalesce those. currently we would
// detect that the current chunk is not big enough, then linear search to find
// a fresh fitting chunk
int _realloc3(void)
{
    int ret;
    if ((ret = na_init())) {
        perror(NULL);
        return ret;
    }

    char *a = na_alloc(10);
    memset(a, 'A', 10);

    char *b = na_alloc(8);
    memset(b, 'B', 8);

    char *c = na_alloc(16);
    memset(c, 'C', 16);

    char *d = na_alloc(16);
    memset(d, 'D', 16);

    na_free(c);

    na_dump();
    hexdump(na_start, 0x50);

    na_realloc(b, 20);

    na_dump();
    hexdump(na_start, 0x100);

    na_close();
    return 0;
}

int main(int argc, char **argv)
{

    /* test1(); */
    /* test2(); */
    /* coalesce2(); */
    _realloc3();
    return 0;
}
