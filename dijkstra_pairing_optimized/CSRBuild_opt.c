#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Definition_pairing_opt.h"

// Edge structure for temporary storage
typedef struct {
    int src;
    int dst;
    int weight;
} Edge;

CSRGraph* CSRBuild(FILE* file) {
    // FILE *file = fopen("../USA-road-d.NY.gr", "r");
    
    // Create graph structure
    CSRGraph *graph = (CSRGraph*)malloc(sizeof(CSRGraph));
    
    // Read header to get actual sizes
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'p') {
            sscanf(line, "p sp %d %d", &graph->num_vertices, &graph->num_edges);
            break;
        }
    }
    
    
    printf("Building graph with %d vertices and %d edges\n", graph->num_vertices, graph->num_edges);
    
    // Allocate memory
    graph->row = (int *)calloc(graph->num_vertices + 1, sizeof(int));
    graph->col = (int *)malloc(sizeof(int) * graph->num_edges);
    graph->weight = (int *)malloc(sizeof(int) * graph->num_edges);
    
    // Temporary storage for edges
    Edge *edges = (Edge *)malloc(graph->num_edges * sizeof(Edge));
    int *deg = (int *)calloc(graph->num_vertices, sizeof(int));
    
    // Read all edges
    int src, dst, weight, id = 0;
    char type;
    fseek(file, 0, SEEK_SET); // Reset to beginning
    
    while (fscanf(file, " %c", &type) == 1) {
        if (type == 'a') {
            if (fscanf(file, " %d %d %d", &src, &dst, &weight) == 3) {
                edges[id].src = src - 1;
                edges[id].dst = dst - 1;
                edges[id].weight = weight;
                deg[src - 1]++;
                id++;
            }
        } else {
            // Skip non-arc lines
            fgets(line, sizeof(line), file);
        }
    }
    
    // Build CSR structure
    graph->row[0] = 0;
    for (int i = 0; i < graph->num_vertices; i++) {
        graph->row[i + 1] = graph->row[i] + deg[i];
    }
    
    free(deg);
    
    int *temp = (int *)malloc(sizeof(int) * graph->num_vertices);
    memcpy(temp, graph->row, graph->num_vertices * sizeof(int));
    
    for (int i = 0; i < graph->num_edges; i++) {
        int s = edges[i].src;
        int p = temp[s]++;
        graph->col[p] = edges[i].dst;
        graph->weight[p] = edges[i].weight;
    }
    
    free(temp);
    free(edges);
    
    return graph;
}

// Build reverse graph for bidirectional search
CSRGraph* CSRBuildReverse(CSRGraph* forward_graph) {
    CSRGraph *graph = (CSRGraph*)malloc(sizeof(CSRGraph));
    graph->num_vertices = forward_graph->num_vertices;
    graph->num_edges = forward_graph->num_edges;
    
    // Allocate memory
    graph->row = (int *)calloc(graph->num_vertices + 1, sizeof(int));
    graph->col = (int *)malloc(sizeof(int) * graph->num_edges);
    graph->weight = (int *)malloc(sizeof(int) * graph->num_edges);
    
    // Count in-degrees
    int *deg = (int *)calloc(graph->num_vertices, sizeof(int));
    for (int u = 0; u < forward_graph->num_vertices; u++) {
        for (int i = forward_graph->row[u]; i < forward_graph->row[u + 1]; i++) {
            int v = forward_graph->col[i];
            deg[v]++;
        }
    }
    
    // Build row offsets
    graph->row[0] = 0;
    for (int i = 0; i < graph->num_vertices; i++) {
        graph->row[i + 1] = graph->row[i] + deg[i];
    }
    
    // Reset degrees for insertion
    memset(deg, 0, sizeof(int) * graph->num_vertices);
    
    // Insert reversed edges
    for (int u = 0; u < forward_graph->num_vertices; u++) {
        for (int i = forward_graph->row[u]; i < forward_graph->row[u + 1]; i++) {
            int v = forward_graph->col[i];
            int weight = forward_graph->weight[i];
            
            int pos = graph->row[v] + deg[v]++;
            graph->col[pos] = u;
            graph->weight[pos] = weight;
        }
    }
    
    free(deg);
    return graph;
}
