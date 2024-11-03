#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stack>

using namespace std;
std::stack<int> stackCycle;
std::stack<int> stackMaxCycle;

// compilazione: g++ lezione15-grafi-check-cycle.cpp
//
// Obiettivo:
// 1) controllo esistenza cicli grafo
// 2) estendere il programma per controllare tutti i nodi (e non solo DFS(0) )

int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 1;

int n = 0; /// dimensione dell'array

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step

int ct_visit = 0; // contatore durante visita

//////////////////////////////////////////////////
/// Definizione della struttura dati lista
//////////////////////////////////////////////////

/// struct per il nodo della lista
typedef struct node {
    int val;
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list {
    node *head;
    // node* tail; /// per lista doubly linked
} list_t;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati lista
//////////////////////////////////////////////////

//////////////////////////////////////////////////
/// Definizione della struttura dati grafo
//////////////////////////////////////////////////

int *V;          // elenco dei nodi del grafo
int *V_visitato; // nodo visitato?

// list_t* E;  /// array con le liste di adiacenza per ogni nodo
list_t **E; /// array di puntatori a le liste di adiacenza per ogni nodo
int n_nodi;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati grafo
//////////////////////////////////////////////////

/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

list_t *global_ptr_ref = NULL; /// usato per memorizzare il puntatore alla prima lista allocata

int get_address(void *node) {
    return (int)((long)node - (long)global_ptr_ref);
}

void node_print(int n) {

    output_graph << "node_" << n << "_" << n_operazione << endl;
    output_graph << "[ shape = oval; ";
    if (V_visitato[n] == 1)
        output_graph << "fillcolor = \"#bbbbbb\"; style=filled; ";
    if (V_visitato[n] == 2)
        output_graph << "fillcolor = \"#555555\"; style=filled; ";
    output_graph << "label = "
                 << "\"Idx: " << n << ", val: " << V[n] << "\" ];\n";

    node_t *elem = E[n]->head;
    while (elem != NULL) { /// disegno arco
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << elem->val << "_" << n_operazione << " [  color=gray ]\n";
        elem = elem->next;
    }
}

void graph_print() {
    for (int i = 0; i < n_nodi; i++)
        node_print(i);
    n_operazione++;
}

void list_print(list_t *l) {
    printf("Stampa lista\n");

    if (l->head == NULL) {
        printf("Lista vuota\n");
    } else {
        node_t *current = l->head;

        while (current != NULL) {
            if (!details)
                printf("%d, ", current->val);
            else { /// stampa completa
                if (current->next == NULL)
                    printf("allocato in %d [Val: %d, Next: NULL]\n",
                           get_address(current),
                           current->val);
                else
                    printf("allocato in %d [Val: %d, Next: %d]\n",
                           get_address(current),
                           current->val,
                           get_address(current->next));
            }
            current = current->next;
        }
        printf("\n");
    }
}

list_t *list_new(void) {
    list_t *l = new list;
    if (details) {
        printf("Lista creata\n");
    }

    l->head = NULL; //// perche' non e' l.head ?
    if (details) {
        printf("Imposto a NULL head\n");
    }

    return l;
}

void list_delete(list_t *l) {
    //// implementare rimozione dal fondo della lista
    //// deallocazione struct list
}

void list_insert_front(list_t *l, int elem) {
    /// inserisce un elemento all'inizio della lista
    node_t *new_node = new node_t;
    new_node->next = NULL;

    new_node->val = elem;

    new_node->next = l->head;

    l->head = new_node;
}

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void print_array_graph(int *A, int n, string c, int a, int l, int m, int r) {
    /// prepara il disegno dell'array A ed il suo contenuto (n celle)
    /// a e' il codice del nodo e c la stringa
    /// l,m,r i tre indici della bisezione

    // return ;

    output_graph << c << a << " [label=<" << endl;

    /// tabella con contenuto array
    output_graph << "<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" > " << endl;
    /// indici
    output_graph << "<TR  >";
    for (int j = 0; j < n; j++) {
        output_graph << "<TD ";
        output_graph << ">" << j << "</TD>" << endl;
    }
    output_graph << "</TR>" << endl;
    output_graph << "<TR>";
    // contenuto
    for (int j = 0; j < n; j++) {
        output_graph << "<TD BORDER=\"1\"";
        if (j == m)
            output_graph << " bgcolor=\"#00a000\""; /// valore testato
        else if (j >= l && j <= r)
            output_graph << " bgcolor=\"#80ff80\""; /// range di competenza
        output_graph << ">" << A[j] << "</TD>" << endl;
    }
    output_graph << "</TR>" << endl;
    output_graph << "</TABLE> " << endl;

    output_graph << ">];" << endl;
}

int maxCycle = 0;
int indexCycle = 0;
int has_cycle(int n, int inizio) {

    graph_print();

    if (details)
        printf("DFS: lavoro sul nodo %d (visitato %d)\n", n, V_visitato[n]);

    if (V_visitato[n] == 1){
        //indexCycle++;
        //stackCycle.push(n);
        if( n == inizio && indexCycle > maxCycle){
            //indexCycle++;
            maxCycle = indexCycle;
            stackMaxCycle = stackCycle;
        }

        return 1;  // c'e' il ciclo
    }



    if (V_visitato[n] == 2)
        return 0;  // trovato un nuovo percorso alternativo (non c'e' il ciclo)

    indexCycle++;
    stackCycle.push(n);

    V_visitato[n] = 1; // prima volta che incontro questo nodo

    if (details)
        printf("Visito il nodo %d (val %d)\n", n, V[n]);

    /// esploro la lista di adiacenza
    int t = 0;
    node_t *elem = E[n]->head;
    while (elem != NULL) { /// elenco tutti i nodi nella lista

        /// espando arco  n --> elem->val
        /// quindi DFS(elem->val)
        output_graph << "dfs_" << n << " -> dfs_" << elem->val;
        if (V_visitato[elem->val])
            output_graph << "[color=gray, label = \"" << ct_visit++ << "\"]";
        else
            output_graph << "[color=red, label = \"" << ct_visit++ << "\"]";
        output_graph << endl;

        
        t += has_cycle(elem->val,inizio);

        if(n==inizio)
        {
            for(int i =0; i<=n_nodi;i++){
                if (i == n) continue;
                V_visitato[i]=0;
            }
        }
        


        elem = elem->next;
    }
    if(!stackCycle.empty()){
            stackCycle.pop();
    }
    indexCycle--;
    V_visitato[n] = 2; // abbandono il nodo per sempre

    return t;
}

void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int parse_cmd(int argc, char **argv) {
    /// controllo argomenti
    int ok_parse = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'v') {
            details = 1;
            ok_parse = 1;
        }
        if (argv[i][1] == 'g') {
            graph = 1;
            ok_parse = 1;
        }
    }

    if (argc > 1 && !ok_parse) {
        printf("Usage: %s [Options]\n", argv[0]);
        printf("Options:\n");
        printf("  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n");
        printf("  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n");
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    int i, test;

    srand((unsigned)time(NULL));

    if (parse_cmd(argc, argv))
        return 1;

    if (graph) {
        output_graph.open("graph.dot");
        /// preparo header
        output_graph << "digraph g" << endl;
        output_graph << "{ " << endl;
        output_graph << "node [shape=none]" << endl;
        output_graph << "rankdir=\"LR\"" << endl;
        ;
        //    output_graph << "edge[tailclip=false,arrowtail=dot];"<<endl;
    }

    // int* V; // elenco dei nodi del grafo
    // list_t* E;  /// array con le liste di adiacenza per ogni nodo

    n_nodi = 5;
    V = new int[n_nodi];          //(int*)malloc(n_nodi*sizeof(int));
    V_visitato = new int[n_nodi]; //(int*)malloc(n_nodi*sizeof(int));

    E = new list_t *[n_nodi]; //(list_t**)malloc(n_nodi*sizeof(list_t*));

    // inizializzazione
    for (int i = 0; i < n_nodi; i++) {
        V[i] = 2 * i;
        V_visitato[i] = 0; // flag = non visitato

        E[i] = list_new();

        if (i == 0)
            global_ptr_ref = E[i];

        for (int j = 0; j < n_nodi; j++) {
            //list_insert_front(E[i], j);

            if (rand()%2==0)
            //  if (i < j && j < 3)
                  list_insert_front(E[i], j);
            //if (i >= 2 && j >= 3)
                 //if (i != j)
                     //list_insert_front(E[i], j);
        }
    }
    // list_insert_front(E[0], 4);
    // list_insert_front(E[0], 1);
    // list_insert_front(E[1], 0);
    // list_insert_front(E[1], 1);
    // list_insert_front(E[1], 4);
    // list_insert_front(E[2], 3);
    // list_insert_front(E[3], 0);
    // list_insert_front(E[3], 1);
    // list_insert_front(E[3], 2);
    // list_insert_front(E[3], 3);
    // list_insert_front(E[4], 2);
    // list_insert_front(E[4], 3);

    //    list_insert_front( E[n_nodi-2] , n_nodi-1);
    //  list_insert_front( E[2] , 0);

    graph_print();

    for (int i = 0; i < n_nodi; i++) {

        printf("Sono il nodo di indice %d nell'array\n", i);
        printf("Il valore del nodo e' %d\n", V[i]);
        printf("La lista di adiacenza e'\n");
        list_print(E[i]);
    }

    int t = 0;
    for (int i = 0; i < n_nodi; i++) {
        t += has_cycle(i,i);
        indexCycle = 0;
        for (int j = 0; j < n_nodi; j++) {
            V_visitato[j]=0;
        }
        while(!stackCycle.empty()){
            stackCycle.pop();
        }
         //printf("test %d: ciclo = %d\n", i, t);
    }
    cout<<"Nodi che compongono il ciclo: ";
    std::stack<int> tempStack;
    while(!stackMaxCycle.empty()){
        tempStack.push(stackMaxCycle.top());
        stackMaxCycle.pop();
    }
    while (!tempStack.empty()) {
        std::cout << tempStack.top() << ", ";
        tempStack.pop();
    }
    
    printf("ciclo = %d\n", t > 0);
    cout<<"Lunghezza ciclo max: "<<maxCycle<<endl;

    if (graph) {
        /// preparo footer e chiudo file
        output_graph << "}" << endl;
        output_graph.close();
        cout << " File graph.dot scritto" << endl
             << "Creare il grafo con: dot graph.dot -Tpdf -o graph.pdf" << endl;
    }

    return 0;
}
