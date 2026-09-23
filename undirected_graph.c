#include "undirected_graph.h"
#include "list_stack.h"
#include "list_queue.h"


// TMATRIX -> triangular matrix
#define TMATRIX_SIZE(n) ((n) * ((n) + 1) / 2)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))


struct _indirect_graph {
    void** nodes;
    int* edges;
    int size;
    int max_size;
};

int tmatrix_index(int i, int j);


// Creates a dynamically allocated node_id
// Useful because the stack, queue and list are generic, and store void*
// node_id* because since the ADTs are generic, they do not memorize ints directly,
// so you can't use the normal node_id
node_id* create_node_id(node_id _id) {
    node_id* id = malloc(sizeof(node_id));
    if (id == NULL) return NULL;

    *id = _id;
    return id;   // newly created memory address
}

indirect_graph indirect_graph_create(int max_size) {
    if (max_size <= 0) return NULL;

    indirect_graph _g = malloc(sizeof(struct _indirect_graph));
    if (_g == NULL) return NULL;

    _g->nodes = malloc(sizeof(void*) * max_size);
    if (_g->nodes == NULL) {
        free(_g);
        return NULL;
    }

    _g->edges = calloc(TMATRIX_SIZE(max_size), sizeof(int));
    if (_g->edges == NULL) {
        free(_g->edges);
        free(_g);
        return NULL;
    }

    _g->size = 0;
    _g->max_size = max_size;
    return _g;
}

// Why free all these?
void indirect_graph_destroy(indirect_graph* _g) {
    if (_g == NULL || *_g == NULL) return;
    free((*_g)->nodes);
    free((*_g)->edges);
    free(*_g);
    *_g = NULL;
}


node_id add_node(indirect_graph _g, void* _value) {
    if (_g == NULL) return INDIRECT_GRAPH_ERROR_NULL;
    if (_g->size == _g->max_size) return INDIRECT_GRAPH_ERROR_MEMORY;

    node_id new_node_id = _g->size;
    _g->nodes[new_node_id] = _value;
    _g->size++;

    return new_node_id;
}


int indirect_graph_get_value(indirect_graph _g, node_id _node, void** _value_out) {
    if (_g == NULL) return INDIRECT_GRAPH_ERROR_NULL;
    if (_node < 0 ||_node  >= _g->size) return INDIRECT_GRAPH_ERROR_INVALID_ID;

    *_value_out = _g->nodes[_node];
    return INDIRECT_GRAPH_SUCCESS;
}


int add_edge(indirect_graph _g, node_id _src, node_id _dst) {
    if (_g == NULL) return INDIRECT_GRAPH_ERROR_NULL;
    if (_src < 0 || _src >= _g->size || _dst < 0 || _dst >= _g->size) return INDIRECT_GRAPH_ERROR_INVALID_ID;

    _g->edges[tmatrix_index(_src, _dst)] = 1;
    return INDIRECT_GRAPH_SUCCESS;
}


int remove_edge(indirect_graph _g, node_id _src, node_id _dst) {
    if (_g == NULL) return INDIRECT_GRAPH_ERROR_NULL;
    if (_src < 0 || _src >= _g->size || _dst < 0 || _dst >= _g->size) return INDIRECT_GRAPH_ERROR_INVALID_ID;

    _g->edges[tmatrix_index(_src, _dst)] = 0;
    return INDIRECT_GRAPH_SUCCESS;
}


int indirect_graph_size(indirect_graph _g) {
    if (_g == NULL) return INDIRECT_GRAPH_ERROR_NULL;
    return _g->size;
}


int indirect_graph_adjacent(indirect_graph _g, node_id _src, node_id _dst) {
    if (_g == NULL) return INDIRECT_GRAPH_ERROR_NULL;
    if (_src < 0 || _src >= _g->size || _dst < 0 || _dst >= _g->size) return INDIRECT_GRAPH_ERROR_INVALID_ID;

    return _g->edges[tmatrix_index(_src, _dst)];
}


list indirect_graph_neighbors(indirect_graph _g, node_id _node) {
    if (_g == NULL || _node < 0 || _node >= _g->size) return NULL;

    list _list = list_create();

    for (int i = 0; i < _g->size; i++) {
        if (_g->edges[tmatrix_index(_node, i)] == 1) {
            // What did you do here?
            node_id* neighbor = create_node_id(i);
            if (neighbor == NULL) {
                list_destroy(&_list);
                return NULL;
            }

            list_insert_back(_list, neighbor);
        }
    }
    return _list;
}


