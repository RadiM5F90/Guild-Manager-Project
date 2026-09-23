#include "hashmap.h"

#include "hashmap.h"
#include <string.h>
#include<stdlib.h>

typedef struct _entity {
    char* key;
    void* value;
}* entity;

struct _hashmap {
    list* buckets;
    int capacity;
};

// Hash function helpers
int hash_len(const char* _key, int _capacity);
int hash_sum(const char* _key, int _capacity);
int hash(const char* _key, int _capacity);


hashmap hashmap_create(int _capacity) {
    if (_capacity <= 0) return NULL;

    hashmap map = malloc(sizeof(struct _hashmap));
    if (map == NULL) return NULL;

    map->capacity = _capacity;
    map->buckets = malloc(sizeof(list) * _capacity);
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }

    for (int i = 0; i < _capacity; i++) map->buckets[i] = list_create();
    return map;
}

void hashmap_destroy(hashmap* _map) {
    if (_map == NULL || *_map == NULL) return;

    for (int i = 0; i < (*_map)->capacity; i++) {
        list _list = (*_map)->buckets[i];
        iterator _it = list_iterator_create(_list);

        while (iterator_has_next(_it)) {
            entity e = (entity)iterator_next(_it);
            free(e->key);
            free(e);
        }
        iterator_destroy(&_it);
        list_destroy(&_list);
    }
    free((*_map)->buckets);
    free(*_map);
    *_map = NULL;
}

int hashmap_set(hashmap _map, char* _key, void* _value) {
    if (_map == NULL) return HASHMAP_ERROR_NULL;
    if (_key == NULL) return HASHMAP_ERROR_KEY;

    int index = hash(_key, _map->capacity);
    list _list = _map->buckets[index];

    iterator _it = list_iterator_create(_list);
    bool found = false;
    entity e = NULL;

    while (iterator_has_next(_it) && !found) {
        e = (entity)iterator_next(_it);
        if (strcmp(e->key, _key) == 0) found = true;
    }
    iterator_destroy(&_it);
    if (found) {
        e->value = _value;
        return HASHMAP_SUCCESS;
    }

    entity new = malloc(sizeof(struct _entity));
    if (new == NULL) return HASHMAP_ERROR_MEMORY;

    new->key = strdup(_key);
    if (new->key == NULL) {
        free(new);
        return HASHMAP_ERROR_MEMORY;
    }
    new->value = _value;
    list_insert_back(_list, new);
    return HASHMAP_SUCCESS;
}

int hashmap_get(hashmap _map, char* _key, void** _value_out) {
    if (_map == NULL || _value_out == NULL) return HASHMAP_ERROR_NULL;
    if (_key == NULL) return HASHMAP_ERROR_KEY;

    int index = hash(_key, _map->capacity);
    list _list = _map->buckets[index];

    iterator _it = list_iterator_create(_list);
    bool found = false;
    entity e = NULL;

    while (iterator_has_next(_it) && !found) {
        e = (entity)iterator_next(_it);
        if (strcmp(e->key, _key) == 0) found = true;
    }
    iterator_destroy(&_it);
    if (found) {
        *_value_out = e->value;
        return HASHMAP_SUCCESS;
    }
    return HASHMAP_ERROR_NOT_FOUND;
}

bool hashmap_has_key(hashmap _map, char* _key) {
    if (_map == NULL || _key == NULL) return false;

    int index = hash(_key, _map->capacity);
    list _list = _map->buckets[index];

    iterator _it = list_iterator_create(_list);
    bool found = false;
    entity e = NULL;

    while (iterator_has_next(_it) && !found) {
        e = (entity)iterator_next(_it);
        if (strcmp(e->key, _key) == 0) found = true;
    }
    iterator_destroy(&_it);
    return found;
}

list hashmap_get_keys(hashmap _map) {
    if (_map == NULL) return NULL;

    list keys = list_create();
    for (int i = 0; i < _map->capacity; i++) {
        list _list = _map->buckets[i];
        iterator _it = list_iterator_create(_list);

        while (iterator_has_next(_it)) {
            entity e = (entity)iterator_next(_it);
            list_insert_back(keys, strdup(e->key));
        }
        iterator_destroy(&_it);
    }
    return keys;
}

int hashmap_size(hashmap _map) {
    if (_map == NULL) return HASHMAP_ERROR_NULL;

    int count = 0;
    for (int i = 0; i < _map->capacity; i++) count += list_size(_map->buckets[i]);
    return count;
}

int hash_len(const char* _key, int _capacity) {
    return strlen(_key) % _capacity;
}

int hash_sum(const char* _key, int _capacity) {
    int result = 0;
    while (*_key != '\0') {
        result = (result + *_key) % _capacity;
        _key++;
    }
    return result;
}

int hash(const char* _key, int _capacity) {
    return hash_sum(_key, _capacity);
}
