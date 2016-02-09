#pragma once

#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#include "hexdump.h"

#define PAGE_SIZE 4096
#define CHUNK_DATA(chunk) ((void *) (((uint8_t *)chunk) + sizeof(*chunk)))
#define CHUNK_HDR(data) ((void *) (uint8_t *)data - sizeof(struct na_chunk_hdr))
#define NEXT_CHUNK(chunk) ((void *)((uint8_t *)chunk + sizeof(*chunk) + chunk->size))


void *na_start;

struct na_chunk_hdr {
    size_t size;  
    bool allocated;
    bool is_last;
} __attribute__((packed));

static void na_panic(const char *msg)
{
    fprintf(stderr, "*** nooballoc2: %s ***\n", msg);
    exit(1);
}

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
    while ((curr->allocated || curr->size < len) && !curr->is_last) {
        curr = NEXT_CHUNK(curr);
    }
    if (curr->is_last) {
        if (curr->size < len) {
            na_panic("no big enough heap chunks");
        }

        struct na_chunk_hdr *next = CHUNK_DATA(curr) + len;
        /* printf("curr %p\n", curr); */
        /* printf("na start %p\n", na_start); */
        /* printf("next %p\n", next); */


        next->size = curr->size - len - sizeof(*next);
        next->allocated = false;
        next->is_last = true;

        curr->is_last = false;
        curr->size = len;

    }

    curr->allocated = true;

    return CHUNK_DATA(curr);
}

void na_free(void *p)
{
    struct na_chunk_hdr *hdr = CHUNK_HDR(p);
    if (!hdr->allocated)
        na_panic("trying to free unallocated chunk!");
    hdr->allocated = false;
    struct na_chunk_hdr *next = NEXT_CHUNK(hdr);
    if (!next->allocated) {
        hdr->size += sizeof(*next) + next->size;
        if (next->is_last) {
            hdr->is_last = true;
        }
    }
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
    size_t heap_size = 0;
    struct na_chunk_hdr *curr = na_start;
    while (curr->is_last == false) {
        dump_hdr(curr);
        if (curr->allocated) {
            hexdump((uint8_t *)curr + sizeof(*curr), curr->size);
        }
        heap_size += curr->size + sizeof(*curr);
        curr = NEXT_CHUNK(curr);
    }
    dump_hdr(curr);
    heap_size += curr->size + sizeof(*curr);
    assert(heap_size == PAGE_SIZE && "all heap sizes don't add up!");
}

int na_close(void)
{
    return munmap(na_start, (size_t) 4096);
}
