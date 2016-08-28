/*Name: Michael Roytman*/
/*Brandeis Map - Tour .h file*/

void KruskalsAlgorithm();
void PrimsAlgorithm();

/*performs preorder on a tree represented by an adjacency_list, starting with the node as the root
and prints reverse edge to produce a simple tour*/
void Preorder(struct adjacency_list* tree, struct list_node* node) {
    while (node != NULL) {
        PrintLeg(node->edge_index); /*prints edge*/
        Preorder(tree, tree->array[node->vertex_index].head);
        PrintLeg(ReverseEdge(node->edge_index)); /*prints reverse edge upon returning*/
        node = node->next;
    }
}

/*performs levelorder on a tree represented by an adjacency_list, starting with the node
represented by vertex_index; used to display a MST*/
void LevelOrder(struct adjacency_list* tree, int vertex_index) {
   
    /*acts as a queue for levelorder*/
    int queue[nV];

    /*keeps track of marked vertices*/
    int marked[nV];

    /*initialize all vertices to unmarked*/
    int i;
    for (i = 0; i < nV; i++) {
        marked[i] = 0;
    }

    int front = -1;
    int rear = -1;

    /*add root to the queue*/
    queue[rear+1] = vertex_index;
    rear++;

    /*while queue is not empty*/
    while (front != rear) {
        front++;
        int current = queue[front];
        printf("%d\n", current);

        /*add all of current node's children to the back of the queue*/
        struct list_node* temp = tree->array[current].head;

        while (temp != NULL) {
            /*if child is unmarked, add it to the queue*/
            if (marked[temp->vertex_index] == 0) {
                queue[rear+1] = temp->vertex_index;
                rear++;
                marked[temp->vertex_index] = 1;
            }
            temp = temp->next;
        }
    }
}

/*performs a tour starting from the Begin vertex by calculating Prim's Minimum Spanning Tree (MST)*/
void Tour () {
    PrimsAlgorithm();
    /*KruskalsAlgorithm();*/
}

/***************************************************************************************/
/*PRIM'S ALGORITHM                                                                     */
/***************************************************************************************/

/*performs a tour starting from the Begin vertex by calculating Prim's Minimum Spanning Tree (MST) and calling 
preorder on the resulting tree, creating a simple tour*/
void PrimsAlgorithm() {
    struct adjacency_list* graph = initGraph(nV);

    int i;

    struct adjacency_list* tree = initGraph(nV);

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

    struct min_heap* heap = initMinHeap(nV);

    /*mark the begin*/
    marked[Begin] = 1;

    int vertices_in_set = 1;

    /*add all incident edges to Begin to the heap*/
    struct list_node* temp = graph->array[Begin].head;

    while (temp != NULL) {
        insert(heap, temp->edge_index, EdgeCost(temp->edge_index)); 
        temp = temp->next;
    }

    /*while there are fewer than nV vertices marked and the heap is not empty*/
    while (vertices_in_set < nV && isEmpty(heap) == 0) {
        struct heap_node* node = deleteMin(heap); /*extract the node with the minimum cost edge*/

        int edge_index = node -> vertex_index; /*refers to the edge_index, despite the name*/

        /*if the end point of the edge is unmarked*/
        if (marked[Eend[edge_index]] == 0) {
            addEdge(tree, Estart[edge_index], Eend[edge_index], edge_index); /*add the edge to the MST*/
            marked[Eend[edge_index]] = 1;
            vertices_in_set++;

            /*insert all incident edges to node to the heap*/
            temp = graph->array[Eend[edge_index]].head;

            while (temp != NULL) {
                if (marked[temp->vertex_index] == 0) {
                    insert(heap, temp->edge_index, EdgeCost(temp->edge_index));
                }
                temp = temp->next;
            }
        }
    }
    
    /*NOTE: I tried to implement Kruskal's algorithm, but was not able to traverse the resulting MTS correctly in preorder
    because the graph is techinically directed, and my code ended up adding the wrong direction arrows to the MST.*/
    /*KruskalsAlgorithm(graph);*/
    
    /*traverse the tree in preorder, creating a tour, by printing the reverse edge upon returning;
    uncomment this to print out the tour*/
    Preorder(tree, tree->array[Begin].head);

    /*traverse the tree in level order; 
    uncomment this to print out a level order of the MST*/
    /*LevelOrder(tree, Begin);*/
}

