/*
 * graph.c
 * Work with graph
 * Theme: Hamiltonian path and cycle in graph 
 * Implementation: Oleksii Fedorchenko, Dmitrii Ivanushkin, Zlata Valakhanovich, December 2022
 */
#include "../lib/graph.h"

// Init Graph and allocate memory
void initGraph (TGraph *graph, int V) {
    graph->V = V;
    graph->array = (TEdge**)malloc(V * sizeof(TEdge*));
    if(graph->array ==NULL)
       exit(1);

    for (int i = 0; i < V; i++)
    {
        graph->array[i] = (TEdge*)malloc(V * sizeof(TEdge));
        if(graph->array[i] == NULL)
            exit(1);
        for (int j = 0; j < V; j++)
            graph->array[i][j].data = 0;
    }
}

// Init path and allocate memory
void initPath (TPath *path, int V) {
    path->V = V;
    path->array = (TEdge*)malloc(V * sizeof(TEdge));
    if(path->array ==NULL)
       exit(1);

    for (int i = 0; i < V; i++)
        path->array[i].data = 0;
}

// Free allocated memory
void freeGraph (TGraph *graph, int V){
    for (int i = 0; i < V; i++)
        free(graph->array[i]);
    free(graph->array);
}

// Free allocated memory
void freePath (TPath *path){
    free(path->array);
} 

// Insert edges between vertices
void insertConn (TGraph *graph, int v, int e) { 
    graph->array[v][e].data = 1; 
}

// Print to stdout the matrix representing graph
void printGraph (TGraph *graph, int V, int cnt) {
    printf("[graph %d]\n", cnt);
    for (int i = 0; i < V; i++)
    {
        for (int j = 0; j < V; j++)
        {
            if (j < V-1)
                printf("%d-", graph->array[i][j].data); 
            else if (j == V-1)
                printf("%d", graph->array[i][j].data);
        }
        printf("\n");
    }
}

// The backtracking algorithm for searching a Hamiltonian path
bool algo (TGraph *graph, TPath *path, int curr_node, int V) {
    if (curr_node + 1 == V) 
    {
        if (graph->array[path->array[curr_node].data][path->array[0].data].data == 1)
            return true;
        else
            return false;
    }
    
    for (int i = 1; i < V; i++)
    {
        if ((graph->array[path->array[curr_node].data][i].data) == 1)
        {
            for (int j = 0; j < curr_node+1; j++)
                if (path->array[j].data == i)
                    return false;

            path->array[curr_node + 1].data = i;

            if (algo (graph, path, curr_node+1, V))
                return true;
            
            path->array[curr_node + 1].data = 0;
        }
    }
    
    return false;
}

// Print result of search with (not)found path
bool cycle (TGraph *graph, TPath *path, int V) {
    initPath(path, V);

    int curr_node = 0;

    if (algo(graph, path, curr_node, V))
    {
        printf("Result: It is a Hamilton graph\n");
        printf("Hamilton cycle is ");
        for (int i = 0; i < V; i++)
            printf("%d-", (*path).array[i].data);
        printf("%d\n", (*path).array[0].data);
        printf("\n");
        return true;
    }
    printf("Result: It is not a Hamilton graph\n");
    printf("\n");
 
    return false;
}