
#include <stdbool.h>

// -------GENERIC LINKED LIST-------

#define LINKED_LIST_SUCCESS 0;
#define LINKED_LIST_ERROR_NULL -1;
#define LINKED_LIST_ERROR_ALLOC -2;
#define LINKED_LIST_ERROR_EMPTY -3;
#define LINKED_LIST_ERROR_INDEX -4;
#define LINKED_LIST_ERROR_NOT_FOUND -5;

typedef struct _list* list;
typedef struct _list_iterator* iterator;

list list_create();

void list_destroy(list* _list);

int list_insert_front(list _list, void* _value);

int list_insert_back(list _list, void* _value);

int list_insert_at(list _list, int _index, void* _value);

int list_get_front(list _list, void** _value_out);

int list_get_back(list _list, void** _value_out);

int list_get_at(list _list, int _index, void** _value_out);

int list_find(list _list, void* _value, int* _index_out);

int list_remove_at(list _list, int _index);

int list_remove_value(list _list, void* _value);

int list_size(list _list);

int list_is_empty(list _list);



iterator list_iterator_create(list _list);

void iterator_destroy(iterator* _it);

bool iterator_has_next(iterator _it);

void* iterator_next(iterator _it);
