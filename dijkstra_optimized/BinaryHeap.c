#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "Definition_opt.h"

BinaryHeap* createHeap(int capacity) {
    BinaryHeap* heap = (BinaryHeap*)malloc(sizeof(BinaryHeap));
    heap->vertices = (int*)malloc(sizeof(int) * capacity);
    heap->keys = (int*)malloc(sizeof(int) * capacity);
    heap->pos = (int*)malloc(sizeof(int) * capacity);
    heap->size = 0;
    heap->capacity = capacity;
    
    // Initialize positions to -1 (not in heap)
    for (int i = 0; i < capacity; i++) {
        heap->pos[i] = -1;
    }
    
    return heap;
}

void freeHeap(BinaryHeap* heap) {
    free(heap->vertices);
    free(heap->keys);
    free(heap->pos);
    free(heap);
}

static void swap(BinaryHeap* heap, int i, int j) {
    int v1 = heap->vertices[i];
    int v2 = heap->vertices[j];
    
    heap->vertices[i] = v2;
    heap->vertices[j] = v1;
    
    int k1 = heap->keys[i];
    heap->keys[i] = heap->keys[j];
    heap->keys[j] = k1;
    
    heap->pos[v1] = j;
    heap->pos[v2] = i;
}

static void heapifyUp(BinaryHeap* heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap->keys[idx] < heap->keys[parent]) {
            swap(heap, idx, parent);
            idx = parent;
        } else {
            break;
        }
    }
}

static void heapifyDown(BinaryHeap* heap, int idx) {
    while (2 * idx + 1 < heap->size) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = idx;
        
        if (left < heap->size && heap->keys[left] < heap->keys[smallest]) {
            smallest = left;
        }
        if (right < heap->size && heap->keys[right] < heap->keys[smallest]) {
            smallest = right;
        }
        
        if (smallest != idx) {
            swap(heap, idx, smallest);
            idx = smallest;
        } else {
            break;
        }
    }
}

void heapPush(BinaryHeap* heap, int vertex, int key) {
    if (heap->pos[vertex] != -1) {
        // Vertex already in heap, update if new key is smaller
        int idx = heap->pos[vertex];
        if (key < heap->keys[idx]) {
            heap->keys[idx] = key;
            heapifyUp(heap, idx);
        }
        return;
    }
    
    if (heap->size >= heap->capacity) {
        return; // Heap full
    }
    
    int idx = heap->size;
    heap->vertices[idx] = vertex;
    heap->keys[idx] = key;
    heap->pos[vertex] = idx;
    heap->size++;
    
    heapifyUp(heap, idx);
}

int heapPop(BinaryHeap* heap) {
    if (heap->size == 0) {
        return -1;
    }
    
    int vertex = heap->vertices[0];
    heap->pos[vertex] = -1;
    
    heap->size--;
    if (heap->size > 0) {
        heap->vertices[0] = heap->vertices[heap->size];
        heap->keys[0] = heap->keys[heap->size];
        heap->pos[heap->vertices[0]] = 0;
        heapifyDown(heap, 0);
    }
    
    return vertex;
}

bool heapEmpty(BinaryHeap* heap) {
    return heap->size == 0;
}

void heapDecrease(BinaryHeap* heap, int vertex, int newKey) {
    int idx = heap->pos[vertex];
    if (idx == -1) {
        heapPush(heap, vertex, newKey);
        return;
    }
    
    if (newKey < heap->keys[idx]) {
        heap->keys[idx] = newKey;
        heapifyUp(heap, idx);
    }
}

int heapTopKey(BinaryHeap* heap) {
    if (heap->size == 0) return INT_MAX;
    return heap->keys[0];
}
