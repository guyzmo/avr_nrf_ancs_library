#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <Arduino.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

extern "C" {

/** @brief structure for a node in the linked list used for the fifo */
typedef struct linked_list_node_t {
    struct linked_list_node_t* next;
    uint8_t* data;
    size_t size;
} linked_list_node_t;

/** @brief structure for the fifo queue
 * @details first is the entry point of the structure, 
 * _last is used to expand the fifo 
 */
typedef struct linked_list_t {
    linked_list_node_t* first;
    linked_list_node_t* _last;
} linked_list_t;

linked_list_node_t* fifo_create_node() {
    linked_list_node_t* node = (linked_list_node_t*)malloc(sizeof(linked_list_node_t));
    node->next = NULL;
    node->data = NULL;
    node->size = 0;
    return node;
}

/** @brief create a new fifo structure
 * @return an empty fifo structure
 */
linked_list_t* fifo_create() {
    linked_list_t* ll = (linked_list_t*)malloc(sizeof(linked_list_t));
    ll->first = NULL;
    ll->_last = NULL;
    return ll;
}

/** @brief push a byte array into the fifo
 * @param ll the linked list structure used as fifo
 * @param data bytearray to push
 */
void fifo_push(linked_list_t* ll, uint8_t* data, size_t len) {
    // linked list is empty, populate first node
    if (ll->first == NULL) {
        ll->first = fifo_create_node();
        ll->first->data = (uint8_t*)malloc(len);
        memcpy(ll->first->data, data, len);
        ll->first->size = len;
        ll->first->next = NULL;
        ll->_last = ll->first;
    // otherwise append a new node
    } else {
        ll->_last->next = fifo_create_node();
        ll->_last = ll->_last->next;
        ll->_last->data = (uint8_t*)malloc(len);
        memcpy(ll->_last->data, data, len);
        ll->_last->size = len;
        ll->_last->next = NULL;
    }
}

/** @brief pops out a byte array from the fifo
 * @details removes the bottom of the queue and returns it
 * @param ll the linked list structure used as fifo
 * @return bytearray poped out
 */
size_t fifo_pop(linked_list_t* ll, uint8_t** data) {
    if (ll->first == NULL) {
        (*data) = NULL;
        return 0;
    }
    *data = ll->first->data;
    size_t len = ll->first->size;
    linked_list_node_t* tmp = ll->first;
    ll->first = ll->first->next;
    free(tmp);
    return len;
}

}

#endif
