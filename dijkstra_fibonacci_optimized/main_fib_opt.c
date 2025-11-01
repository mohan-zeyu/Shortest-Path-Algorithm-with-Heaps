#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include "Definition_fib_opt.h"

// External functions
CSRGraph* CSRBuildReverse(CSRGraph* forward_graph);

typedef struct {
    int source;
    int target;
    int original_index;
} Query;

int main() {
    struct timeval start, end;
    
    printf("=== Fibonacci Heap Optimized Version ===\n");
    printf("Loading graph...\n");
    clock_t c0 = clock();
    printf("0: NY, 1: BAY, 2: COL, 3: FLA, 4: NW, 5: NE, 6: CAL, 7: LKS, 8: E, 9: W, 10: CTR, 11: USA\n");
    FILE *file;
    int choice;
    printf("City code name:\n");
    scanf("%d", &choice);
    
    // Input file and output file pointer
    FILE *filein, *fileout;
    switch (choice){
        case 0: file = fopen("../Map/USA-road-d.NY.gr", "r"); printf("NY\n"); 
            filein=fopen("../Queries/NY_Queries.txt","r"); fileout = fopen("../Results/NY_Answer_Fib.txt", "w"); break;
        case 1: file = fopen("../Map/USA-road-d.BAY.gr", "r"); printf("BAY\n");
            filein=fopen("../Queries/BAY_Queries.txt","r"); fileout = fopen("../Results/BAY_Answer_Fib.txt", "w"); break;
        case 2: file = fopen("../Map/USA-road-d.COL.gr", "r");printf("COL\n");
            filein=fopen("../Queries/COL_Queries.txt","r"); fileout = fopen("../Results/COL_Answer_Fib.txt", "w"); break;
        case 3: file = fopen("../Map/USA-road-d.FLA.gr", "r"); printf("FLA\n");
            filein=fopen("../Queries/FLA_Queries.txt","r"); fileout = fopen("../Results/FLA_Answer_Fib.txt", "w"); break;
        case 4: file = fopen("../Map/USA-road-d.NW.gr", "r");printf("NW\n");
            filein=fopen("../Queries/NW_Queries.txt","r"); fileout = fopen("../Results/NW_Answer_Fib.txt", "w"); break;
        case 5: file = fopen("../Map/USA-road-d.NE.gr", "r");printf("NE\n");
            filein=fopen("../Queries/NE_Queries.txt","r"); fileout = fopen("../Results/NE_Answer_Fib.txt", "w"); break;
        case 6: file = fopen("../Map/USA-road-d.CAL.gr", "r"); printf("CAL\n");
            filein=fopen("../Queries/CAL_Queries.txt","r"); fileout = fopen("../Results/CAL_Answer_Fib.txt", "w"); break;
        case 7: file = fopen("../Map/USA-road-d.LKS.gr", "r"); printf("LKS\n");
            filein=fopen("../Queries/LKS_Queries.txt","r"); fileout = fopen("../Results/LKS_Answer_Fib.txt", "w"); break;
        case 8: file = fopen("../Map/USA-road-d.E.gr", "r"); printf("E\n");
            filein=fopen("../Queries/E_Queries.txt","r"); fileout = fopen("../Results/E_Answer_Fib.txt", "w"); break;
        case 9: file = fopen("../Map/USA-road-d.W.gr", "r"); printf("W\n");
            filein=fopen("../Queries/W_Queries.txt","r"); fileout = fopen("../Results/W_Answer_Fib.txt", "w"); break;
        case 10: file = fopen("../Map/USA-road-d.CTR.gr", "r"); printf("CTR\n");
            filein=fopen("../Queries/CTR_Queries.txt","r"); fileout = fopen("../Results/CTR_Answer_Fib.txt", "w"); break;
        case 11: file = fopen("../Map/USA-road-d.USA.gr", "r"); printf("USA\n");
            filein=fopen("../Queries/USA_Queries.txt","r"); fileout = fopen("../Results/USA_Answer_Fib.txt", "w"); break;
        default: printf("Invalid choice!\n"); return 2;
    }
    
    if (!file) {
        fprintf(stderr, "Failed to open graph file\n");
        return 1;
    }
    
    // Graph initialization
    CSRGraph* graph = CSRBuild(file);
    fclose(file);
    if (!graph) {
        fprintf(stderr, "Failed to build graph\n");
        return 1;
    }
    
    // Build reverse graph for bidirectional search
    printf("Building reverse graph...\n");
    CSRGraph* reverse_graph = CSRBuildReverse(graph);
    
    clock_t c1 = clock();
    double cpu_seconds = (double)(c1 - c0) / CLOCKS_PER_SEC;
    printf("Graph loading time: %.3f seconds\n", cpu_seconds);
    
    // Initialize Fibonacci heap workspaces
    BiDirWorkSpaceFib* bidir_workspace = (BiDirWorkSpaceFib*)malloc(sizeof(BiDirWorkSpaceFib));
    
    bidir_workspace->forward = (WorkSpaceFib*)malloc(sizeof(WorkSpaceFib));
    bidir_workspace->forward->distance = (int*)malloc(sizeof(int) * graph->num_vertices);
    bidir_workspace->forward->visited = (bool*)malloc(sizeof(bool) * graph->num_vertices);
    bidir_workspace->forward->heap = createFibHeap(graph->num_vertices);
    
    bidir_workspace->backward = (WorkSpaceFib*)malloc(sizeof(WorkSpaceFib));
    bidir_workspace->backward->distance = (int*)malloc(sizeof(int) * graph->num_vertices);
    bidir_workspace->backward->visited = (bool*)malloc(sizeof(bool) * graph->num_vertices);
    bidir_workspace->backward->heap = createFibHeap(graph->num_vertices);
    
    if (!filein || !fileout) {
        perror("Cannot open query/output file");
        return 1;
    }
    
    // Process query file
    int query_count;
    fscanf(filein, "%d", &query_count);
    Query *queries = (Query*)malloc(sizeof(Query) * query_count);
    
    int cnt = 0;
    while (fscanf(filein, " %d %d", &(queries[cnt].source), &(queries[cnt].target)) == 2) {
        queries[cnt].source--;
        queries[cnt].target--;
        queries[cnt].original_index = cnt;
        cnt++;
    }
    fclose(filein);
    
    int* results = (int*)malloc(sizeof(int) * query_count);
    
    printf("Processing %d queries with Fibonacci heap...\n", query_count);
    
    // Use bidirectional Dijkstra with Fibonacci heap
    clock_t c2 = clock();
    for (int i = 0; i < query_count; i++) {
        results[i] = BidirectionalDijkstraFib(bidir_workspace, graph, reverse_graph, 
                                              queries[i].source, queries[i].target);
        if(i % 50 == 0) {
            printf("%d queries processed\n", i);
        }
    }
    
    clock_t c3 = clock();
    double query_seconds = (double)(c3 - c2) / CLOCKS_PER_SEC;
    printf("\n=== Performance Results ===\n");
    printf("Query processing time: %.3f seconds\n", query_seconds);
    printf("Average time per query: %.4f seconds\n", query_seconds / query_count);
    printf("Queries per second: %.2f\n", query_count / query_seconds);
    
    // Write results
    for (int i = 0; i < query_count; i++) {
        fprintf(fileout, "%d\n", results[i]);
    }
    fclose(fileout);
    
    printf("Results written successfully!\n");
    
    // Cleanup
    free(bidir_workspace->forward->distance);
    free(bidir_workspace->forward->visited);
    freeFibHeap(bidir_workspace->forward->heap);
    free(bidir_workspace->forward);
    
    free(bidir_workspace->backward->distance);
    free(bidir_workspace->backward->visited);
    freeFibHeap(bidir_workspace->backward->heap);
    free(bidir_workspace->backward);
    
    free(bidir_workspace);
    
    free(queries);
    free(results);
    
    free(graph->col);
    free(graph->row);
    free(graph->weight);
    free(graph);
    
    free(reverse_graph->col);
    free(reverse_graph->row);
    free(reverse_graph->weight);
    free(reverse_graph);
    
    return 0;
}
