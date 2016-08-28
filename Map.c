/*Name: Michael Roytman*/
/*Brandeis Map*/

/*Standard system stuff*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

/*Path to the map folder; may need to change this.*/
#include "MapPATH.h"

#include "Map.h" /*Map data parameters, structures, and functions.*/

#include "MapData.h"   /*Functions to input the map data.*/
#include "MapInput.h"  /*Functions to get user input.*/
#include "MapOutput.h" /*Functions to produce output.*/

/*Use this to get the time to travel along an edge.*/
#define EdgeCost(X) ( (TimeFlag) ? Time(X) : Elength[X] )

/*Use this to print a leg of a route or tour.*/
void PrintLeg(int edge);


/***************************************************************************************/
/*GRAPH ADJACENCY LIST DATA STRUCTURE                                                  */
/***************************************************************************************/
/*a struct representing a node in a linked list*/
struct list_node {
    struct list_node* next; /*pointer to next list_node; recursive definition of a list*/
    
    int vertex_index; /*vertex_index representing the end-point of the edge*/
    int edge_index; /*index of the edge between the vertex in the array and the list_node*/
};

/*a struct representing a linked list*/
struct linked_list {
    struct list_node *head; /*a pointer to the head of the linked list, which is a list_node*/
};

/*a struct representing an adjacency list*/
struct adjacency_list {
    struct linked_list* array; /*an adjacency_list is an array of linked_lists*/
    int num_vertices;
};


/*initialize a list node*/
struct list_node* initListNode(int vertex_index, int edge_index) {
    struct list_node* node = (struct list_node*) malloc(sizeof(struct list_node)); /*allocate memory*/
    
    node -> vertex_index = vertex_index;
    node -> edge_index = edge_index;
    node -> next = NULL; /*sets next pointer to null*/

    return node;
}

/*initialize an adjacency list*/
struct adjacency_list* initGraph(int num_vertices) {
    struct adjacency_list* graph = (struct adjacency_list*) malloc(sizeof(struct adjacency_list)); /*allocate memory*/
    
    graph -> num_vertices = num_vertices;
    graph -> array = (struct linked_list*) malloc(num_vertices * sizeof(struct linked_list));

    /*sets the head of the linked list in each index of the array to null*/
    int i;
    for (i = 0; i < num_vertices; i++) {
        graph->array[i].head = NULL;
    }

    return graph;
}

/*add an edge to the adjacency_list*/
void addEdge(struct adjacency_list* graph, int start_vertex, int end_vertex, int edge_index) {

    /*initialize a new list node*/
    struct list_node* new_node = initListNode(end_vertex, edge_index);

    /*inserts new_node at the head of the linked list pointed to at array[start_index]*/
    new_node->next = graph->array[start_vertex].head;
    graph->array[start_vertex].head = new_node;
 
}

/***************************************************************************************/
/*HEAP DATA STRUCTURE                                                                  */
/***************************************************************************************/
/*a struct representing a node in a heap*/
struct heap_node {
    int vertex_index; /*index of the vertex in the graph*/
    int distance; /*distance/priority on which we order the min_heap*/
};

/*a struct representing a min_heap*/
struct min_heap {
    struct heap_node** array; /*an array of pointers to heap_nodes*/
    int size; /*size of the heap*/
    int capacity; /*capacity of the heap*/
};


/*intialize a heap node*/
struct heap_node* initHeapNode(int vertex_index, int distance) {
    struct heap_node* node = (struct heap_node*)(malloc(sizeof(struct heap_node))); /*allocate memory*/

    node->vertex_index = vertex_index;
    node->distance = distance;
    
    return node;
}

/*initialize a min heap*/
struct min_heap* initMinHeap(int capacity) {
    struct min_heap* heap = (struct min_heap*)(malloc(sizeof(struct min_heap))); /*allocate memory*/

    heap -> size = -1; /*heap is empty*/
    heap -> capacity = capacity;
    heap -> array = (struct heap_node**)malloc(sizeof(struct heap_node*)*(capacity)); /*allocate memory for the array*/

    return heap;
}

/*swap two heap_nodes in a min_heap*/
void swap(struct heap_node* element, struct heap_node* to_swap) {
    struct heap_node temp = *to_swap;

    *to_swap = *element;
    *element = temp;
}

