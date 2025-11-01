#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdbool.h>
#include <limits.h>
#include <stdlib.h> 

#define INF INT_MAX// Define infinity for distances

// PLHNode (Leftist Heap Node) definition
typedef struct PLHNode {
    int key;  // Distance from the source
    int vertex; //1-based vertex index
    int npl;    // Null Path Length (Rank)
    struct PLHNode *left;
    struct PLHNode *right;
} PLHNode;
typedef struct {// Abstract Heap structure (Passed by pointer for in-place modification)
    PLHNode* root;
    int size; // Current number of elements in the heap
} AbstractHeap;
typedef struct {// Dijkstra result item
    int distance; 
    int vertex;   
} DijkstraItem;
typedef struct Edge {
    int target_vertex;
    int weight;
    struct Edge *next;
} Edge;

typedef struct Graph {
    int num_vertices;// Number of vertices V
    Edge **adj_list; // Adjacency list array (1-based)
} Graph;


//  Heap Interface 
typedef struct {
    AbstractHeap* (*create)(); //create(need malloc)
    void (*insert)(AbstractHeap *H, int distance, int vertex); 
    void (*pop)(AbstractHeap *H, DijkstraItem *out_item);//pop  @return out_item
    bool (*is_empty)(AbstractHeap *H);
    int (*min_key)(AbstractHeap *H); 
    void (*free_heap)(AbstractHeap *H); 
} HeapInterface;
// Performs Bidirectional Dijkstra for Point-to-Point queries.
int bidirectional_dijkstra(const Graph *G, const Graph *G_rev, int s, int t, const HeapInterface *interface);
//Graph Utility Functions 
Graph* create_graph(int N);
void graph_add_edge(Graph *G, int u, int v, int w);
void graph_free(Graph *G);
extern const HeapInterface LeftistHeapInterface;
#endif 