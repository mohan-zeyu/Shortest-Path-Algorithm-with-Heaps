#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "Definition_pairing_opt.h"

// Memory pool for pairing heap nodes to reduce allocation overhead
typedef struct {
    PairNode* nodes;
    int* free_list;
    int free_count;
    int capacity;
} NodePool;

static NodePool* pool = NULL;

// Initialize memory pool
void initNodePool(int capacity) {
    if (pool != NULL) return;
    
    pool = (NodePool*)malloc(sizeof(NodePool));
    pool->nodes = (PairNode*)malloc(sizeof(PairNode) * capacity);
    pool->free_list = (int*)malloc(sizeof(int) * capacity);
    pool->capacity = capacity;
    pool->free_count = capacity;
    
    // Initialize free list
    for (int i = 0; i < capacity; i++) {
        pool->free_list[i] = i;
        pool->nodes[i].in_use = false;
    }
}

void freeNodePool() {
    if (pool != NULL) {
        free(pool->nodes);
        free(pool->free_list);
        free(pool);
        pool = NULL;
    }
}

// Get a node from the pool
static PairNode* allocNode(int dst, int weight) {
    if (pool == NULL || pool->free_count == 0) {
        // Fallback to malloc if pool is exhausted
        PairNode* node = (PairNode*)malloc(sizeof(PairNode));
        node->weight = weight;
        node->dst = dst;
        node->LeftChild = NULL;
        node->NextSibling = NULL;
        node->Previous = NULL;
        node->in_use = true;
        node->in_heap = true;
        return node;
    }
    
    int idx = pool->free_list[--pool->free_count];
    PairNode* node = &pool->nodes[idx];
    
    node->weight = weight;
    node->dst = dst;
    node->LeftChild = NULL;
    node->NextSibling = NULL;
    node->Previous = NULL;
    node->in_use = true;
    node->in_heap = true;
    
    return node;
}

// Return a node to the pool
static void freeNode(PairNode* node) {
    if (pool == NULL) {
        free(node);
        return;
    }
    
    // Check if this node is from the pool
    if (node >= pool->nodes && node < pool->nodes + pool->capacity) {
        int idx = node - pool->nodes;
        node->in_use = false;
        node->in_heap = false;
        if (pool->free_count < pool->capacity) {
            pool->free_list[pool->free_count++] = idx;
        }
    } else {
        // Node was allocated with malloc
        free(node);
    }
}

// Create pairing heap
PairingHeap* createPairingHeap(int capacity) {
    PairingHeap* heap = (PairingHeap*)malloc(sizeof(PairingHeap));
    heap->root = NULL;
    heap->node_map = (PairNode**)calloc(capacity, sizeof(PairNode*));
    heap->size = 0;
    heap->capacity = capacity;
    
    // Initialize node pool if not already done
    if (pool == NULL) {
        initNodePool(capacity);
    }
    
    return heap;
}

// Free pairing heap
void freePairingHeap(PairingHeap* heap) {
    // Clear all nodes
    for (int i = 0; i < heap->capacity; i++) {
        if (heap->node_map[i] != NULL && heap->node_map[i]->in_heap) {
            freeNode(heap->node_map[i]);
        }
    }
    free(heap->node_map);
    free(heap);
}

// Combine two trees (link operation)
static PairNode* combineTreesOpt(PairNode* T1, PairNode* T2) {
    if (T1 == NULL) return T2;
    if (T2 == NULL) return T1;
    
    if (T1->weight > T2->weight) {
        // Swap to maintain heap property
        PairNode* temp = T1;
        T1 = T2;
        T2 = temp;
    }
    
    // Make T2 the leftmost child of T1
    T2->NextSibling = T1->LeftChild;
    if (T1->LeftChild != NULL) {
        T1->LeftChild->Previous = T2;
    }
    T2->Previous = T1;
    T1->LeftChild = T2;
    
    return T1;
}

