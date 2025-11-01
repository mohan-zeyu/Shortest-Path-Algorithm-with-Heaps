#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include "Definition_fib_opt.h"

// Bidirectional Dijkstra with Fibonacci heap
int BidirectionalDijkstraFib(BiDirWorkSpaceFib *biworkspace, CSRGraph *graph, 
                             CSRGraph *reverse_graph, int src, int dst) {
    WorkSpaceFib *forward = biworkspace->forward;
    WorkSpaceFib *backward = biworkspace->backward;
    
    // Initialize distances and visited arrays
    for (int i = 0; i < graph->num_vertices; i++) {
        forward->distance[i] = INT_MAX;
        forward->visited[i] = false;
        backward->distance[i] = INT_MAX;
        backward->visited[i] = false;
    }
    
    // Reset heaps
    fibHeapReset(forward->heap);
    fibHeapReset(backward->heap);
    
    // Initialize source and destination
    forward->distance[src] = 0;
    backward->distance[dst] = 0;
    fibHeapInsert(forward->heap, src, 0);
    fibHeapInsert(backward->heap, dst, 0);
    
    int best_distance = INT_MAX;
    int meeting_vertex = -1;
    int forward_processed = 0, backward_processed = 0;
    
    // Main bidirectional search loop
    while (!fibHeapEmpty(forward->heap) || !fibHeapEmpty(backward->heap)) {
        // Check termination condition
        int forward_min = fibHeapGetMinKey(forward->heap);
        int backward_min = fibHeapGetMinKey(backward->heap);
        
        if (forward_min != INT_MAX && backward_min != INT_MAX) {
            if (forward_min + backward_min >= best_distance) {
                break;  // Found optimal path
            }
        }
        
        // Choose direction - expand the side with fewer processed vertices for balance
        bool expand_forward = false;
        if (fibHeapEmpty(backward->heap)) {
            expand_forward = true;
        } else if (!fibHeapEmpty(forward->heap)) {
            expand_forward = (forward_processed <= backward_processed);
        }
        
        // Expand from forward direction
        if (expand_forward && !fibHeapEmpty(forward->heap)) {
            int u = fibHeapExtractMin(forward->heap);
            
            if (u == -1 || forward->visited[u]) continue;
            forward->visited[u] = true;
            forward_processed++;
            
            // Check if we've found a path through this vertex
            if (backward->distance[u] != INT_MAX) {
                int total = forward->distance[u] + backward->distance[u];
                if (total < best_distance) {
                    best_distance = total;
                    meeting_vertex = u;
                }
            }
            
            // Relax edges
            int start = graph->row[u];
            int end = graph->row[u + 1];
            
            for (int i = start; i < end; i++) {
                int v = graph->col[i];
                
                if (forward->visited[v]) continue;
                
                int weight = graph->weight[i];
                int newDist = forward->distance[u] + weight;
                
                if (newDist < forward->distance[v]) {
                    forward->distance[v] = newDist;
                    
                    // Use insert (which handles decrease-key internally)
                    fibHeapInsert(forward->heap, v, newDist);
                    
                    // Check for meeting point
                    if (backward->distance[v] != INT_MAX) {
                        int total = newDist + backward->distance[v];
                        if (total < best_distance) {
                            best_distance = total;
                            meeting_vertex = v;
                        }
                    }
                }
            }
        } 
        // Expand from backward direction
        else if (!fibHeapEmpty(backward->heap)) {
            int u = fibHeapExtractMin(backward->heap);
            
            if (u == -1 || backward->visited[u]) continue;
            backward->visited[u] = true;
            backward_processed++;
            
            // Check if we've found a path through this vertex
            if (forward->distance[u] != INT_MAX) {
                int total = forward->distance[u] + backward->distance[u];
                if (total < best_distance) {
                    best_distance = total;
                    meeting_vertex = u;
                }
            }
            
            // Relax edges in reverse graph
            int start = reverse_graph->row[u];
            int end = reverse_graph->row[u + 1];
            
            for (int i = start; i < end; i++) {
                int v = reverse_graph->col[i];
                
                if (backward->visited[v]) continue;
                
                int weight = reverse_graph->weight[i];
                int newDist = backward->distance[u] + weight;
                
                if (newDist < backward->distance[v]) {
                    backward->distance[v] = newDist;
                    
                    // Use insert (which handles decrease-key internally)
                    fibHeapInsert(backward->heap, v, newDist);
                    
                    // Check for meeting point
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
    
    return (best_distance == INT_MAX) ? -1 : best_distance;
}
