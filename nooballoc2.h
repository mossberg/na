#pragma once

#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#include "hexdump.h"

#define PAGE_SIZE 4096
#define CHUNK_DATA(chunk) ((void *) (((uint8_t *)chunk) + sizeof(*chunk)))
#define NEXT_CHUNK(chunk) ((struct na_chunk_hdr *)((uint8_t *)chunk + sizeof(*chunk) + chunk->size))


void *na_start;

struct na_chunk_hdr {
    size_t size;  
    bool allocated;
    bool is_last;
} __attribute__((packed));

int na_init(void)
{
    na_start = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    if (na_start == MAP_FAILED)
        return -1;
    struct na_chunk_hdr *first = na_start;
    first->size = PAGE_SIZE - sizeof(*first);
    first->allocated = false;
    first->is_last = true;
    return 0;
}

void *na_alloc(size_t len)
{
    struct na_chunk_hdr *curr = na_start;
    puts("yo");
    hexdump(na_start, 0x50);
    while ((curr->allocated || curr->size < len) && curr->is_last == false) {
        curr = NEXT_CHUNK(curr);
    }
        printf("curr %p\n", curr);
    if (curr->is_last) {
        if (curr->size < len) {
            fprintf(stderr, "*** nooballoc2: no big enough heap chunks ***\n");
            exit(1);
        }

        struct na_chunk_hdr *next = CHUNK_DATA(curr) + len;
        printf("curr %p\n", curr);
        printf("na start %p\n", na_start);
        printf("next %p\n", next);


        next->size = curr->size - len - sizeof(*next);
        next->allocated = false;
        next->is_last = true;

        curr->is_last = false;
        curr->size = len;

    }

    curr->allocated = true;

    return CHUNK_DATA(curr);
}

static void dump_hdr(struct na_chunk_hdr *hdr)
{
    printf("> %p\n", hdr);
    printf("\tsize: %lu\n", hdr->size);
    printf("\tallocated: %d\n", hdr->allocated);
    printf("\tis_last: %d\n", hdr->is_last);
}

void na_dump(void)
{
    struct na_chunk_hdr *curr = na_start;
    while (curr->is_last == false) {
        dump_hdr(curr);
        if (curr->allocated) {
            hexdump((uint8_t *)curr + sizeof(*curr), curr->size);
        }
        curr = NEXT_CHUNK(curr);
    }
    dump_hdr(curr);
}

int na_close(void)
{
    return munmap(na_start, (size_t) 4096);
}
