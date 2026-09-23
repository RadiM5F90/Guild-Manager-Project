#include <stdio.h>
#include <stdlib.h>
#include "list_stack.h"


// ------- GENERIC LIST-STACK -------


typedef struct _node {
    void* value;
    struct _node *next;
}node;

struct _stack {
    node* top;
    int size;
    int capacity;
};

stack create_stack(int _capacity) {
    if (_capacity <= 0) return NULL;

    stack s = malloc(sizeof(struct _stack));
    if (s == NULL) return NULL;

    s->top = NULL;
    s->capacity = _capacity;
    s->size = 0;

    return s;
}

void destroy_stack(stack* _stack) {
    if (_stack == NULL || *_stack == NULL) return;

    node* current = (*_stack)->top;

    while (current != NULL) {
        node* next = current->next;
        free(current);
        current = next;
    }
    free(*_stack);
    *_stack = NULL;
}

bool stack_is_empty(stack _stack) {
    return (_stack == NULL || _stack->size == 0);
}

bool stack_is_full(stack _stack) {
    if (_stack == NULL) return false;
    return (_stack->size == _stack->capacity);
}

int stack_push(stack _stack, void* _value) {
    if (_stack == NULL) return STACK_ERROR_NULL;
    if (stack_is_full(_stack)) return STACK_ERROR_FULL;

    node* new_node = malloc(sizeof(node));
    if (new_node == NULL) return STACK_ERROR_ALLOC;

    new_node->value = _value;
    new_node->next = _stack->top;
    _stack->top = new_node;

    _stack->size++;
    return STACK_SUCCESS;
}

int stack_pop(stack _stack, void** _value_out) {
    if (_stack == NULL) return STACK_ERROR_NULL;
    if (stack_is_empty(_stack)) return STACK_ERROR_EMPTY;

    node* to_delete = _stack->top;
    *_value_out = to_delete->value;
    _stack->top = to_delete->next;
    free(to_delete);
    _stack->size--;

    return STACK_SUCCESS;

}

int stack_top(stack _stack, void** _value_out) {
    if (_stack == NULL) return STACK_ERROR_NULL;
    if (stack_is_empty(_stack)) return STACK_ERROR_EMPTY;

    *_value_out = _stack->top->value;

    return STACK_SUCCESS;
}

int stack_size(stack _stack) {
    if (_stack == NULL) return 0;
    return _stack->size;
}
