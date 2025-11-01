#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "Definition_fib_opt.h"

// MEMORY POOL MANAGEMENT for better cache performance and reduced allocation overhead

static inline FibNode* allocNode(FibonacciHeap* heap, int vertex, int key) {
    FibNode* node;
    
    if (heap->free_count > 0) {
        // Get from pool
        int idx = heap->free_list[--heap->free_count];
        node = &heap->node_pool[idx];
    } else {
        // Pool exhausted, use malloc (shouldn't happen with proper sizing)
        node = (FibNode*)malloc(sizeof(FibNode));
    }
    
    node->vertex = vertex;
    node->key = key;
    node->degree = 0;
    node->marked = false;
    node->parent = NULL;
    node->child = NULL;
    node->left = node;
    node->right = node;
    
    return node;
}

static inline void freeNode(FibonacciHeap* heap, FibNode* node) {
    // Check if node is from pool
    if (node >= heap->node_pool && node < heap->node_pool + heap->capacity) {
        int idx = node - heap->node_pool;
        if (heap->free_count < heap->capacity) {
            heap->free_list[heap->free_count++] = idx;
        }
    } else {
        free(node);
    }
}

// ============================================================================
// FIBONACCI HEAP OPERATIONS
// ============================================================================

FibonacciHeap* createFibHeap(int capacity) {
    FibonacciHeap* heap = (FibonacciHeap*)malloc(sizeof(FibonacciHeap));
    heap->min = NULL;
    heap->n = 0;
    heap->capacity = capacity;
    
    // Initialize node mapping for O(1) access
    heap->node_map = (FibNode**)calloc(capacity, sizeof(FibNode*));
    
    // Initialize memory pool
    heap->node_pool = (FibNode*)malloc(sizeof(FibNode) * capacity);
    heap->free_list = (int*)malloc(sizeof(int) * capacity);
    heap->free_count = capacity;
    
    for (int i = 0; i < capacity; i++) {
        heap->free_list[i] = i;
    }
    
    return heap;
}

void freeFibHeap(FibonacciHeap* heap) {
    // Free all nodes in node_map
    for (int i = 0; i < heap->capacity; i++) {
        if (heap->node_map[i] != NULL) {
            freeNode(heap, heap->node_map[i]);
            heap->node_map[i] = NULL;
        }
    }
    
    free(heap->node_map);
    free(heap->node_pool);
    free(heap->free_list);
    free(heap);
}

// Add node to root list (circular doubly-linked list)
static inline void addToRootList(FibonacciHeap* heap, FibNode* node) {
    if (heap->min == NULL) {
        heap->min = node;
        node->left = node;
        node->right = node;
    } else {
        node->right = heap->min->right;
        node->left = heap->min;
        heap->min->right->left = node;
        heap->min->right = node;
    }
}

// Remove node from its sibling list
static inline void removeFromList(FibNode* node) {
    node->left->right = node->right;
    node->right->left = node->left;
}

// Insert with lazy strategy - just add to root list
void fibHeapInsert(FibonacciHeap* heap, int vertex, int key) {
    // Check if vertex already in heap
    if (heap->node_map[vertex] != NULL) {
        // Decrease key if new key is smaller
        if (key < heap->node_map[vertex]->key) {
            fibHeapDecreaseKey(heap, vertex, key);
        }
        return;
    }
    
    FibNode* node = allocNode(heap, vertex, key);
    heap->node_map[vertex] = node;
    
    addToRootList(heap, node);
    
    if (node->key < heap->min->key) {
        heap->min = node;
    }
    
    heap->n++;
}

// Link tree y as child of x (used in consolidation)
static inline void link(FibonacciHeap* heap, FibNode* y, FibNode* x) {
    // Remove y from root list
    removeFromList(y);
    
    // Make y child of x
    y->parent = x;
    y->marked = false;
    
    if (x->child == NULL) {
        x->child = y;
        y->left = y;
        y->right = y;
    } else {
        y->right = x->child->right;
        y->left = x->child;
        x->child->right->left = y;
        x->child->right = y;
    }
    
    x->degree++;
}

