#include "dijkstra_leftist.h"
#include "timing_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
extern const HeapInterface LeftistHeapInterface;
Graph* create_graph(int N);
void graph_add_edge(Graph *G, int u, int v, int w);
void graph_free(Graph *G);
int N,M;
char tmp[100];
FILE *filein, *fileout,*file;
typedef struct {
    int u, v, w;
} TempEdge;
int getMax(int a,int b){
    return a>b?a:b;
}
Graph* initialize_graph_dynamic(Graph **G_rev_ptr, int *N_ptr) {
    int max_vertex = 0, edge_count = 0;// Dynamic reading to determine V and E 
    size_t temp_capacity = 1048576; // Optimized initial allocation for large data (1MB of edges initially)
    TempEdge *temp_edges = (TempEdge*)malloc(temp_capacity * sizeof(TempEdge));
    if (!temp_edges) {
        perror("Initial temp_edges allocation failed");
        return NULL;
    }
    printf("Starting first pass: Reading edges from stdin and calculating V and E...\n");
    char ch;
    for(int i=0;i<7;i++) fgets(tmp,sizeof(tmp),file);
    while (fscanf(file," %c",&ch)==1){// Loop until end-of-file (EOF)
        fscanf(file,"%d %d %d", &temp_edges[edge_count].u, &temp_edges[edge_count].v, &temp_edges[edge_count].w);
        max_vertex = getMax(max_vertex,temp_edges[edge_count].u);// Determine the maximum vertex index (N)
        max_vertex = getMax(max_vertex,temp_edges[edge_count].v);
        edge_count++;
        if (edge_count >= temp_capacity) {// If capacity is reached, double the array size (realloc)
            temp_capacity *= 2;
            TempEdge *new_ptr = (TempEdge*)realloc(temp_edges, temp_capacity * sizeof(TempEdge));
            temp_edges = new_ptr;
        }
    }
    if (edge_count < temp_capacity) {
        TempEdge *final_ptr = (TempEdge*)realloc(temp_edges, edge_count * sizeof(TempEdge));
        if (final_ptr) {
             temp_edges = final_ptr;
        }
    }
    
    N = max_vertex;
    M = edge_count;
    *N_ptr = N;
    Graph *G = create_graph(N);//  Allocate and construct adjacency lists
    Graph *G_rev = create_graph(N);
    *G_rev_ptr = G_rev;
    if (!G || !G_rev) {
        free(temp_edges);
        return NULL;
    }
    for (int i = 0; i < M; i++) {// Populate the graphs from the temporary edge array
        int u = temp_edges[i].u;
        int v = temp_edges[i].v;
        int w = temp_edges[i].w;
        graph_add_edge(G, u, v, w);
        graph_add_edge(G_rev, v, u, w);// Add edge to the reverse graph
    }
    free(temp_edges);
    fclose(file);
    return G;
}
int main() {
    int NUM_QUERIES;
    srand(time(NULL)); 
    Graph *G = NULL;//Graph Initialization and Data Reading
    Graph *G_rev = NULL;
    printf("Loading graph...\n");
    printf("0: NY, 1: BAY, 2: COL, 3: FLA, 4: NW, 5: NE, 6: CAL, 7: LKS, 8: E, 9: W, 10: CTR, 11: USA\n");
    int choice;
    printf("City code name:\n");
    scanf("%d", &choice);
    switch (choice){
        case 0: file = fopen("../Map/USA-road-d.NY.gr", "r"); printf("NY\n"); 
            filein=fopen("../Queries/NY_Queries.txt","r"); fileout = fopen("../Results/NY_Answer.txt", "w"); break;
        case 1: file = fopen("../Map/USA-road-d.BAY.gr", "r"); printf("BAY\n");
            filein=fopen("../Queries/BAY_Queries.txt","r"); fileout = fopen("../Results/BAY_Answer.txt", "w"); break;
        case 2: file = fopen("../Map/USA-road-d.COL.gr", "r");printf("COL\n");
            filein=fopen("../Queries/COL_Queries.txt","r"); fileout = fopen("../Results/COL_Answer.txt", "w"); break;
        case 3: file = fopen("../Map/USA-road-d.FLA.gr", "r"); printf("FLA\n");
            filein=fopen("../Queries/FLA_Queries.txt","r"); fileout = fopen("../Results/FLA_Answer.txt", "w"); break;
        case 4: file = fopen("../Map/USA-road-d.NW.gr", "r");printf("NW\n");
            filein=fopen("../Queries/NW_Queries.txt","r"); fileout = fopen("../Results/NW_Answer.txt", "w"); break;
        case 5: file = fopen("../Map/USA-road-d.NE.gr", "r");printf("NE\n");
            filein=fopen("../Queries/NE_Queries.txt","r"); fileout = fopen("../Results/NE_Answer.txt", "w"); break;
        case 6: file = fopen("../Map/USA-road-d.CAL.gr", "r"); printf("CAL\n");
            filein=fopen("../Queries/CAL_Queries.txt","r"); fileout = fopen("../Results/CAL_Answer.txt", "w"); break;
        case 7: file = fopen("../Map/USA-road-d.LKS.gr", "r"); printf("LKS\n");
            filein=fopen("../Queries/LKS_Queries.txt","r"); fileout = fopen("../Results/LKS_Answer.txt", "w"); break;
        case 8: file = fopen("../Map/USA-road-d.E.gr", "r"); printf("E\n");
            filein=fopen("../Queries/E_Queries.txt","r"); fileout = fopen("../Results/E_Answer.txt", "w"); break;
        case 9: file = fopen("../Map/USA-road-d.W.gr", "r"); printf("W\n");
            filein=fopen("../Queries/W_Queries.txt","r"); fileout = fopen("../Results/W_Answer.txt", "w"); break;
        case 10: file = fopen("../Map/USA-road-d.CTR.gr", "r"); printf("CTR\n");
            filein=fopen("../Queries/CTR_Queries.txt","r"); fileout = fopen("../Results/CTR_Answer.txt", "w"); break;
        case 11: file = fopen("../Map/USA-road-d.USA.gr", "r"); printf("USA\n");
            filein=fopen("../Queries/USA_Queries.txt","r"); fileout = fopen("../Results/USA_Answer.txt", "w"); break;
        default: printf("Fuck you!"); return 2;
    }
    clock_t c0 = clock();
    G = initialize_graph_dynamic(&G_rev, &N);// Read all edges dynamically from stdin and initialize graphs
    clock_t c1 = clock();
    double cpu_seconds = (double)(c1 - c0) / CLOCKS_PER_SEC;

    printf("Graph loading time: %.3f seconds\n", cpu_seconds);

    const HeapInterface *interface = &LeftistHeapInterface;
    int total_found = 0;
    fscanf(filein,"%d",&NUM_QUERIES);// Use the correctly maintained num_edges for the report
    printf("\n=== Bidirectional Dijkstra with Persistent Leftist Heap ===\n");
    printf("Graph Fully Constructed: V=%d, E=%d\n", G->num_vertices, M); 
    printf("Query Count: %d\n\n", NUM_QUERIES);
    printf("--- Performance Test (%d random queries) ---\n", NUM_QUERIES);
    start_timer();
    for (int i = 0; i < NUM_QUERIES; i++) {//Performance Test
        int s,t;
        fscanf(filein,"%d %d",&s,&t);
        int dist = bidirectional_dijkstra(G, G_rev, s, t, interface);// Run the optimized shortest path algorithm
        fprintf(fileout,"%d\n",dist);
        if (dist != INF) {
            total_found++;
        }
    }
    double cpu_time_used = stop_timer();

    printf("Total queries attempted: %d\n", NUM_QUERIES);
    printf("Total paths found (not INF): %d\n", total_found);
    printf("Total execution time: %.6f seconds\n", cpu_time_used);
    printf("Average time per query: %.6f milliseconds\n", (cpu_time_used * 1000.0) / NUM_QUERIES);
    graph_free(G);//clean up
    graph_free(G_rev);
    
    return 0;
}