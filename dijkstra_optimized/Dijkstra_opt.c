#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include "Definition_opt.h"

// Forward declaration for heap top key
int heapTopKey(BinaryHeap* heap);

// Standard Dijkstra with early termination
int Dijkstra(WorkSpace *workspace, CSRGraph *graph, int src, int dst) {
    // Reset workspace
    for (int i = 0; i < graph->num_vertices; i++) {
        workspace->distance[i] = INT_MAX;
        workspace->visited[i] = false;
    }
    workspace->heap->size = 0;
    memset(workspace->heap->pos, -1, sizeof(int) * graph->num_vertices);
    
    workspace->distance[src] = 0;
    heapPush(workspace->heap, src, 0);
    
    while (!heapEmpty(workspace->heap)) {
        int u = heapPop(workspace->heap);
        
        // Early termination when we reach destination
        if (u == dst) {
            return workspace->distance[dst];
        }
        
        if (workspace->visited[u]) continue;
        workspace->visited[u] = true;
        
        int start = graph->row[u];
        int end = graph->row[u + 1];
        
        for (int i = start; i < end; i++) {
            int v = graph->col[i];
            int weight = graph->weight[i];
            int newDist = workspace->distance[u] + weight;
            
            if (newDist < workspace->distance[v]) {
                workspace->distance[v] = newDist;
                heapPush(workspace->heap, v, newDist);
            }
        }
    }
    
    return INT_MAX; // No path found
}

// Bidirectional Dijkstra - much faster for point-to-point queries
int BidirectionalDijkstra(BiDirWorkSpace *biworkspace, CSRGraph *graph, CSRGraph *reverse_graph, int src, int dst) {
    WorkSpace *forward = biworkspace->forward;
    WorkSpace *backward = biworkspace->backward;
    
    // Initialize
    for (int i = 0; i < graph->num_vertices; i++) {
        forward->distance[i] = INT_MAX;
        forward->visited[i] = false;
        backward->distance[i] = INT_MAX;
        backward->visited[i] = false;
    }
    forward->heap->size = 0;
    backward->heap->size = 0;
    memset(forward->heap->pos, -1, sizeof(int) * graph->num_vertices);
    memset(backward->heap->pos, -1, sizeof(int) * graph->num_vertices);
    
    forward->distance[src] = 0;
    backward->distance[dst] = 0;
    heapPush(forward->heap, src, 0);
    heapPush(backward->heap, dst, 0);
    
    int best_distance = INT_MAX;
    int meeting_vertex = -1;
    
    while (!heapEmpty(forward->heap) || !heapEmpty(backward->heap)) {
        // Termination condition: if the sum of minimum keys in both heaps >= best_distance
        int forward_min = heapTopKey(forward->heap);
        int backward_min = heapTopKey(backward->heap);
        
        if (forward_min != INT_MAX && backward_min != INT_MAX) {
            if (forward_min + backward_min >= best_distance) {
                break;
            }
        }
        
        // Expand from the direction with smaller frontier
        bool expand_forward = false;
        if (heapEmpty(backward->heap)) {
            expand_forward = true;
        } else if (!heapEmpty(forward->heap)) {
            expand_forward = (forward->heap->size <= backward->heap->size);
        }
        
        if (expand_forward && !heapEmpty(forward->heap)) {
            int u = heapPop(forward->heap);
            
            if (forward->visited[u]) continue;
            forward->visited[u] = true;
            
            // Check if this vertex was reached from backward search
            if (backward->distance[u] != INT_MAX) {
                int total = forward->distance[u] + backward->distance[u];
                if (total < best_distance) {
                    best_distance = total;
                    meeting_vertex = u;
                }
            }
            
            int start = graph->row[u];
            int end = graph->row[u + 1];
            
            for (int i = start; i < end; i++) {
                int v = graph->col[i];
                int weight = graph->weight[i];
                int newDist = forward->distance[u] + weight;
                
                if (newDist < forward->distance[v]) {
                    forward->distance[v] = newDist;
                    heapPush(forward->heap, v, newDist);
                    
                    // Check for path through v
                    if (backward->distance[v] != INT_MAX) {
                        int total = newDist + backward->distance[v];
                        if (total < best_distance) {
                            best_distance = total;
                            meeting_vertex = v;
                        }
                    }
                }
            }
        } else if (!heapEmpty(backward->heap)) {
            int u = heapPop(backward->heap);
            
            if (backward->visited[u]) continue;
            backward->visited[u] = true;
            
            // Check if this vertex was reached from forward search
            if (forward->distance[u] != INT_MAX) {
                int total = forward->distance[u] + backward->distance[u];
                if (total < best_distance) {
                    best_distance = total;
                    meeting_vertex = u;
                }
            }
            
            int start = reverse_graph->row[u];
            int end = reverse_graph->row[u + 1];
            
            for (int i = start; i < end; i++) {
                int v = reverse_graph->col[i];
                int weight = reverse_graph->weight[i];
                int newDist = backward->distance[u] + weight;
                
                if (newDist < backward->distance[v]) {
                    backward->distance[v] = newDist;
                    heapPush(backward->heap, v, newDist);
                    
                    // Check for path through v
                    if (forward->distance[v] != INT_MAX) {
                        int total = forward->distance[v] + newDist;
                        if (total < best_distance) {
                            best_distance = total;
                            meeting_vertex = v;
                        }
                    }
                }
            }
        }
    }
    
    biworkspace->best_distance = best_distance;
    biworkspace->meeting_point = meeting_vertex;
    
    return best_distance;
}