/*returns the minimum between x and y; returns y if x=y*/
int minimum(int x, int y) {
    if (x < y) {
        return x;
    }
    else {
        return y;
    }
}

/*heapifies down the min_heap at index element_index*/
void heapifyDown(struct min_heap *heap, int element_index) {
    int minimum = element_index;
    int left = 2*element_index+1; /*left child of element_index*/
    int right = 2*element_index+2; /*right child of element_index*/

    /*sets minimum to which ever index is smaller, left or right*/
    if (left < heap-> size && heap->array[left]->distance < heap->array[minimum]->distance) {
        minimum = left;
    }

    if (right < heap-> size && heap->array[right]->distance < heap->array[minimum]->distance) {
        minimum = right;
    }

    /*if the minimum is the not the element_index, swap the elememnts and heapify the minimum down*/
    if (minimum != element_index) {
        swap(heap->array[minimum], heap->array[element_index]);
        heapifyDown(heap, minimum);
    }
}

/*heapifies up the min_heap at index element_index*/
void heapifyUp(struct min_heap *heap, int element_index) {
    /*if the element is not the root*/
    if (element_index > 0) {

        int parent = (element_index-1)/2; /*parent of element_index*/

        /*if parent is in the bounds of the heap and the parent's distance > element's distance*/
        if (parent >= 0 && heap->array[parent]->distance > heap->array[element_index]->distance) {

            /*swap the elements and heapify the parent up*/
            swap(heap->array[element_index], heap->array[parent]);
            heapifyUp(heap, parent);
        }
    }
}

/*inserts a vertex_index, distance pair into the min heap*/
void insert(struct min_heap *heap, int vertex_index, int distance) {
    
    struct heap_node* node = initHeapNode(vertex_index, distance); /*initializes heap node*/

    heap -> size++;
    heap -> array[heap->size] = node; /*adds node to the rightmost-bottom leaf of the heap*/
    heapifyUp(heap, heap->size); /*heapifies the new node up*/
}

/*checks whether the heap is empty*/
int isEmpty(struct min_heap *heap) {
    if (heap->size == -1) {
        return 1;
    }
    return 0;
}

/*deletes the minimum from the min heap and heapifies down to maintain heap propery*/
struct heap_node* deleteMin(struct min_heap *heap) {
    
    /*if the heap is not empty*/
    if (isEmpty(heap) == 0) {

        /*minimum element in a min heap is the root*/
        struct heap_node* minimum = heap->array[0];

        /*swaps rightmost-bottom leaf of heap for the old root*/
        heap->array[0] = heap->array[heap->size];

        /*decrements size*/
        heap->size--;

        /*heapifies down the new root*/
        heapifyDown(heap, 0);

        /*returns minimum*/
        return minimum;
    }
    /*if heap is empty, then no minimum*/
    else {
        return NULL;
    }
}

/*finds the vertex described by vertex_index in the min heap*/
int findVertex(struct min_heap *heap, int vertex_index) {
    int i;

    /*traverses the heap array, looking for vertex_index*/
    for (i = 0; i <= heap->size; i++) {
        if (heap->array[i]->vertex_index == vertex_index) {
            return i;
        }
    }
    return -1;
}

/*decreases the key of the node represented by vertex_index to new_value*/
void decreaseKey(struct min_heap *heap, int vertex_index, int new_value) {
    /*finds the vertex in the heap*/
    int heap_index = findVertex(heap, vertex_index);

    /*sets the new distance value*/
    heap->array[heap_index]->distance = new_value;

    /*heapifies the element with the new key up*/
    heapifyUp(heap, heap_index);
}

/*finds the edge_index of the edge between start_index and end_index by looking in the adjacency list*/
int findEdgeIndex(struct adjacency_list *graph, int start_index, int end_index) {
    /*the linked list pointed to by start_index*/
    struct list_node *temp = graph->array[start_index].head;

    /*traverses the linked list pointed to by start_index, looking for end_index*/
    while (temp != NULL) {
       if (temp->vertex_index == end_index) {
            /*return the edge index in the list node containing end_index*/
            return temp->edge_index;
       }
       temp = temp->next;
    }
    return -1; /*not found*/
}

