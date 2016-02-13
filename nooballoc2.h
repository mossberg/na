#pragma once

#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "hexdump.h"

#define NDEBUG 1
#include "debug.h"


#define PAGE_SIZE 4096
#define CHUNK_DATA(chunk) ((void *) (((uint8_t *)chunk) + sizeof(*chunk)))
#define CHUNK_HDR(data) ((struct na_chunk_hdr *) ((uint8_t *)data - sizeof(struct na_chunk_hdr)))
#define NEXT_CHUNK_HDR(chunk) ((struct na_chunk_hdr *)((uint8_t *)chunk + sizeof(*chunk) + chunk->size))
/* don't call this on the first chunk (chunk->prev_size == -1) */
#define PREV_CHUNK_HDR(chunk) ((struct na_chunk_hdr *)((uint8_t *)chunk - sizeof(*chunk) - chunk->prev_size))

void na_dump(void);
void na_free(void *p);

void *na_start;

struct na_chunk_hdr {
    ssize_t prev_size;
    size_t size;  
    bool allocated;
    bool is_last;
} __attribute__((packed));

static void dump_hdr(struct na_chunk_hdr *hdr);

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
    first->prev_size = -1;
    first->size = PAGE_SIZE - sizeof(*first);
    first->allocated = false;
    first->is_last = true;
    return 0;
}


static struct na_chunk_hdr *get_chunk_of_len(size_t len)
{
    struct na_chunk_hdr *curr = na_start;
    while ((curr->allocated || curr->size < len) && !curr->is_last) {
        curr = NEXT_CHUNK_HDR(curr);
    }
    if (curr->is_last) {
        if (curr->size < len) {
            na_panic("no big enough heap chunks");
        }

        struct na_chunk_hdr *next = (struct na_chunk_hdr *) ((uintptr_t) \
                CHUNK_DATA(curr) + len);

        debug("curr %p", curr);
        debug("na start %p", na_start);
        debug("next %p", next);

        next->prev_size = len;
        next->size = curr->size - len - sizeof(*next);
        next->allocated = false;
        next->is_last = true;

        curr->is_last = false;
        curr->size = len;

    }

    curr->allocated = true;

    return curr;

}

void *na_alloc(size_t len)
{
    return CHUNK_DATA(get_chunk_of_len(len));
}

// assumes there is a next chunk
static void forward_coalesce(struct na_chunk_hdr *hdr)
{
    struct na_chunk_hdr *next = NEXT_CHUNK_HDR(hdr);
    if (next->allocated)
        return;
    size_t diff = sizeof(*next) + next->size;
    hdr->size += diff;
    if (next->is_last) {
        hdr->is_last = true;
    } else {
        /* if we didn't just coalesce the last chunk, we need to update
         * the new next's prev_size */
        next = NEXT_CHUNK_HDR(hdr);
        next->prev_size += diff;
    }
}

static void backward_coalesce(struct na_chunk_hdr *hdr)
{
    if (hdr->prev_size == -1)
        return;
    struct na_chunk_hdr *prev = PREV_CHUNK_HDR(hdr);
    if (prev->allocated)
        return;
    prev->size += sizeof(*hdr) + hdr->size;
    /* we always keep the next chunk's prev_size updated */
    struct na_chunk_hdr *next = NEXT_CHUNK_HDR(hdr);
    next->prev_size = prev->size;
}

void *na_realloc(void *p, size_t len)
{
    struct na_chunk_hdr *curr = CHUNK_HDR(p);
    if (!curr->allocated)
        na_panic("trying to realloc unallocated chunk!");
    if (len <= curr->size) {
        return p;
    } else if (!NEXT_CHUNK_HDR(curr)->allocated) {
        struct na_chunk_hdr *next = NEXT_CHUNK_HDR(curr);
        if (len <= curr->size + sizeof(*next) + next->size) {
            if (next->is_last) {
                struct na_chunk_hdr *last = next;
                // todo these names are really bad, but the math should be
                // right
                struct na_chunk_hdr *new_last = (struct na_chunk_hdr *) ((uintptr_t) \
                        CHUNK_DATA(curr) + len);
                size_t wilderness_diff = sizeof(*new_last) + (len - (curr->size + sizeof(*last)));
                curr->size = len;

                /* we have to copy the last chunk hdr in case the requested len
                 * makes new_last overlap with last so we can safely refer
                 * to both */
                struct na_chunk_hdr tmp;
                memcpy(&tmp, last, sizeof(tmp));

                new_last->prev_size = curr->size;
                new_last->size = tmp.size - wilderness_diff;
                new_last->allocated = false;
                new_last->is_last = true;
            } else {
                forward_coalesce(curr);
            }
            return p;
        }
    }

    struct na_chunk_hdr *bigger = get_chunk_of_len(len);
    memcpy(CHUNK_DATA(bigger), CHUNK_DATA(curr), curr->size);
    curr->allocated = false;

    return CHUNK_DATA(bigger);
}

void na_free(void *p)
{
    if (!p)
        return;
    struct na_chunk_hdr *hdr = CHUNK_HDR(p);
    if (!hdr->allocated)
        na_panic("trying to free unallocated chunk!");
    hdr->allocated = false;

    forward_coalesce(hdr);
    backward_coalesce(hdr);
}


static void dump_hdr(struct na_chunk_hdr *hdr)
{
    printf("> %p\n", hdr);
    printf("\tprev_size: %zd (0x%zx)\n", hdr->prev_size, hdr->prev_size);
    printf("\tsize: %lu (0x%zx)\n", hdr->size, hdr->size);
    printf("\tallocated: %d\n", hdr->allocated);
    printf("\tis_last: %d\n", hdr->is_last);
}

void na_dump(void)
{
    size_t heap_size = 0;
    struct na_chunk_hdr *curr = na_start;
    puts(" >>> DUMP BEGIN <<< ");
    while (curr->is_last == false) {
        dump_hdr(curr);
        if (curr->allocated) {
            hexdump((uint8_t *)curr + sizeof(*curr), curr->size);
        }
        heap_size += curr->size + sizeof(*curr);
        curr = NEXT_CHUNK_HDR(curr);
    }
    dump_hdr(curr);
    heap_size += curr->size + sizeof(*curr);
    debug("heap size %lu\n", heap_size);
    assert(heap_size == PAGE_SIZE && "all heap sizes don't add up!");

    puts(" >>> DUMP END <<< ");
}

int na_close(void)
{
    return munmap(na_start, (size_t) 4096);
}
