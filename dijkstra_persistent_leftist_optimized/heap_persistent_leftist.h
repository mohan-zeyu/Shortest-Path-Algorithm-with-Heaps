#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdbool.h>
#include <limits.h>

#define INF INT_MAX
typedef struct PLHNode {
    int key;    // Distance
    int vertex; // vertex index(1-based)
    int npl;    // Null Path Length (Rank)
    struct PLHNode *left;
    struct PLHNode *right;
} PLHNode;
typedef struct {
    PLHNode* root;
    int size; 
} AbstractHeap;//Abstract Heap Structure (contains only root node and size, to be passed via pointer)
typedef struct {
    int distance; 
    int vertex;   
} DijkstraItem;
typedef struct Edge {
    int target_vertex;
    int weight;
    struct Edge *next;
} Edge;
typedef struct Graph {
    int num_vertices;
    Edge **adj_list; 
} Graph;
typedef struct {
    AbstractHeap* (*create)(); //create,@return  pointer to a heap
    void (*insert)(AbstractHeap *H, int distance, int vertex); // insert: takes pointer to the heap and modifies it in place
    void (*pop)(AbstractHeap *H, DijkstraItem *out_item);
    bool (*is_empty)(AbstractHeap *H);
    int (*min_key)(AbstractHeap *H); // Min_key takes pointer to the heap
    void (*free_heap)(AbstractHeap *H); //Releases memory
} HeapInterface;
int bidirectional_dijkstra(const Graph *G, const Graph *G_rev, 
                           int s, int t, const HeapInterface *interface);
void graph_add_edge(Graph *G, int u, int v, int w);
void graph_free(Graph *G);
extern const HeapInterface LeftistHeapInterface;

#endif 