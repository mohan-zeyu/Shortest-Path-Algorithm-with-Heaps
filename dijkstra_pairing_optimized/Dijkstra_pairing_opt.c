#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include "Definition_pairing_opt.h"

// Bidirectional Dijkstra with pairing heap
int BidirectionalDijkstraPairing(BiDirWorkSpacePairing *biworkspace, CSRGraph *graph, 
                                  CSRGraph *reverse_graph, int src, int dst) {
    WorkSpacePairing *forward = biworkspace->forward;
    WorkSpacePairing *backward = biworkspace->backward;
    
    // Initialize forward search
    for (int i = 0; i < graph->num_vertices; i++) {
        forward->distance[i] = INT_MAX;
        forward->visited[i] = false;
        backward->distance[i] = INT_MAX;
        backward->visited[i] = false;
    }
    
    // Reset heaps
    pairingHeapReset(forward->heap);
    pairingHeapReset(backward->heap);
    
    forward->distance[src] = 0;
    backward->distance[dst] = 0;
    pairingHeapInsert(forward->heap, src, 0);
    pairingHeapInsert(backward->heap, dst, 0);
    
    int best_distance = INT_MAX;
    int meeting_vertex = -1;
    int forward_processed = 0, backward_processed = 0;
    
    while (!pairingHeapEmpty(forward->heap) || !pairingHeapEmpty(backward->heap)) {
        // Check termination condition
        int forward_min = pairingHeapGetMinKey(forward->heap);
        int backward_min = pairingHeapGetMinKey(backward->heap);
        
        if (forward_min != INT_MAX && backward_min != INT_MAX) {
            if (forward_min + backward_min >= best_distance) {
                break;  // Optimal path found
            }
        }
        
        // Choose direction with smaller frontier
        bool expand_forward = false;
        if (pairingHeapEmpty(backward->heap)) {
            expand_forward = true;
        } else if (!pairingHeapEmpty(forward->heap)) {
            // Expand the side with fewer processed vertices for balance
            expand_forward = (forward_processed <= backward_processed);
        }
        
        if (expand_forward && !pairingHeapEmpty(forward->heap)) {
            int u = pairingHeapExtractMin(forward->heap);
            
            if (u == -1 || forward->visited[u]) continue;
            forward->visited[u] = true;
            forward_processed++;
            
            // Check if path found
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
                    pairingHeapInsert(forward->heap, v, newDist);
                    
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
        } else if (!pairingHeapEmpty(backward->heap)) {
            int u = pairingHeapExtractMin(backward->heap);
            
            if (u == -1 || backward->visited[u]) continue;
            backward->visited[u] = true;
            backward_processed++;
            
            // Check if path found
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
                    pairingHeapInsert(backward->heap, v, newDist);
                    
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
