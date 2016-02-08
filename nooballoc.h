#pragma once

#include <sys/mman.h>
#include <stdint.h>

#define PAGE_SIZE 4096U
#define NA_START() ((void *)(na_free & ~(PAGE_SIZE - 1)))

void *na_start;
void *na_free;

int na_init(void)
{
    na_start = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    if (na_start == MAP_FAILED)
        return -1;
    na_free = na_start;
    return 0;
}

void *na_alloc(size_t len)
{
    void *ret = na_free;
    na_free += len;
    return ret;
}

int na_close(void)
{
    /* munmap(NA_START(), (size_t )PAGE_SIZE); */
    return munmap(na_start, (size_t) 4096);
}
