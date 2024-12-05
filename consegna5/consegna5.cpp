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
    bool connected;
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list
{
    int rappresentante;
    node *head;
} list_t;

typedef struct edge
{
    int u;
    int v;
    float w;
    bool connected;

} edge_t;

int n_nodi = 8;
int max_edge = 10;
int iterator_edge=0;
list_t **graph; // array di puntatori a liste, posso dichiarare la grandezza a runtime, e poi farlo puntare in altre zone di memoria. 
edge_t **edges;


void create_edge(int u, int v, float weight) //creo arco BI-direzionale. Se dico che 0 è connesso a 1 di conseguenza 1 è connesso a 0
{
    /// inserisco nella lista del nodo U il nodo V con peso W   ->  U - V
    node_t *new_node = new node_t;
    new_node->val = v;
    new_node->w = weight;
    new_node->connected = false;
    new_node->next = graph[u]->head;
    graph[u]->head = new_node;
    graph[u]->rappresentante = -999;

    /// inserisco nella lista del nodo V il nodo U con peso W   -> V - U
    new_node = new node_t;
    new_node->val = u;
    new_node->w = weight;
    new_node->connected = false;
    new_node->next = graph[v]->head;
    graph[v]->head = new_node;
    graph[v]->rappresentante = -999;

    edges[iterator_edge] = new edge{u,v,weight};
    iterator_edge++;
}

void insertion_sort(edge_t** edges, int max_edge)
{
    for(int i = 1; i < max_edge; i++) { 
        int key = edges[i]->w; 
        edge* tempEdge = edges[i];
        int j = i - 1; 
        while((edges[j]->w > key) && (j >= 0)) { 
            edges[j + 1] = edges[j]; 
            j--;
        } 
        edges[j + 1] = tempEdge; 
    }
}

void kruskal()
{
    for(int i = 0; i < max_edge; i++) //ciclo principale, prendo gli archi dal primo all'ultimo in ordine crescente
    {
        edge* min_edge = edges[i];
        if(graph[min_edge->u]->rappresentante == -999 && graph[min_edge->v]->rappresentante == -999)
        {
                graph[min_edge->u]->rappresentante = min_edge->u;
                graph[min_edge->v]->rappresentante = min_edge->u;

                node_t* elem_u = graph[min_edge->u]->head;
                node_t* elem_v = graph[min_edge->v]->head;
                while(elem_u != NULL)
                {
                    if(elem_u->val == min_edge->v)
                    {
                        elem_u->connected=true;
                        break;
                    }
                    elem_u = elem_u->next;
                }
                while (elem_v != NULL)
                {
                    if(elem_v->val == min_edge->u)
                    {
                        elem_v->connected=true;
                        break;
                    }
                    elem_v = elem_v->next;
                }
                

                cout<<"Nodi: "<<min_edge->u<<" "<<min_edge->v<<" con peso: "<<min_edge->w<<endl;
        }
        else if(graph[min_edge->u]->rappresentante != -999 && graph[min_edge->v]->rappresentante == -999)
        {
                graph[min_edge->v]->rappresentante = graph[min_edge->u]->rappresentante;

                node_t* elem_u = graph[min_edge->u]->head;
                node_t* elem_v = graph[min_edge->v]->head;
                while(elem_u != NULL)
                {
                    if(elem_u->val == min_edge->v)
                    {
                        elem_u->connected=true;
                        break;
                    }
                    elem_u = elem_u->next;
                }
                while (elem_v != NULL)
                {
                    if(elem_v->val == min_edge->u)
                    {
                        elem_v->connected=true;
                        break;
                    }
                    elem_v = elem_v->next;
                }

                cout<<"Nodi: "<<min_edge->u<<" "<<min_edge->v<<" con peso: "<<min_edge->w<<endl;
        }
        else if(graph[min_edge->u]->rappresentante == -999 && graph[min_edge->v]->rappresentante != -999)
        {
                graph[min_edge->u]->rappresentante = graph[min_edge->v]->rappresentante;

                node_t* elem_u = graph[min_edge->u]->head;
                node_t* elem_v = graph[min_edge->v]->head;
                while(elem_u != NULL)
                {
                    if(elem_u->val == min_edge->v)
                    {
                        elem_u->connected=true;
                        break;
                    }
                    elem_u = elem_u->next;
                }
                while (elem_v != NULL)
                {
                    if(elem_v->val == min_edge->u)
                    {
                        elem_v->connected=true;
                        break;
                    }
                    elem_v = elem_v->next;
                }

                cout<<"Nodi: "<<min_edge->u<<" "<<min_edge->v<<" con peso: "<<min_edge->w<<endl;
        }
        else
        {
            if(graph[min_edge->u]->rappresentante == graph[min_edge->v]->rappresentante)
            {
                cout<<"Nodi: "<<min_edge->u<<" "<<min_edge->v<<" CICLO TROVATO "<<endl;
            }
            else
            {
                node_t* elem = graph[min_edge->v]->head;
                bool cycle = false;

                while(elem != NULL)
                {
                    if(elem->val == min_edge->u)
                    {
                        elem = elem->next;
                        continue;
                    }
                    if(graph[elem->val]->rappresentante == -999)
                    {
                        elem = elem->next;
                        continue; //sono in un nodo ancora scollegato
                    }
                    if(graph[elem->val]->rappresentante == graph[min_edge->u]->rappresentante && elem->connected == true)
                    {
                        cycle=true;
                        cout<<"Nodi: "<<min_edge->u<<" "<<min_edge->v<<" CICLO TROVATO "<<endl;
                        break; 
                    } 
                    elem = elem->next;
                    
                }
                if(!cycle)
                {
                    graph[min_edge->v]->rappresentante = graph[min_edge->u]->rappresentante;
                    node_t* elem_v = graph[min_edge->v]->head;
                    while(elem_v->next != NULL)
                    {
                        if(elem_v->connected == true)
                        {
                            graph[elem_v->val]->rappresentante = graph[min_edge->u]->rappresentante;
                        }
                        elem_v = elem_v->next;
                    }
                    cout<<"Nodi: "<<min_edge->u<<" "<<min_edge->v<<" con peso: "<<min_edge->w<<endl;
                }
            }
        }
    }
}


int main(int argc, char **argv)
{
    graph = new list_t *[n_nodi];
    for(int i =0; i<n_nodi; i++)
    {
        graph[i] = new list;
        graph[i]->head = NULL;
    }
    edges = new edge_t *[max_edge];
    for(int i =0; i<max_edge; i++)
    {
        edges[i] = new edge {-1,-1,999999};
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

    insertion_sort(edges,max_edge);

    kruskal();

    return 0;
}