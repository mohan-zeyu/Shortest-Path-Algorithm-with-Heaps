#ifndef DEFINITION_FIB_OPT_H
#define DEFINITION_FIB_OPT_H

#include <stdbool.h>

// CSR structure
typedef struct {
    int num_vertices;
    int num_edges;
    int *row;      // Size: num_vertices + 1
    int *col;      // Size: num_edges
    int *weight;   // Size: num_edges (edge weights)
} CSRGraph;

// Fibonacci heap node
typedef struct FibNode {
    int vertex;
    int key;
    int degree;
    bool marked;
    struct FibNode* parent;
    struct FibNode* child;
    struct FibNode* left;
    struct FibNode* right;
} FibNode;

// Fibonacci heap structure with optimizations
typedef struct {
    FibNode* min;
    FibNode** node_map;  // Direct mapping from vertex to node for O(1) access
    int n;               // Number of nodes
    int capacity;
    FibNode* node_pool;  // Memory pool for nodes
    int* free_list;
    int free_count;
} FibonacciHeap;

// Workspace for Dijkstra
typedef struct {
    int* distance;
    bool* visited;
    FibonacciHeap* heap;
} WorkSpaceFib;

// Bidirectional workspace
typedef struct {
    WorkSpaceFib* forward;
    WorkSpaceFib* backward;
    int best_distance;
    int meeting_point;
} BiDirWorkSpaceFib;

// Function declarations
CSRGraph* CSRBuild(FILE *file);
CSRGraph* CSRBuildReverse(CSRGraph* forward_graph);
int BidirectionalDijkstraFib(BiDirWorkSpaceFib *workspace, CSRGraph *graph, 
                             CSRGraph *reverse_graph, int src, int dst);

// Fibonacci heap operations
FibonacciHeap* createFibHeap(int capacity);
void freeFibHeap(FibonacciHeap* heap);
void fibHeapInsert(FibonacciHeap* heap, int vertex, int key);
int fibHeapExtractMin(FibonacciHeap* heap);
int fibHeapGetMinKey(FibonacciHeap* heap);
bool fibHeapEmpty(FibonacciHeap* heap);
void fibHeapDecreaseKey(FibonacciHeap* heap, int vertex, int newKey);
void fibHeapReset(FibonacciHeap* heap);

#endif
