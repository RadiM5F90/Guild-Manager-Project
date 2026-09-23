#include "list_queue.h"
#include <stdio.h>
#include <stdlib.h>


typedef struct _node {
    void* value;
    struct _node* next;
}node;

struct _queue {
    node* head;
    node* tail;
    int size;
    int capacity;
};

queue create_queue(int _capacity) {
    if (_capacity <= 0) return NULL;

    queue q = malloc(sizeof(struct _queue));
    if (q == NULL) return NULL;

    q->head = q->tail = NULL;
    q->size = 0;
    q->capacity = _capacity;

    return q;
}

void destroy_queue(queue* _queue) {
    if (_queue == NULL || *_queue == NULL) return;

    node* current = (*_queue)->head;
    while(current != NULL) {
        node* next = current->next;
        free(current);
        current = next;
    }
    free(*_queue);
    *_queue = NULL;
}

bool queue_is_empty(queue _queue) {
    return (_queue == NULL || _queue->size == 0);
}

bool queue_is_full(queue _queue) {
    if (_queue == NULL) return false;
    return _queue->size == _queue->capacity;
}

int enqueue(queue _queue, void* _value) {
    if (_queue == NULL) return QUEUE_ERROR_NULL;
    if (queue_is_full(_queue)) return QUEUE_ERROR_FULL;

    node* new_node = malloc(sizeof(node));
    if (new_node == NULL) return QUEUE_ERROR_ALLOC;

    new_node->value = _value;
    new_node->next = NULL;

    if (_queue->tail == NULL) _queue->head = new_node;
    else _queue->tail->next = new_node;

    _queue->tail = new_node;
    _queue->size++;

    return QUEUE_SUCCESS;
}

int dequeue(queue _queue, void** _value_out) {
    if (_queue == NULL) return QUEUE_ERROR_NULL;
    if (queue_is_empty(_queue)) return QUEUE_ERROR_EMPTY;

    node* to_delete = _queue->head;
    *_value_out = to_delete->value;
    _queue->head = to_delete->next;

    free(to_delete);
    _queue->size--;

    if (_queue->head == NULL) _queue->tail = NULL;

    return QUEUE_SUCCESS;
}

int queue_peek(queue _queue, void** _value_out) {
    if (_queue == NULL) return QUEUE_ERROR_NULL;
    if (queue_is_empty(_queue)) return QUEUE_ERROR_EMPTY;

    *_value_out = _queue->head->value;

    return QUEUE_SUCCESS;
}

int queue_size(queue _queue) {
    if (_queue == NULL) return QUEUE_ERROR_NULL;

    return _queue->size;
}