// Insert or decrease key
void pairingHeapInsert(PairingHeap* heap, int dst, int weight) {
    // Check if node already exists
    if (heap->node_map[dst] != NULL && heap->node_map[dst]->in_heap) {
        // Decrease key if new weight is smaller
        if (weight < heap->node_map[dst]->weight) {
            pairingHeapDecreaseKey(heap, dst, weight);
        }
        return;
    }
    
    // Create new node
    PairNode* node = allocNode(dst, weight);
    heap->node_map[dst] = node;
    
    // Insert into heap
    if (heap->root == NULL) {
        heap->root = node;
    } else {
        heap->root = combineTreesOpt(heap->root, node);
    }
    
    heap->size++;
}

// Two-pass pairing for DeleteMin
static PairNode* twoPassPairing(PairNode* firstChild) {
    if (firstChild == NULL || firstChild->NextSibling == NULL) {
        return firstChild;
    }
    
    // First pass: pair from left to right
    PairNode* pairs[10000];  // Stack for paired trees
    int pair_count = 0;
    
    while (firstChild != NULL) {
        PairNode* first = firstChild;
        PairNode* second = first->NextSibling;
        
        if (second != NULL) {
            firstChild = second->NextSibling;
            
            // Disconnect for pairing
            first->NextSibling = NULL;
            first->Previous = NULL;
            second->NextSibling = NULL;
            second->Previous = NULL;
            
            pairs[pair_count++] = combineTreesOpt(first, second);
        } else {
            pairs[pair_count++] = first;
            first->NextSibling = NULL;
            first->Previous = NULL;
            break;
        }
    }
    
    // Second pass: combine from right to left
    PairNode* result = pairs[--pair_count];
    while (pair_count > 0) {
        result = combineTreesOpt(pairs[--pair_count], result);
    }
    
    return result;
}

// Extract minimum
int pairingHeapExtractMin(PairingHeap* heap) {
    if (heap->root == NULL) {
        return -1;
    }
    
    PairNode* min = heap->root;
    int dst = min->dst;
    
    // Two-pass pairing of children
    heap->root = twoPassPairing(min->LeftChild);
    
    // Mark as not in heap
    min->in_heap = false;
    heap->node_map[dst] = NULL;
    heap->size--;
    
    // Return node to pool
    freeNode(min);
    
    return dst;
}

// Get minimum without extracting
int pairingHeapGetMin(PairingHeap* heap) {
    if (heap->root == NULL) {
        return -1;
    }
    return heap->root->dst;
}

// Get minimum key
int pairingHeapGetMinKey(PairingHeap* heap) {
    if (heap->root == NULL) {
        return INT_MAX;
    }
    return heap->root->weight;
}

// Decrease key operation
void pairingHeapDecreaseKey(PairingHeap* heap, int dst, int newWeight) {
    PairNode* node = heap->node_map[dst];
    
    if (node == NULL || !node->in_heap) {
        // Node not in heap, insert it
        pairingHeapInsert(heap, dst, newWeight);
        return;
    }
    
    if (newWeight >= node->weight) {
        return;  // New weight is not smaller
    }
    
    node->weight = newWeight;
    
    // If node is the root, we're done
    if (node == heap->root) {
        return;
    }
    
    // Detach node from its parent
    if (node->Previous != NULL) {
        if (node->Previous->LeftChild == node) {
            // Node is leftmost child
            node->Previous->LeftChild = node->NextSibling;
        } else {
            // Node is a sibling
            node->Previous->NextSibling = node->NextSibling;
        }
        
        if (node->NextSibling != NULL) {
            node->NextSibling->Previous = node->Previous;
        }
        
        node->NextSibling = NULL;
        node->Previous = NULL;
        
        // Merge with root
        heap->root = combineTreesOpt(heap->root, node);
    }
}

// Check if heap is empty
bool pairingHeapEmpty(PairingHeap* heap) {
    return heap->root == NULL;
}

// Reset heap for reuse
void pairingHeapReset(PairingHeap* heap) {
    // Mark all nodes as not in heap
    for (int i = 0; i < heap->capacity; i++) {
        if (heap->node_map[i] != NULL) {
            heap->node_map[i]->in_heap = false;
            freeNode(heap->node_map[i]);
            heap->node_map[i] = NULL;
        }
    }
    heap->root = NULL;
    heap->size = 0;
}
