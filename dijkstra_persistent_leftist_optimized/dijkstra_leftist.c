#include "dijkstra_leftist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int getMin(int a,int b){
    return a<b?a:b;
}
Graph* create_graph(int N) {//Creates a new graph structure.
    Graph *G = (Graph*)malloc(sizeof(Graph));
    G->num_vertices = N;// Allocate N+1 pointers for 1-based indexing(from 1 to N)
    G->adj_list = (Edge**)calloc(N + 1, sizeof(Edge*));
    if (!G->adj_list) {//error:Allocation failed
        free(G);
        return NULL;
    }
    return G;
}
void graph_add_edge(Graph *G, int u, int v, int w) {//Adds an edge from u to v with weight w to graph G
    if (u < 1 || u > G->num_vertices || v < 1 || v > G->num_vertices) return;
    Edge *new_edge = (Edge*)malloc(sizeof(Edge));//Creating a new edge node
    new_edge->target_vertex = v;
    new_edge->weight = w;// Insert new edge at the head of the adjacency list (head-insertion)
    new_edge->next = G->adj_list[u];
    G->adj_list[u] = new_edge;//// Increment total edge count
}
void graph_free(Graph *G) {//Frees all memory occupied by graph G.
    if (!G) return;
    for (int i = 1; i <= G->num_vertices; i++) {
        Edge *current = G->adj_list[i];//Traverse the entire graph
        while (current != NULL) {
            Edge *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(G->adj_list);
    free(G);
}
int bidirectional_dijkstra(const Graph *G, const Graph *G_rev, //Implements Bidirectional Dijkstra using the provided HeapInterface.
                           int s, int t, const HeapInterface *interface) {
    if (s == t) return 0;
    int N = G->num_vertices;
    int *dist_f = (int*)malloc((N + 1) * sizeof(int));// Allocate memory for distance and visited arrays (1-based)
    int *dist_b = (int*)malloc((N + 1) * sizeof(int));
    bool *visited_f = (bool*)calloc(N + 1, sizeof(bool)),*visited_b = (bool*)calloc(N + 1, sizeof(bool)); 
    for (int i = 1; i <= N; i++)  dist_f[i] = dist_b[i] = INF;//initialize
    dist_f[s] = 0;dist_b[t] = 0;
    AbstractHeap *pq_f = interface->create(),*pq_b = interface->create();
    interface->insert(pq_f, 0, s);interface->insert(pq_b, 0, t);
    int min_dist = INF; // Best path found so far
    DijkstraItem item_f,item_b;
    while (!interface->is_empty(pq_f) && !interface->is_empty(pq_b)) {//alternate forward and backward searches
        // Forward Search Step 
        if (pq_f->size + pq_b->size == 0) break; 
        if (pq_f->size <= pq_b->size) {//Prioritize the smaller heap
            interface->pop(pq_f, &item_f); // Modify pq_f in place
            int u = item_f.vertex;
            int d_u = item_f.distance;
            if (d_u > dist_f[u]) continue; // lazy deletion:ignore stale distance
            visited_f[u] = true; //tag visited
            if (visited_b[u]) // Check for meeting point
                min_dist= getMin(min_dist,d_u+dist_b[u]);
            if (d_u + interface->min_key(pq_b) >= min_dist)break;//Pruning condition: If current best + smallest possible distance from the other side >=min_dist , stop
            Edge *edge = G->adj_list[u];// Relax neighbors of u
            while (edge != NULL) {
                int v = edge->target_vertex,weight = edge->weight;
                if (d_u + weight<dist_f[v]) {
                    dist_f[v] = d_u + weight;
                    interface->insert(pq_f, dist_f[v], v);// Lazy Insertion:Insert the improved path distance 
                }
                edge = edge->next;
            }
        }
        else {//  Backward Search Step (Symmetric)
            interface->pop(pq_b, &item_b); //Modify pq_b in place
            int u = item_b.vertex,d_u = item_b.distance;
            if (d_u > dist_b[u]) continue;//skip the case won't be the greatest
            visited_b[u] = true; 
            if (visited_f[u]) min_dist = getMin(min_dist,d_u+dist_f[u]);
            if (d_u + interface->min_key(pq_f) >= min_dist) break;
            Edge *edge = G_rev->adj_list[u];//relax neighbors of u using the reverse graph G_rev
            while (edge != NULL) {
                int v = edge->target_vertex,weight = edge->weight;
                if (d_u + weight < dist_b[v]) {
                    dist_b[v] = d_u + weight;
                    interface->insert(pq_b, dist_b[v], v); //Lazy Insertion
                }
                edge = edge->next;
            }
        }
    }
    interface->free_heap(pq_f);//Clean up memory 
    interface->free_heap(pq_b); 
    free(dist_f);free(dist_b);free(visited_f); free(visited_b);
    return min_dist;
}