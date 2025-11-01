#include "dijkstra_leftist.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
static PLHNode* plh_create_node(int key, int vertex) {//Node Creation
    PLHNode* n = (PLHNode*)malloc(sizeof(PLHNode));
    n->key = key;n->vertex = vertex;
    n->npl = 0; // Null Path Length (NPL) is 0 for a single node
    n->left = NULL;n->right = NULL;
    return n;
}
/**
 * @return the root of the new heap
 */
static PLHNode* plh_meld_recursive(PLHNode* H1, PLHNode* H2) {//Mutable meld
    if (!H1) return H2;if (!H2) return H1;
    if (H2->key < H1->key){ // Ensure H1's key is the minimum (Min-Heap property)
        PLHNode *temp = H1;
        H1 = H2;
        H2 = temp;
    }
    H1->right = plh_meld_recursive(H1->right, H2);// Recursively Meld the right subtree of H1 with H2
    if (!H1->left || (H1->right && H1->left->npl < H1->right->npl)){// Enforce the Leftist property (NPL: NPL of left >= NPL of right)
        PLHNode* temp = H1->left;
        H1->left = H1->right;
        H1->right = temp;
    }
    H1->npl = (H1->right)?H1->right->npl + 1:0;//Update NPL (NPL of node = NPL of right child + 1)
    return H1;
}
AbstractHeap* plh_create() {//initialize
    AbstractHeap *H = (AbstractHeap*)malloc(sizeof(AbstractHeap));
    H->root = NULL;H->size = 0;
    return H;
}
bool plh_is_empty(AbstractHeap *H) {//1 for empty
    return H->root == NULL;
}
int plh_min_key(AbstractHeap *H) {// Returns the key of the minimum element (root)
    if (H->root) return H->root->key;
    return INF;
}
void plh_insert(AbstractHeap *H, int distance, int vertex) {
    PLHNode* node_new = plh_create_node(distance, vertex);//Create a new node for the item
    H->root = plh_meld_recursive(H->root, node_new);//Meld the current heap root with the new node, and update the root pointer
    H->size++;
}
void plh_pop(AbstractHeap *H, DijkstraItem *out_item) {
    if (!H->root) {
        out_item->distance = INF; out_item->vertex = -1;
        return;
    }
    out_item->distance = H->root->key;//Extract the result from the root
    out_item->vertex = H->root->vertex;
    PLHNode* old_root = H->root;//Save the old root for freeing and Meld its children
    H->root = plh_meld_recursive(H->root->left, H->root->right);
    old_root->left = old_root->right = NULL; // Clean up the old root node (memory management) and free
    free(old_root); 
    H->size--;
}
static void plh_free_recursive(PLHNode* node) {//Recursively free all nodes
    if (!node) return;
    plh_free_recursive(node->left);
    plh_free_recursive(node->right);
    free(node);
}

void plh_free_heap(AbstractHeap *H) {
    if (!H) return;
    plh_free_recursive(H->root);// Free the content (all heap nodes)
    free(H); // Free the AbstractHeap structure
}
const HeapInterface LeftistHeapInterface = {//"class"
    .create = plh_create,
    .is_empty = plh_is_empty,
    .insert = plh_insert,
    .pop = plh_pop,
    .min_key = plh_min_key,
    .free_heap = plh_free_heap
};