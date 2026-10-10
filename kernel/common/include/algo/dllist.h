#ifndef __ALGO_DLLIST_H
#define __ALGO_DLLIST_H

#include <stddef.h>
#include <stdint.h>

struct dllist;

/* doubly linked list */
typedef struct dllist_node {
    struct dllist* list; // parent list
    struct dllist_node* prev;
    struct dllist_node* next;
} dllist_node_t;

typedef struct dllist {
    dllist_node_t* head;
    dllist_node_t* tail;
} dllist_t;

void dllist_insert(dllist_t* list, dllist_node_t* node, int pos);
// non-negative pos = insert before node pos from start (e.g. pos = 0 => insert at beginning, pos = 1 => insert after 1st node, etc.)
// negative pos = insert after node (-pos) from end (e.g. pos = -1 => insert at end, pos = -2 => insert before last node)

void dllist_remove(dllist_node_t* node);

/* convenience functions */

static inline void dllist_insert_head(dllist_t* list, dllist_node_t* node) {
    dllist_insert(list, node, 0);
}

static inline void dllist_insert_tail(dllist_t* list, dllist_node_t* node) {
    dllist_insert(list, node, -1);
}

static inline dllist_node_t* dllist_traverse_next(const dllist_node_t* node) {
    return node->next;
}

static inline dllist_node_t* dllist_traverse_prev(const dllist_node_t* node) {
    return node->prev;
}

static inline dllist_node_t* dllist_head(const dllist_t* list) {
    return list->head;
}

static inline dllist_node_t* dllist_tail(const dllist_t* list) {
    return list->tail;
}


#endif