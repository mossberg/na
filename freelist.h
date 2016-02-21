#pragma once

#include <sys/mman.h>
#include <stdio.h>

#include "nooballoc2.h"
#define NDEBUG 0
#include "debug.h"

#define PAGE_SIZE 4096


typedef int flnode_data_t;

void *fl_start;
struct flnode *fl_head;
struct flnode *fl_tail;

size_t fl_size;

struct flnode {
    /* struct na_chunk_hdr *chk; */ 
    flnode_data_t chk;
    struct flnode *next;
};

static struct flnode *flnode_alloc(void);

int fl_init(void)
{
    fl_start = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    if (fl_start == MAP_FAILED)
        return -1;
    fl_head = fl_start;
    fl_tail = fl_start;
    fl_size = PAGE_SIZE / sizeof(struct flnode);

    debug("fl_start %p", fl_start);
    debug("fl_head %p", fl_head);
    debug("fl_size %lu", fl_size);

    return 0;
}

int fl_close(void)
{
    fl_start = NULL;
    fl_head = NULL;
    return munmap(fl_start, PAGE_SIZE);
}

int fl_append(flnode_data_t data)
{
    struct flnode *new = flnode_alloc();
    if (!new) {
        return -1;
    }
    debug("yo %p", new);
    new->chk = data;
    fl_tail->next = new;
    fl_tail = new;
    return 0;
}

static struct flnode *flnode_alloc(void)
{
    struct flnode *curr = fl_start;
    debug("%d", curr->chk);
    while (curr < (struct flnode *)((uintptr_t)fl_start + PAGE_SIZE) &&
            curr->chk) {
        curr++;
    }
    debug("curr %p", curr);
    debug("max %p", (struct flnode *)((uintptr_t) fl_start + PAGE_SIZE));
    if (curr  >= (struct flnode *)((uintptr_t) fl_start + PAGE_SIZE)) {
        return NULL;
    }
    return curr;
}

static void dump_fl(struct flnode *node)
{
    printf("> %p\n", node);
    printf("\tchk: %d\n", node->chk);
    printf("\tnext: %p\n", node->next);
}

void fl_dump(void)
{
    struct flnode *curr = fl_head;
    while (curr->next) {
        dump_fl(curr);
        curr = curr->next;
    }
    dump_fl(curr);
}
