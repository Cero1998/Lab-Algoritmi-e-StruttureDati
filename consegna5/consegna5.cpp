#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;


typedef struct node
{
    int val; /// prossimo nodo
    float w; /// peso dell'arco
    int rappresentante = NULL;
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list
{
    node *head;
} list_t;

int n_nodi = 8;
list_t **graph; // array di puntatori a liste, posso dichiarare la grandezza a runtime, e poi farlo puntare in altre zone di memoria.




void create_edge(int u, int v, int weight) //creo arco BI-direzionale. Se dico che 0 è connesso a 1 di conseguenza 1 è connesso a 0
{
    /// inserisco nella lista del nodo U il nodo V con peso W   ->  U - V
    node_t *new_node = new node_t;
    new_node->val = v;
    new_node->w = weight;
    new_node->next = graph[u]->head;
    graph[u]->head = new_node;

    /// inserisco nella lista del nodo V il nodo U con peso W   -> V - U
    new_node = new node_t;
    new_node->val = u;
    new_node->w = weight;
    new_node->next = graph[v]->head;
    graph[v]->head = new_node;
}


int main()
{
    graph = new list_t *[n_nodi];
    for(int i =0; i<n_nodi; i++)
    {
        graph[i] = new list;
    }
    //list_insert_front(graph[0],2,1);
    create_edge(0,1,1);
    create_edge(0,7,10);
    create_edge(0,2,4);
    create_edge(1,2,5);
    create_edge(1,4,8);
    create_edge(2,3,3);
    create_edge(2,4,6);
    create_edge(3,5,7);
    create_edge(4,5,9);
    create_edge(5,6,2);



    return 0;
}