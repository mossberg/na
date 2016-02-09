#pragma once

#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#include "hexdump.h"

#define PAGE_SIZE 4096
#define NA_START() ((void *)(na_free & ~(PAGE_SIZE - 1)))
#define NEXT_CHUNK(chunk) ((struct na_chunk_hdr *) (((char *)chunk) + sizeof(struct na_chunk_hdr) + chunk->size))

#define pp(ptr, msg) printf("##msg %p\n", ptr)

void *na_start;
/* void *na_free; */

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
    /* na_free = na_start; */
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
        curr = (uint8_t *)curr + sizeof(*curr) + curr->size;
    }
        printf("curr %p\n", curr);
    if (curr->is_last) {
        if (curr->size < len) {
            fprintf(stderr, "*** nooballoc2: no big enough heap chunks ***\n");
            exit(1);
        }

        struct na_chunk_hdr *next = ((uint8_t *) curr) + sizeof(*curr) + len;
        /* struct na_chunk_hdr *next = NEXT_CHUNK(curr); */
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


    /* void *ret = na_free; */
    /* na_free += len; */
    return (uint8_t *)curr + sizeof(*curr);
}

static void dump_hdr(struct na_chunk_hdr *hdr)
{
    printf("> %p\n", hdr);
    printf("\tsize: %lu\n", hdr->size);
    printf("\tallocated: %lu\n", hdr->allocated);
    printf("\tis_last: %lu\n", hdr->is_last);
}

void na_dump(void)
{
    struct na_chunk_hdr *curr = na_start;
    while (curr->is_last == false) {
        dump_hdr(curr);
        hexdump((uint8_t *)curr + sizeof(*curr), curr->size);
        curr = (uint8_t *)curr + sizeof(*curr) + curr->size;
    }
    dump_hdr(curr);
}

int na_close(void)
{
    /* munmap(NA_START(), (size_t )PAGE_SIZE); */
    return munmap(na_start, (size_t) 4096);
}
