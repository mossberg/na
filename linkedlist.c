#include <stdio.h>
#include "nooballoc2.h"

struct node {
    int data;
    struct node *next;
};

/*
 * returns head of list
 */
struct node *
ll_append(struct node *head, int data)
{
    if (!head) {
        struct node *n = na_alloc(sizeof(*n));
        n->data = data;
        n->next = NULL;
        return n;
    }

    struct node *curr = head;
    while (curr->next) {
        curr = curr->next;
    }
    struct node *new = na_alloc(sizeof(*new));
    new->data = data;
    new->next = NULL;
    curr->next = new;

    return head;
}

void
ll_dump_node(struct node *node)
{
    printf("> %p\n", node);
    printf("\t%d\n", node->data);
    printf("\t%p\n", node->next);
}

void
ll_dump(struct node *head)
{
    struct node *curr = head;
    while (curr) {
        ll_dump_node(curr);
        curr = curr->next;
    }
}

void
ll_destroy(struct node *head)
{
    struct node *curr = head;
    while (curr) {
        struct node *next = curr->next;
        na_free(curr);
        curr = next;
    }
}

void
ll_test(void)
{
    na_dump();
    struct node *head = ll_append(NULL, 1);
    na_dump();
    ll_append(head, 2);
    na_dump();
    ll_append(head, 3);
    na_dump();
    ll_dump(head);
    na_dump();
    ll_destroy(head);
    na_dump();
}


int main(int argc, char **argv)
{
    na_init();
    ll_test();
    na_close();
    return 0;
}
