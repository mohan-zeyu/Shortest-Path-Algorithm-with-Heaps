#ifndef DEFINITION_PAIRING_OPT_H
#define DEFINITION_PAIRING_OPT_H

#include <stdbool.h>

// CSR structure
typedef struct {
    int num_vertices;
    int num_edges;
    int *row;      // Size: num_vertices + 1
    int *col;      // Size: num_edges
    int *weight;   // Size: num_edges (edge weights)
} CSRGraph;

// Optimized pairing heap node with better tracking
typedef struct PairNode {
    int weight;
    int dst;
    struct PairNode* LeftChild;
    struct PairNode* NextSibling;
    struct PairNode* Previous;  // Parent or previous sibling
    bool in_use;
    bool in_heap;
} PairNode;

// Pairing heap structure with node mapping
typedef struct {
    PairNode* root;
    PairNode** node_map;  // Direct mapping from vertex to node
    int size;
    int capacity;
} PairingHeap;

// Workspace for Dijkstra with pairing heap
typedef struct {
    int* distance;
    bool* visited;
    PairingHeap* heap;
} WorkSpacePairing;

// Bidirectional workspace
typedef struct {
    WorkSpacePairing* forward;
    WorkSpacePairing* backward;
    int best_distance;
    int meeting_point;
} BiDirWorkSpacePairing;

// Function declarations
CSRGraph* CSRBuild(FILE *file);
CSRGraph* CSRBuildReverse(CSRGraph* forward_graph);
int DijkstraPairingOpt(WorkSpacePairing *workspace, CSRGraph *graph, int src, int dst);
int BidirectionalDijkstraPairing(BiDirWorkSpacePairing *workspace, CSRGraph *graph, CSRGraph *reverse_graph, int src, int dst);

// Pairing heap operations
PairingHeap* createPairingHeap(int capacity);
void freePairingHeap(PairingHeap* heap);
void pairingHeapInsert(PairingHeap* heap, int dst, int weight);
int pairingHeapExtractMin(PairingHeap* heap);
int pairingHeapGetMin(PairingHeap* heap);
int pairingHeapGetMinKey(PairingHeap* heap);
void pairingHeapDecreaseKey(PairingHeap* heap, int dst, int newWeight);
bool pairingHeapEmpty(PairingHeap* heap);
void pairingHeapReset(PairingHeap* heap);

// Memory pool management
void initNodePool(int capacity);
void freeNodePool();

#endif
