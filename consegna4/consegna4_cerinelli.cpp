// Codice scritto/alterato da Cerinelli Matteo(matricola 294907) per l'esame di Laboratorio Algoritmi e Strutture Dati
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

// compilazione: g++ lezione17-grafi-dijkstra.cpp
//
// Obiettivo:
// 1) grafo con archi pesati
// 2) implementazione shortest path

#define INFTY 1000000

int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 1;

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step

int ct_visit = 0; // contatore durante visita

//////////////////////////////////////////////////
/// Definizione della struttura dati lista
//////////////////////////////////////////////////

/// struct per il nodo della lista
typedef struct node
{
    int val; /// prossimo nodo
    float w; /// peso dell'arco
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list
{
    node *head;
} list_t;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati lista
//////////////////////////////////////////////////

//////////////////////////////////////////////////
/// Definizione della struttura dati grafo
//////////////////////////////////////////////////

int *V;          // elenco dei nodi del grafo
int *V_visitato; // nodo visitato?
int *V_prev;     // nodo precedente dalla visita
float *V_dist;   // distanza da sorgente

/// CODICE CERINELLI
const int MAX_SIZE = 256; /// allocazione statica
int heap[MAX_SIZE]; //min heap con al primo posto la distanza minima
int posizione_nodi_heap[MAX_SIZE]; //array con indice che mi indica dove nell'heap è la distanza di quel nodo
int array_support_heap_nodi[MAX_SIZE]; //array che mi dice dove nell'array posizione_nodi c'è un determinato nodo
                                       //mi serve perchè altrimenti io posso dal nodo risalire all'heap, ma non viceversa
                                       //es: l'heap in posizione 2 a che nodo appartiene? potrei fare un ciclo nell'array
                                       //posizione_nodi finchè non lo trovo, ma a quel punto non è più ottimale.
                                       //perciò uso questo nuovo array.
                                       //es:
                                       //heap                   [5,10,2000,-1,-1,-1,-1,-1] 5 è il min
                                       //posizione_nodi_heap    [-1,-1,0,-1,2,-1,-1,1] il nodo 2 è il min
                                       //support_heap_nodi      [2,7,4,-1,-1,-1,-1,-1] il nodo min si trova in posizione 2
int indice_minimo = -8888; //quale nodo è il minimo
int indice_massimo_corrente = -8888; //quale nodo è il max in quel momento
int indice_massimo = -999; //quale nodo è il max (serve per la remove_min)
int heap_size = 0; /// dimensione attuale dell'heap
/// FINE CODICE CERINELLI

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

int get_address(void *node)
{
    return (int)((long)node - (long)global_ptr_ref);
}

void node_print(int n)
{

    /// calcolo massima distanza (eccetto infinito)
    float max_d = 0;
    for (int i = 0; i < n_nodi; i++)
        if (max_d < V_dist[i] && V_dist[i] < INFTY)
            max_d = V_dist[i];

    output_graph << "node_" << n << "_" << n_operazione << endl;
    output_graph << "[ shape = oval; ";

    if (V_visitato[n] == 1)
        output_graph << "penwidth = 4; ";

    float col = V_dist[n] / max_d; /// distanza in scala 0..1
    output_graph << "fillcolor = \"0.0 0.0 " << col / 2 + 0.5 << "\"; style=filled; ";
    if (V_dist[n] < INFTY)
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: " << V_dist[n] << "\" ];\n";
    else
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: INF\" ];\n";

    node_t *elem = E[n]->head;
    while (elem != NULL)
    { /// disegno arco
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << elem->val << "_" << n_operazione << " [ label=\"" << elem->w << "\", len=" << elem->w / 100 * 10 << " ]\n";
        elem = elem->next;
    }

    if (V_prev[n] != -1)
    { // se c'e' un nodo precedente visitato -> disegno arco

        float len = 0;

        len = 1;
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << V_prev[n] << "_" << n_operazione << " [ color=blue, penwidth=5, len=" << len / 100 * 10 << " ]\n";
    }
}

void graph_print()
{
    for (int i = 0; i < n_nodi; i++)
        node_print(i);
    n_operazione++;
}

void list_print(list_t *l)
{
    printf("Stampa lista\n");

    if (l->head == NULL)
    {
        printf("Lista vuota\n");
    }
    else
    {
        node_t *current = l->head;

        while (current != NULL)
        {
            if (!details)
                printf("%d w:%f, ", current->val, current->w);
            else
            { /// stampa completa
                if (current->next == NULL)
                    printf("allocato in %d [Val: %d, W: %f, Next: NULL]\n",
                           get_address(current),
                           current->val,
                           current->w);
                else
                    printf("allocato in %d [Val: %d, W: %f, Next: %d]\n",
                           get_address(current),
                           current->val,
                           current->w,
                           get_address(current->next));
            }
            current = current->next;
        }
        printf("\n");
    }
}

list_t *list_new(void)
{
    list_t *l = new list;
    if (details)
    {
        printf("Lista creata\n");
    }

    l->head = NULL; //// perche' non e' l.head ?
    if (details)
    {
        printf("Imposto a NULL head\n");
    }

    return l;
}

void list_insert_front(list_t *l, int elem, float w)
{
    /// inserisce un elemento all'inizio della lista
    node_t *new_node = new node_t;
    new_node->next = NULL;

    new_node->val = elem;
    new_node->w = w;

    new_node->next = l->head;

    l->head = new_node;
}

void print_array(int *A, int dim)
{
    for (int j = 0; j < dim; j++)
    {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void print_array_graph(int *A, int n, string c, int a, int l, int m, int r)
{
    /// prepara il disegno dell'array A ed il suo contenuto (n celle)
    /// a e' il codice del nodo e c la stringa
    /// l,m,r i tre indici della bisezione

    // return ;

    output_graph << c << a << " [label=<" << endl;

    /// tabella con contenuto array
    output_graph << "<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" > " << endl;
    /// indici
    output_graph << "<TR  >";
    for (int j = 0; j < n; j++)
    {
        output_graph << "<TD ";
        output_graph << ">" << j << "</TD>" << endl;
    }
    output_graph << "</TR>" << endl;
    output_graph << "<TR>";
    // contenuto
    for (int j = 0; j < n; j++)
    {
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

int parent_idx(int n)
{
    if (n == 0)
        return -1;
    return (n - 1) / 2;
}

int child_L_idx(int n)
{
    if (2 * n + 1 >= heap_size)
        return -1;
    return 2 * n + 1;
}

int child_R_idx(int n)
{
    if (2 * n + 2 >= heap_size)
        return -1;
    return 2 * n + 2;
}

/// restituisce 0 se il nodo in posizione n e' un nodo interno (almeno un figlio)
/// restituisce 1 se il nodo non ha figli
int is_leaf(int n)
{
    return (child_L_idx(n) == -1); // non c'e' bisogno di controllare il figlio R
}

// CODICE CERINELLI
void heap_insert(int elem)
{
    /// inserisco il nuovo nodo con contenuto elem
    /// nell'ultima posizione dell'array
    /// ovvero continuo a completare il livello corrente

    if (indice_massimo == -999) // prima volta che entro nella heap insert, quindi il massimo è per forza lui
    {
        indice_massimo = elem;
    }
    if (details)
        printf("Inserisco elemento %d in posizione %d\n", elem, heap_size);

    if (heap_size < MAX_SIZE)
    {
        int i = heap_size;
        heap_size++;

        // heap[i] = elem;
        heap[i] = V_dist[elem]; // Metto nell'heap  in posizione I la distanza alla posizione ELEM   -> heap[0] -> v_dist[10] -> posizione 0 valore nodo 10
        posizione_nodi_heap[elem] = i; //metto la posizione dell'heap nell'array di nodi nell'indice ELEM
        indice_massimo_corrente = elem; // il nodo con dist maggiore è l'ultimo inserito (per ora)
        array_support_heap_nodi[i] = elem; //segno il nodo che sta nella posizione i dell'heap

        while (i != 0)
        { // non sono sulla radice
            if (heap[parent_idx(i)] <= heap[i] && heap[parent_idx(i)] != -1)
            { /// proprieta' dell' heap e' rispettata -> esco
                if (details)
                    printf("Il genitore ha valore %d >= del nodo %d, esco\n", heap[parent_idx(i)], heap[i]);
                break;
            }

            if (details)
                printf("Il genitore ha valore %d < del nodo %d, swap\n", heap[parent_idx(i)], heap[i]);
            /// il nodo ha un genitore sicuramente >   --> swap
            int t = heap[parent_idx(i)];
            heap[parent_idx(i)] = heap[i];
            heap[i] = t;

            int temp = posizione_nodi_heap[array_support_heap_nodi[parent_idx(i)]];
            posizione_nodi_heap[array_support_heap_nodi[parent_idx(i)]] = posizione_nodi_heap[array_support_heap_nodi[i]];
            posizione_nodi_heap[array_support_heap_nodi[i]] = temp;

            int temp2 = array_support_heap_nodi[parent_idx(i)];
            array_support_heap_nodi[parent_idx(i)] = array_support_heap_nodi[i];
            array_support_heap_nodi[i] = temp2;

            // tree_print_graph(0); // radice
            // n_operazione++;

            i = parent_idx(i);
            posizione_nodi_heap[elem] = i; // ridondante ???
        }
        if (i == 0) //se sono arrivato fino alla "cima" dell'heap
        {
            indice_minimo = elem;
        }
        if (posizione_nodi_heap[indice_massimo_corrente] == (heap_size - 1)) //se sono alla fine dell'heap
        {
            indice_massimo = indice_massimo_corrente;// il max è il nuovo inserito
        }
    }
    else
        printf("Heap pieno!\n");
}

void decrease_key(int indice_nodo, int key)
{ // non mi serve a nulla?
    // key = nuovo valore

    if (posizione_nodi_heap[indice_nodo] == -8888)
    {
        printf("Nodo non esistente\n");
        return;
    }

    if (heap[posizione_nodi_heap[indice_nodo]] < key)
    {
        printf("la chiave non e' piu' piccola!\n");
        return;
    }

    heap[posizione_nodi_heap[indice_nodo]] = key; //setto il nuovo valore di distanza

    int i = posizione_nodi_heap[indice_nodo]; //mi segno a che indice dell'heap sono e inizio a riordinare
    while (i != 0)
    { // non sono sulla radice
        if (heap[parent_idx(i)] <= heap[i] && heap[parent_idx(i)] != -1)
        { /// proprieta' dell' heap e' rispettata -> esco
            if (details)
                printf("Il genitore ha valore %d <= del nodo %d, esco\n", heap[parent_idx(i)], heap[i]);
            return;
        }

        if (details)
            printf("Il genitore ha valore %d > del nodo %d, swap\n", heap[parent_idx(i)], heap[i]);
        /// il nodo ha un genitore sicuramente <   --> swap
        int parentNode = heap[parent_idx(i)];
        heap[parent_idx(i)] = heap[i];
        heap[i] = parentNode;

        int temp = posizione_nodi_heap[array_support_heap_nodi[parent_idx(i)]];
        posizione_nodi_heap[array_support_heap_nodi[parent_idx(i)]] = posizione_nodi_heap[array_support_heap_nodi[i]];
        posizione_nodi_heap[array_support_heap_nodi[i]] = temp;

        int temp2 = array_support_heap_nodi[parent_idx(i)];
        array_support_heap_nodi[parent_idx(i)] = array_support_heap_nodi[i];
        array_support_heap_nodi[i] = temp2;

        // tree_print_graph(0); // radice
        n_operazione++;

        i = parent_idx(i);
        posizione_nodi_heap[indice_nodo] = i; // superfluo
    }
    if (i == 0) //se sono arrivato in cima segno il nuovo minimo
    {
        indice_minimo = indice_nodo;
    }
}

int heap_remove_min()
{

    if (heap_size <= 0)
    { /// heap vuoto!
        printf("Errore: heap vuoto\n");
        return -7777;
    }

    int minimo_heap = heap[0];
    int minimo_nodi = indice_minimo;

    if (details)
        printf("Minimo identificato %d\n", minimo_heap);

    //inizio lo swap tra il minimo e il massimo, per poi togliere di 1 la heap size (così da sovrascriverlo in futuro)
    //e poi riordino l'heap con la heapify_down (anzichè controllare i != 0, controllo se sono una foglia)
    heap[0] = heap[heap_size - 1];
    heap[heap_size - 1] = minimo_heap;

    int temp = posizione_nodi_heap[indice_minimo];
    posizione_nodi_heap[indice_minimo] = posizione_nodi_heap[indice_massimo];
    posizione_nodi_heap[indice_massimo] = temp;

    int temp2 = array_support_heap_nodi[0];
    array_support_heap_nodi[0] = array_support_heap_nodi[heap_size - 1];
    array_support_heap_nodi[heap_size - 1] = temp2;

    // elimino il minimo (ora in fondo all'array)
    array_support_heap_nodi[heap_size - 1] = -8888; //metto -8888 per indicare che non c'è più nulla
    posizione_nodi_heap[indice_minimo] = -8888;
    heap_size--;


    int i = 0; // indice di lavoro (parto dalla root)

    while (!is_leaf(i))
    { /// garantisco di fermarmi alla foglia

        if (details)
            printf("Lavoro con il nodo in posizione i = %d, valore %d\n", i, heap[i]);

        int con_chi_mi_scambio = -8888;

        /// controllo il nodo i con il suo figlio L
        if (heap[i] > heap[child_L_idx(i)])
        { // il nodo i e' piu' grande
            /// attivare uno swap (la proprieta' heap non e' rispettata)
            con_chi_mi_scambio = child_L_idx(i);
            if (details)
                printf("Figlio L e' piu' piccolo (valore %d)\n", heap[child_L_idx(i)]);

            if (child_R_idx(i) >= 0 && // esiste il nodo destro
                heap[child_L_idx(i)] > heap[child_R_idx(i)])
            {
                con_chi_mi_scambio = child_R_idx(i);
                if (details)
                    printf("Figlio R e' ancora piu' piccolo (valore %d)\n", heap[child_R_idx(i)]);
            }
        }
        else
        { // il nodo e' piu' piccolo del figlio L

            if (child_R_idx(i) >= 0)
            { // esiste il figlio R
                if (heap[i] > heap[child_R_idx(i)])
                { /// attivo lo swap
                    con_chi_mi_scambio = child_R_idx(i);
                    if (details)
                        printf("Figlio R e' piu' piccolo del nodo (valore %d)\n", heap[child_R_idx(i)]);
                }
                else
                    break;
            }
            else
                break;
        }

        /// swap tra i e con_chi_mi_scambio
        int t = heap[i];
        heap[i] = heap[con_chi_mi_scambio];
        heap[con_chi_mi_scambio] = t;

        int temp = posizione_nodi_heap[array_support_heap_nodi[i]];
        posizione_nodi_heap[array_support_heap_nodi[i]] = posizione_nodi_heap[con_chi_mi_scambio];
        posizione_nodi_heap[con_chi_mi_scambio] = temp;

        int temp2 = array_support_heap_nodi[i];
        array_support_heap_nodi[i] = array_support_heap_nodi[con_chi_mi_scambio];
        array_support_heap_nodi[con_chi_mi_scambio] = temp2;

        i = con_chi_mi_scambio;
        posizione_nodi_heap[2];
        i;

        // tree_print_graph(0);  // radice
        // n_operazione++;
    }

    return minimo_nodi; // indice_minimo?
}
// FINE CODICE CERINELLI

void shortest_path_originale(int n)
{

    /*      V_visitato[i]=0;  // flag = non visitato
      V_prev[i]=-1;  // non c'e' precedente
      V_dist[i]=INFTY;  // infinito
    */

    V_dist[n] = 0;

    int q_size = n_nodi; /// contatore degli elementi in coda (V_visitato)

    while (q_size != 0)
    {

        graph_print();

        /// trova il minimo in coda
        float best_dist = INFTY;
        int best_idx = -1;
        for (int i = 0; i < n_nodi; i++)
        {
            if (V_visitato[i] == 0 && V_dist[i] < best_dist)
            { /// nodo e' in coda e e' migliore del nodo corrente
                best_dist = V_dist[i];
                best_idx = i;
            }
        }
        if (best_idx >= 0)
        {
            /// estrai dalla coda
            int u = best_idx;
            V_visitato[u] = 1;
            q_size--;

            /// esploro la lista di adiacenza
            node_t *elem = E[u]->head;
            while (elem != NULL)
            {
                int v = elem->val; /// arco u --> v

                /// alt ← dist[u] + Graph.Edges(u, v)
                float alt = V_dist[u] + elem->w; /// costo per arrivare al nuovo nodo passando per u
                if (alt < V_dist[v])
                { // il percorso sorgente ---> u --> v migliora il percorso attuale sorgente --> v
                    V_dist[v] = alt;
                    V_prev[v] = u;
                }
                elem = elem->next;
            }
        }
        else
        { /// coda non vuota E nodi non raggiungibili ---> FINITO
            q_size = 0;
        }
    }

    graph_print();
}

// CODICE CERINELLI
void shortest_path_cerinelli(int n)
{

    V_dist[n] = 0;  // distanza, setto l'origine a 0
    heap_insert(n); // inserisco nell'heap delle distanze il primo valore 0

    int q_size = n_nodi; /// contatore degli elementi in coda (V_visitato)

    while (q_size != 0)
    {

        graph_print();
        float nodo_con_dist_minore = heap_remove_min();
        if (nodo_con_dist_minore >= 0)
        {
            /// estrai dalla coda
            int u = nodo_con_dist_minore;
            V_visitato[u] = 1;
            q_size--;

            /// esploro la lista di adiacenza
            node_t *elem = E[u]->head;
            while (elem != NULL)
            {
                int v = elem->val; /// arco u --> v

                /// alt ← dist[u] + Graph.Edges(u, v)
                float alt = V_dist[u] + elem->w; /// costo per arrivare al nuovo nodo passando per u
                if (alt < V_dist[v])
                { // il percorso sorgente ---> u --> v migliora il percorso attuale sorgente --> v
                    V_dist[v] = alt;
                    V_prev[v] = u;
                    if (posizione_nodi_heap[v] == -8888)
                    { // prima volta che trovo il nodo
                        heap_insert(v);
                    }
                    else
                    {
                        decrease_key(v, alt);
                    }
                }
                elem = elem->next;
            }
        }
        else
        {
            q_size = 0; /// coda non vuota E nodi non raggiungibili ---> FINITO
        }
    }
}

bool bellmanford_cerinelli(int elem)
{
    for (int i = 0; i < n_nodi - 1; i++)
    {
        shortest_path_cerinelli(elem); // rilasso per n_nodi -1 volte gli archi
    }

    // Controllo per cicli negativi
    for (int u = 0; u < n_nodi; u++)
    {
        node_t *elem = E[u]->head;
        while (elem != NULL)
        {
            int v = elem->val;
            float alt = V_dist[u] + elem->w;
            if (alt < V_dist[v])
            {
                cout << "Errore: Ciclo negativo!!!" << endl;
                return false;
            }
            elem = elem->next;
        }
    }
    cout << "Nessun Ciclo Negativo" << endl;
    return true;
}
// FINE CODICE CERINELLI

void swap(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int parse_cmd(int argc, char **argv)
{
    /// controllo argomenti
    int ok_parse = 0;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][1] == 'v')
        {
            details = 1;
            ok_parse = 1;
        }
        if (argv[i][1] == 'g')
        {
            graph = 1;
            ok_parse = 1;
        }
    }

    if (argc > 1 && !ok_parse)
    {
        printf("Usage: %s [Options]\n", argv[0]);
        printf("Options:\n");
        printf("  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n");
        printf("  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n");
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    int i, test;

    // init random
    srand((unsigned)time(NULL));

    if (parse_cmd(argc, argv))
        return 1;

    if (graph)
    {
        output_graph.open("graph.dot");
        /// preparo header
        output_graph << "digraph g" << endl;
        output_graph << "{ " << endl;
        output_graph << "node [shape=none]" << endl;
        output_graph << "rankdir=\"LR\"" << endl;
        ;
        //    output_graph << "edge[tailclip=false,arrowtail=dot];"<<endl;
    }

    int N = 10;
    n_nodi = N * N;

    n_nodi = 5;

    //// init nodi
    V = new int[n_nodi];
    V_visitato = new int[n_nodi];
    V_prev = new int[n_nodi];
    V_dist = new float[n_nodi];
    // heap = new int[n_nodi];
    //// init archi
    E = new list_t *[n_nodi]; //(list_t**)malloc(n_nodi*sizeof(list_t*));

    // costruzione grafo
    for (int i = 0; i < n_nodi; i++)
    {
        V[i] = 2 * i;
        V_visitato[i] = 0; // flag = non visitato
        V_prev[i] = -1;    // non c'e' precedente
        V_dist[i] = INFTY; // infinito
        heap[i] = -8888;
        posizione_nodi_heap[i] = -8888; // ogni nodo corrisponde alla cella pari dell'heap ( 1 -> 1, 2 -> 2 ecc...)
        array_support_heap_nodi[i] = -8888;
        E[i] = list_new();

        if (i == 0)
            global_ptr_ref = E[i];

        int x = i % N;
        int y = i / N;

        // for (int dx = -2; dx <= 2; dx += 1)
        //     for (int dy = -2; dy <= 2; dy += 1)
        //         if (abs(dx) + abs(dy) >= 1 &&
        //             abs(dx) + abs(dy) <= 1
        //             ) { // limito gli archi ai vicini con 1 variazione assoluta sulle coordinate

        //             int nx = x + dx;
        //             int ny = y + dy;

        //             if (nx >= 0 && nx < N &&
        //                 ny >= 0 && ny < N) { /// coordinate del nuovo nodo sono nel grafo

        //                 int j = nx + N * ny; /// indice del nuovo nodo
        //                 list_insert_front(E[i], j, 15 * sqrt(dx*dx + dy*dy));
        //             }
        //         }
    }

    int partenza = 0;
    int arrivo = n_nodi - 1;
    int w_max = 100;


    //SCOMMENTARE PER GRAFO CON NODI COSTOSI
    // for (int i = 0; i < n_nodi - 1; i++)
    // {
    //     /// arco costoso
    //     list_insert_front(E[i], arrivo, w_max - 2 * i);
    //     /// arco 1
    //     if (i > 0)
    //         list_insert_front(E[i - 1], i, 1);
    // }

    graph_print();

    for (int i = 0; i < n_nodi; i++)
    {
        printf("Sono il nodo di indice %d nell'array\n", i);
        printf("Il valore del nodo e' %d\n", V[i]);
        printf("La lista di adiacenza e'\n");
        list_print(E[i]);
    }

// CODICE CERINELLI

    list_insert_front(E[0], 1, 3);
    list_insert_front(E[0], 2, 2);
    list_insert_front(E[1], 3, 4);
    list_insert_front(E[2], 3, -2); //CAMBIARE -2 in -3 per renderlo negativo
    list_insert_front(E[4], 2, 1);
    list_insert_front(E[3], 4, 1);


    shortest_path_cerinelli(0);

    bellmanford_cerinelli(0);

// FINE CODICE CERINELLI

    if (graph)
    {
        /// preparo footer e chiudo file
        output_graph << "}" << endl;
        output_graph.close();
        cout << " File graph.dot scritto" << endl
             << "****** Creare il grafo con: neato graph.dot -Tpdf -o graph.pdf" << endl;
    }

    return 0;
}