// Consolidate trees to maintain degree invariant
static void consolidate(FibonacciHeap* heap) {
    if (heap->min == NULL) return;
    
    // Max degree is O(log n)
    int max_degree = (int)(log(heap->n) / log(2)) + 6;  // +6 for safety margin
    FibNode** degree_array = (FibNode**)calloc(max_degree, sizeof(FibNode*));
    
    // Collect all root nodes first to avoid infinite loop
    FibNode* roots[10000];  // Should be enough for root list
    int root_count = 0;
    
    FibNode* x = heap->min;
    if (x != NULL) {
        do {
            roots[root_count++] = x;
            x = x->right;
        } while (x != heap->min);
    }
    
    // Process each root
    for (int i = 0; i < root_count; i++) {
        x = roots[i];
        int d = x->degree;
        
        while (d < max_degree && degree_array[d] != NULL) {
            FibNode* y = degree_array[d];
            
            if (x->key > y->key) {
                FibNode* temp = x;
                x = y;
                y = temp;
            }
            
            link(heap, y, x);
            degree_array[d] = NULL;
            d++;
        }
        
        if (d < max_degree) {
            degree_array[d] = x;
        }
    }
    
    // Rebuild root list from degree array
    heap->min = NULL;
    for (int i = 0; i < max_degree; i++) {
        if (degree_array[i] != NULL) {
            if (heap->min == NULL) {
                heap->min = degree_array[i];
                heap->min->left = heap->min;
                heap->min->right = heap->min;
            } else {
                addToRootList(heap, degree_array[i]);
                if (degree_array[i]->key < heap->min->key) {
                    heap->min = degree_array[i];
                }
            }
        }
    }
    
    free(degree_array);
}

// Extract minimum - main expensive operation
int fibHeapExtractMin(FibonacciHeap* heap) {
    FibNode* z = heap->min;
    
    if (z == NULL) {
        return -1;
    }
    
    int vertex = z->vertex;
    
    // Add all children to root list
    if (z->child != NULL) {
        FibNode* child = z->child;
        FibNode* first_child = child;
        
        do {
            FibNode* next = child->right;
            child->parent = NULL;
            addToRootList(heap, child);
            child = next;
        } while (child != first_child);
    }
    
    // Remove z from root list
    if (z == z->right) {
        heap->min = NULL;
    } else {
        heap->min = z->right;
        removeFromList(z);
        consolidate(heap);
    }
    
    heap->n--;
    heap->node_map[vertex] = NULL;
    freeNode(heap, z);
    
    return vertex;
}

// Cut node from parent and add to root list
static void cut(FibonacciHeap* heap, FibNode* x, FibNode* y) {
    // Remove x from child list of y
    if (x->right == x) {
        y->child = NULL;
    } else {
        y->child = x->right;
        removeFromList(x);
    }
    
    y->degree--;
    
    // Add x to root list
    x->parent = NULL;
    x->marked = false;
    addToRootList(heap, x);
}

// Cascading cut for maintaining potential
static void cascadingCut(FibonacciHeap* heap, FibNode* y) {
    FibNode* z = y->parent;
    
    if (z != NULL) {
        if (!y->marked) {
            y->marked = true;
        } else {
            cut(heap, y, z);
            cascadingCut(heap, z);
        }
    }
}

// Decrease key - O(1) amortized
void fibHeapDecreaseKey(FibonacciHeap* heap, int vertex, int newKey) {
    FibNode* x = heap->node_map[vertex];
    
    if (x == NULL) {
        // Not in heap, insert it
        fibHeapInsert(heap, vertex, newKey);
        return;
    }
    
    if (newKey >= x->key) {
        return;  // Not actually decreasing
    }
    
    x->key = newKey;
    FibNode* y = x->parent;
    
    // If heap property violated, cut and do cascading cuts
    if (y != NULL && x->key < y->key) {
        cut(heap, x, y);
        cascadingCut(heap, y);
    }
    
    // Update min if necessary
    if (x->key < heap->min->key) {
        heap->min = x;
    }
}

// Get minimum key without extracting
int fibHeapGetMinKey(FibonacciHeap* heap) {
    if (heap->min == NULL) {
        return INT_MAX;
    }
    return heap->min->key;
}

// Check if heap is empty
bool fibHeapEmpty(FibonacciHeap* heap) {
    return heap->min == NULL;
}

// Reset heap for reuse
void fibHeapReset(FibonacciHeap* heap) {
    // Free all nodes and reset
    for (int i = 0; i < heap->capacity; i++) {
        if (heap->node_map[i] != NULL) {
            freeNode(heap, heap->node_map[i]);
            heap->node_map[i] = NULL;
        }
    }
    
    heap->min = NULL;
    heap->n = 0;
    
    // Reset free list
    heap->free_count = heap->capacity;
    for (int i = 0; i < heap->capacity; i++) {
        heap->free_list[i] = i;
    }
}