/***************************************************************************************/
/*Dijkstra Algorithm                                                                   */
/*DijkstraFlag=1 to supress output when Dijkstra is called from tour code.)            */
/***************************************************************************************/

void Dijkstra(int DijkstraFlag) {
    
    struct adjacency_list* graph = initGraph(nV);

    int i;

    /*builds the graph using the edges*/
    for (i = MinEdge; i < nE; i++) {
        addEdge(graph, Estart[i], Eend[i], Eindex[i]);
    }

    /*array to keep track whether vertices are marked (1) or unmarked (0)*/
    int marked[nV];

    /*initialize all vertices to unmarked (0)*/
    for (i = 0; i < nV; i++) {
        marked[i] = 0;
    }

    /*array to keep track of the minimum distance of each vertex from the source*/
    int distance[nV];

    /*initialize distance of all vertices to infinity (HUGE_VAL)*/
    for (i = 0; i < nV; i++) {
        distance[i] = HUGE_VAL;
    }

    /*initialize distance of source vertex to 0*/
    distance[Begin] = 0;

    /*initialize min_heap of size nV*/
    struct min_heap* heap = initMinHeap(nV);

    
    /*builds the heap with all the vertices, using their associated distances (HUGE_VAL or 0 at this point) as the keys*/
    for (i = MinVertex; i < nV; i++) {
       insert(heap, i, distance[i]);     
    }

    /*array to keep track of the predecessor of eahc vertex; predecessor[i] will contain the edge index that connects vertex i
    to its predecessor on the minimum path from the source to vertex i*/
    int predecessor[nV];
    
    /*initializes all entres in predecessor array to -1*/
    for (i = 0; i < nV; i++) {
        predecessor[i] = -1;
    }    

    /*while the heap is not empty*/
    while (isEmpty(heap) == 0) {
        /*delete the minimum element at the top of the min_heap; greedy aspect of the algorithm*/
        struct heap_node* node = deleteMin(heap);

        /*check whether this vertex has already been visited/marked by looking at marked array*/
        if (marked[node->vertex_index] == 0) {
           
            /*mark the vertex in the marked array*/
            marked[node->vertex_index] = 1;

            struct list_node* temp;

            /*gets all the vertices incident to node*/
            temp = graph->array[node->vertex_index].head;

            /*for all vertices incident to node*/
            while (temp != NULL) {
                /*if the vertex is unmarked, see if we can relax the edge*/
               if (marked[temp->vertex_index] == 0) {
                    int old_distance = distance[temp->vertex_index];
                    int new_distance = distance[node->vertex_index] + EdgeCost(temp->edge_index);

                    /*if we can relax the edge*/
                    if (new_distance < old_distance) {
                        distance[temp->vertex_index] = new_distance;

                        decreaseKey(heap, temp->vertex_index, new_distance); /*decreases the key in the min_heap*/
                        predecessor[temp->vertex_index] = temp->edge_index; /*changes predecessor to be able to reconstruct minimum path*/
                    }                  
                }
                temp = temp->next;
            }
        }
    }

    /*an array containing the edges to print in reverse order*/
    int edgesToPrint[nV];

    int k = 0; /*index for edgesToPrint*/

    int j = Finish;

    /*backtracking while loop; loop through the array, traversing the path from Begin to Finish backwards,
    adding the edges to edges to print*/
    while (j != Begin) {
        int edge = predecessor[j];
        edgesToPrint[k] = edge;
        k = k+1;
        j = Estart[edge];
    } 

    /*traverses edgesToPrint backwards to print out the path from Begin to Finish in the correct order*/
    for (i = k-1; i >= 0; i--) {
        PrintLeg(edgesToPrint[i]);
    }
}

/***************************************************************************************/
/*CAMPUS TOUR                                                                          */
/***************************************************************************************/
#include "Tour.h"


/***************************************************************************************/
/*MAIN PROGRAM                                                                         */
/***************************************************************************************/
int main() {
GetVertices();
GetEdges();
while (GetRequest()) {RouteOpen(); TourFlag ? Tour() : Dijkstra(0); RouteClose();}
return(0);

printf("Help");
}
