#include <algo/dllist.h>

void dllist_insert(dllist_t* list, dllist_node_t* node, int pos) {
    if (!list || !node) return;
    node->list = list;
    node->prev = node->next = NULL;
    
    if (!list->head) {
        list->head = list->tail = node;
        return;
    }

    if (pos >= 0) {
        dllist_node_t* curr = list->head; // node currently at index pos
        for (int i = 0; i < pos && curr; i++) curr = curr->next;

        if (!curr) { // out of bounds -> insert after tail
            node->prev = list->tail;
            list->tail->next = node;
            list->tail = node;
        } else if (curr == list->head) { // insert before current head
            node->next = list->head;
            list->head->prev = node;
            list->head = node;
        } else { // insert before curr
            node->prev = curr->prev;
            node->next = curr;
            curr->prev->next = node;
            curr->prev = node;
        }
    } else {
        int steps = -pos - 1;
        dllist_node_t* curr = list->tail;
        for (int i = 0; i < steps && curr->prev; i++) curr = curr->prev;

        if (!steps) { // insert after tail
            node->prev = list->tail;
            list->tail->next = node;
            list->tail = node;
        } else { // insert before curr
            node->prev = curr->prev;
            node->next = curr;
            if (curr->prev) curr->prev->next = node;
            else list->head = node;
            curr->prev = node;
        }
    }
}

void dllist_remove(dllist_node_t* node) {
    if (!node || !node->list) return;

    dllist_t* list = node->list;
    if (list->head == node) { // list head
        if (list->head == list->tail) list->head = list->tail = NULL; // list's only node
        else {
            list->head = node->next;
            node->next->prev = NULL;
        }
    } else if (list->tail == node) { // list tail
        list->tail = node->prev;
        node->prev->next = NULL;
    } else {
        dllist_node_t* prev = node->prev;
        dllist_node_t* next = node->next;
        prev->next = next; next->prev = prev;
    }

    node->list = NULL;
    node->prev = node->next = NULL;
}
