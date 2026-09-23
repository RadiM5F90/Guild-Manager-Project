
#include <stdbool.h>

#define STACK_SUCCESS 0
#define STACK_ERROR_NULL -1
#define STACK_ERROR_ALLOC -2
#define STACK_ERROR_EMPTY -3
#define STACK_ERROR_FULL -4

// ----- GENERIC STACK -----

typedef struct _stack* stack;

stack create_stack(int _capacity);

void destroy_stack(stack* _stack);

int stack_push(stack _stack, void* _value);

int stack_pop(stack _stack, void** _value_out);

int stack_top(stack _stack, void** _value_out);

bool stack_is_empty(stack _stack);

bool stack_is_full(stack _stack);

int stack_size(stack _stack);