list indirect_graph_dfs(indirect_graph _g, node_id _start) {
    if (_g == NULL || _start < 0 || _start >= _g->size) return NULL;

    bool* visited = calloc(_g->size, sizeof(bool));
    list result = list_create();
    stack s = create_stack(_g->size);

    node_id* start = create_node_id(_start);

    stack_push(s, start);

    while (!stack_is_empty(s)) {
        void* value = NULL;
        stack_pop(s, &value);

        node_id current = *((node_id*)value);
        free(value);

        if (visited[current]) continue;

        visited[current] = true;

        node_id* result_id = create_node_id(current);
        list_insert_back(result, result_id);

        list neighbors = indirect_graph_neighbors(_g, current);
        iterator it = list_iterator_create(neighbors);

        while (iterator_has_next(it)) {
            void* neighbor_value = iterator_next(it);
            node_id neighbor = *((node_id*)neighbor_value);

            if (!visited[neighbor]) {
                node_id* neighbor_id = create_node_id(neighbor);
                stack_push(s, neighbor_id);
            }
        }
        iterator_destroy(&it);
        list_destroy(&neighbors);
    }
    destroy_stack(&s);
    free(visited);
    return result;
}


list indirect_graph_bfs(indirect_graph _g, node_id _start) {
    if (_g == NULL || _start < 0 || _start >= _g->size) return NULL;

    bool* visited = calloc(_g->size, sizeof(bool));
    list result = list_create();
    queue q = create_queue(_g->size);

    node_id* start = create_node_id(_start);

    // Marking _start as visited when inserting it into the queue
    // This prevents the same node from being inserted into the queue multiple times
    visited[_start] = true;
    enqueue(q, start);

    while (!queue_is_empty(q)) {
        void* value = NULL;
        dequeue(q, &value);

        node_id current = *((node_id*)value);
        free(value);

        node_id* result_id = create_node_id(current);

        list_insert_back(result, result_id);

        list neighbors = indirect_graph_neighbors(_g, current);
        iterator it = list_iterator_create(neighbors);

        while (iterator_has_next(it)) {
            void* neighbor_value = iterator_next(it);
            node_id neighbor = *((node_id*)neighbor_value);

            if (!visited[neighbor]) {
                visited[neighbor] = true;
                node_id* neighbor_id = create_node_id(neighbor);
                enqueue(q, neighbor_id);
            }
        }
        iterator_destroy(&it);
        list_destroy(&neighbors);
    }
    destroy_queue(&q);
    free(visited);
    return result;
}

// Uses the DFS to find if a path between 2 nodes exists
int indirect_graph_path_exists(indirect_graph _g, node_id _src, node_id _dst) {
    if (_g == NULL) return INDIRECT_GRAPH_ERROR_NULL;
    if (_src < 0 || _src >= _g->size || _dst < 0 || _dst >= _g->size) return INDIRECT_GRAPH_ERROR_INVALID_ID;

    // array, useful to keep track of the visited nodes
    bool* visited = calloc(_g->size, sizeof(bool));
    stack s = create_stack(_g->size);

    node_id* start = create_node_id(_src);
    stack_push(s, start);
    visited[_src] = true;

    while (!stack_is_empty(s)) {
        void* value = NULL;
        stack_pop(s, &value);

        // value is a void*, that points to a node_id so -> (node_id*)value
        // *((node_id*)value) -> "go to the address, and get its value"
        // current stores the node id
        // es. value = 3, after this -> node_id current = *((node_id*)value), current = 3
        node_id current = *((node_id*)value);
        free(value);

        // get the neighbors, nodes connected to current
        list neighbors = indirect_graph_neighbors(_g, current);
        iterator it = list_iterator_create(neighbors);

        while (iterator_has_next(it)) {
            // next element in the list
            void* neighbor_value = iterator_next(it);
            node_id neighbor = *((node_id*)neighbor_value);

            // check if it was already visited
            if (!visited[neighbor]) {

                // check if you found the _dst
                if (neighbor == _dst) {
                    iterator_destroy(&it);
                    list_destroy(&neighbors);
                    destroy_stack(&s);
                    free(visited);
                    return true;
                }

                // if neighbor is not the _dst
                visited[neighbor] = true;
                node_id* neighbor_id = create_node_id(neighbor);
                stack_push(s, neighbor_id);
            }
        }
        iterator_destroy(&it);
        list_destroy(&neighbors);
    }
    destroy_stack(&s);
    free(visited);

    // Returns true if the path is found, false otherwise
    // the function is int, true and false are compatible with integers
    return false;
}

int tmatrix_index(int i, int j) {
    int row = MAX(i, j);
    int col = MIN(i, j);
    return (row * (row + 1)) / 2 + col;
}
