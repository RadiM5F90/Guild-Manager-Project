
#include <stdlib.h>
#include <stdbool.h>
#include "linked_list.h"

#define INDIRECT_GRAPH_SUCCESS 0
#define INDIRECT_GRAPH_ERROR_NULL -1
#define INDIRECT_GRAPH_ERROR_INVALID_ID -2
#define INDIRECT_GRAPH_ERROR_MEMORY -3

typedef struct _indirect_graph* indirect_graph;
typedef int node_id;

indirect_graph indirect_graph_create(int max_size);

void indirect_graph_destroy(indirect_graph* _g);

node_id add_node(indirect_graph _g, void* _value);

int indirect_graph_get_value(indirect_graph _g, node_id _node, void** _value_out);

int add_edge(indirect_graph _g, node_id _src, node_id _dst);

int remove_edge(indirect_graph _g, node_id _src, node_id _dst);

int indirect_graph_size(indirect_graph _g);

int indirect_graph_adjacent(indirect_graph _g, node_id _src, node_id _dst);




list indirect_graph_neighbors(indirect_graph _g, node_id _node);

list indirect_graph_dfs(indirect_graph _g, node_id _start);

list indirect_graph_bfs(indirect_graph _g, node_id _start);


int indirect_graph_path_exists(indirect_graph _g, node_id _src, node_id _dst);