/***************************************************************************************/
/*KRUSKAL'S ALGORITHM/UNION FIND DATA STRUCTURE                                        */
/***************************************************************************************/

/*struct representing a set of the union-find data structure*/
struct subset {
    int parent;
    int size;
};

/*initializes union find data structure*/
struct subset* initUnionFind(int numVertices) {
    struct subset* set_of_subsets = (struct subset*) malloc(sizeof(struct subset) * numVertices); /*allocate memory*/
    
    int i;

    for (i = 0; i < numVertices; i++)
    {
        /*originally, each sets parent is itself*/
        set_of_subsets[i].parent = i;
        /*each set is originally of size 1*/
        set_of_subsets[i].size = 1;
    }

    return set_of_subsets;
}

/*find function that finds the parent of i and sets all encountered subset's parent's to the root (path compression)*/
int find(struct subset* set_of_subsets, int i) {
    
    /*find root and make root as parent of i (this is the path compression)*/ 
    if (set_of_subsets[i].parent != i) {
        set_of_subsets[i].parent = find(set_of_subsets, set_of_subsets[i].parent);
    }
 
    return set_of_subsets[i].parent;
}
 
/*union function that unions together two subsets (had to rename to unionSet from union because compiler did not like name union)*/
void unionSet(struct subset* set_of_subsets, int x, int y) {
    int x_parent = find(set_of_subsets, x); /*parent of x*/
    int y_parent = find(set_of_subsets, y); /*parent of y*/
 
    if (x_parent == y_parent) {
        return;
    }

    /*union the smaller size tree to the larger size tree*/
    if (set_of_subsets[x_parent].size < set_of_subsets[y_parent].size) {
        set_of_subsets[x_parent].parent = y_parent;
        set_of_subsets[y_parent].size += set_of_subsets[x_parent].size; /*update size of larger tree*/
    }
    else if (set_of_subsets[x_parent].size > set_of_subsets[y_parent].size) {
        set_of_subsets[y_parent].parent = x_parent;
        set_of_subsets[x_parent].size += set_of_subsets[y_parent].size; /*update size of larger tree*/
    }
 
    /*if sizes are equal, choose one set consistently and union the other with it*/
    else
    {
        set_of_subsets[y_parent].parent = x_parent;
        set_of_subsets[x_parent].size += set_of_subsets[y_parent].size; /*update size of the tree we joined to*/

    }
}

/*performs Kruskal's algorithm to find the MST*/
void KruskalsAlgorithm() {

    struct adjacency_list* graph = initGraph(nV);

    int i;

    /*builds the graph using the edges*/
    for (i = MinEdge; i < nE; i++) {
        addEdge(graph, Estart[i], Eend[i], Eindex[i]);
    }

    struct adjacency_list* tree = initGraph(nV);

    struct subset* set_of_subsets = initUnionFind(nV);
    int set_count = nV-MinVertex;

    struct min_heap* heap = initMinHeap(nV);

    /*adds all edges in the graph to the heap*/
    for (i = MinEdge; i < nE; i++) {
            insert(heap, Eindex[i], EdgeCost(Eindex[i]));
    }

    /*while there is at least one set in the union-find data structure and the heap is not empty*/
    while (set_count > 1 && isEmpty(heap) == 0) {
        int edge_index = deleteMin(heap)->vertex_index; /*extract the node with the minimum cost edge; refers to edge_index, despite the name */

        int start_index = Estart[edge_index]; /*start of edge*/
        int end_index = Eend[edge_index]; /*end of edge*/

        int start_find = find(set_of_subsets, start_index);
        int end_find = find(set_of_subsets, end_index);

        /*if they are in different trees*/
        if (start_find != end_find) {
            addEdge(tree, start_index, end_index, edge_index); /*add edge to the MST*/

            unionSet(set_of_subsets, start_index, end_index); /*union them together*/
            set_count--; /*one fewer union-find data structures since they were unioned*/
        }
    }
}
