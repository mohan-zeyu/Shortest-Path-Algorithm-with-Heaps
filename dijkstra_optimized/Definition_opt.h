#ifndef DEFINITION_OPT_H
#define DEFINITION_OPT_H

#include <stdbool.h>

// CSR structure
typedef struct {
    int num_vertices;
    int num_edges;
    int *row;      // Size: num_vertices + 1
    int *col;      // Size: num_edges
    int *weight;   // Size: num_edges (edge weights)
} CSRGraph;

// Binary heap for better cache locality
typedef struct {
    int *vertices;  // vertex indices
    int *keys;      // distances/keys
    int *pos;       // position of vertex in heap
    int size;
    int capacity;
} BinaryHeap;

// Workspace for Dijkstra
typedef struct {
    int* distance;
    bool* visited;
    BinaryHeap* heap;
} WorkSpace;

// Bidirectional search workspace
typedef struct {
    WorkSpace* forward;
    WorkSpace* backward;
    int best_distance;
    int meeting_point;
} BiDirWorkSpace;

// Function declarations
CSRGraph* CSRBuild(FILE* file);
int Dijkstra(WorkSpace *workspace, CSRGraph *graph, int src, int dst);
int BidirectionalDijkstra(BiDirWorkSpace *workspace, CSRGraph *graph, CSRGraph *reverse_graph, int src, int dst);

// Binary heap operations
BinaryHeap* createHeap(int capacity);
void freeHeap(BinaryHeap* heap);
void heapPush(BinaryHeap* heap, int vertex, int key);
int heapPop(BinaryHeap* heap);
bool heapEmpty(BinaryHeap* heap);
void heapDecrease(BinaryHeap* heap, int vertex, int newKey);

#endif
