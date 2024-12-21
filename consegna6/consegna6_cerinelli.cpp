#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

// compilazione: g++ lezione19.cpp
//
// Obiettivo:
// 1)

int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step

int ct_visit = 0; // contatore durante visita

typedef struct list my_stack;

my_stack *s;

int idx = 0;

//////////////////////////////////////////////////
/// Definizione della struttura dati lista
//////////////////////////////////////////////////

/// struct per il nodo della lista
typedef struct node
{
    int val;
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list
{
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
int *V_lowlink;  //
int *V_onStack;

int **M;

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

// CODICE CERINELLI

char *file1; //stringhe dei due file
char *file2;

int sizeFile1;  //dimensione dei due file
int sizeFile2;

int maxLCS = 0; //lunghezza longest substring
int LCSXcoordinate; //coordinate della matrice della substring
int LCSYCoordinate;

// FINE CODICE CERINELLI

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


// CODICE CERINELLI

char *getStringFromFileWithoutCarrelReturn(const char *nameFile, int &sizeFile)
{
    ifstream stream1;                // apro input stream
    stream1.open(nameFile);          // apro file
    stream1.seekg(0, std::ios::end); // vai alla fine del file
    size_t size = stream1.tellg();   // prendi la size

    char *contentOfFile = new char[size + 1]; // creo variabile che contiene contenuto del file
    stream1.seekg(0);                         // torna all'inizio del file
    stream1.read(contentOfFile, size);        // popolo variabile

    char *newContent = new char[size + 1]; // Creo variabile per nuovo file senza \n
    int newSize = 0;                       // Nuova dimensione nuova stringa senza \n

    for (int i = 0; i < size; i++)
    { // ciclo per togliere \n
        if (contentOfFile[i] != '\n')
        {
            newContent[newSize++] = contentOfFile[i];
        }
    }
    newContent[newSize]='\0';
    sizeFile = newSize;
    return newContent;
}

char *getStringFromFileWithCarrelReturn(const char *nameFile, int &sizeFile)
{
    ifstream stream1;                // apro input stream
    stream1.open(nameFile);          // apro file
    stream1.seekg(0, std::ios::end); // vai alla fine del file
    size_t size = stream1.tellg();   // prendi la size

    char *contentOfFile = new char[size + 1]; // creo variabile che contiene contenuto del file
    stream1.seekg(0);                         // torna all'inizio del file
    stream1.read(contentOfFile, size);        // popolo variabile
    contentOfFile[size]='\0';
    sizeFile = size;
    return contentOfFile;
}

void longestCommonSubstring()
{
    int matrixLCS[sizeFile1 + 1][sizeFile2 + 1]; // cremo matrice di grandezza file1 * file2

    // popolo matrice tutta a 0
    for (int x = 0; x < sizeFile1 + 1; x++)
    {
        for (int y = 0; y < sizeFile2 + 1; y++)
        {
            matrixLCS[x][y] = 0;
        }
    }

    // ciclo per individuazione longest common substring
    // aggiungo +1 a file size per permettere la riga e colonna vuota
    // tolgo -1 perchè la stringa è rimasta invariata e non la il carattere vuoto
    for (int x = 1; x < sizeFile1 + 1; x++)
    {
        for (int y = 1; y < sizeFile2 + 1; y++)
        {
            // se è lo stesso carattere
            if (file1[x - 1] == file2[y - 1])
            {
                // se ho due spazi riporto il risultato vecchio, cosi' int main == int      main
                if (file1[x - 1] == ' ' && file1[x] == ' ' && file2[y - 1] == ' ' && file2[y] == ' ')
                {
                    matrixLCS[x][y] = matrixLCS[x - 1][y - 1];
                    continue;
                }

                matrixLCS[x][y] = matrixLCS[x - 1][y - 1] + 1;
                if (maxLCS < matrixLCS[x][y])
                {
                    maxLCS = matrixLCS[x][y];
                    LCSXcoordinate = x;
                    LCSYCoordinate = y;
                }
            }
        }
    }

    // Stampo matrice con diagonali per LCS
    for (int i = 0; i <= sizeFile2; i++)
    {
        if (i == 0)
            cout << "  -";
        else
            cout << file2[i - 1];
    }
    cout << endl;
    for (int x = 0; x < sizeFile1 + 1; x++)
    {
        if (x == 0)
            cout << "- ";
        else
            cout << file1[x - 1] << " ";
        for (int y = 0; y < sizeFile2 + 1; y++)
        {
            cout << matrixLCS[x][y];
        }
        cout << endl;
    }

    // Stampo la longest common substring:
    cout << "Longest common substring: ";

    const int maxSize = 100; // Dimensione massima della stringa
    char bufferReversedString[maxSize] = ""; // Inizializziamo il buffer con una stringa vuota
    int currentLength = 0; // Lunghezza attuale della stringa

    int i = 1; //parto da 1 perchè la matrice ha una riga extra
    int posizioneCarattereFinale = LCSXcoordinate;
    while (matrixLCS[LCSXcoordinate][LCSYCoordinate] != 0) // risalgo la diagonale finche' non arrivo a 0
    {
        bufferReversedString[currentLength++] = file1[posizioneCarattereFinale - (i)]; // stampo non al contrario la stringa
        i++;
        LCSXcoordinate--;
        LCSYCoordinate--;
    }

    bufferReversedString[currentLength] = '\0';

    for (int i = currentLength - 1; i >= 0; --i) { //chiesto a chatgpt come stamparla al contrario (in ordine giusto)
        cout << bufferReversedString[i];
    }
    cout<<endl;
}

void checkDifferences()
{   
   
    char* saveptr1;
    char* saveptr2;
    char* file1Splitted = strtok_r(file1, "\n", &saveptr1); //chiesto a chatgpt come fare la split di un puntatore a char
    char* file2Splitted = strtok_r(file2, "\n", &saveptr2);
    int counterOpenBracketFile1 = 0; //contatore parentesi {
    int counterOpenBracketFile2 = 0;
    int rowFile1=1; //contatore righe
    int rowFile2=1;
    bool closedBracketFile1 = false; //mi serve per sapere se chiudo una { prima che nell'altro file
    bool closedBracketFile2 = false;
    
    if(strchr(file1Splitted, '{') != nullptr)
        counterOpenBracketFile1++;
    if(strchr(file2Splitted, '{') != nullptr)
        counterOpenBracketFile2++;

    while(file1Splitted != nullptr || file2Splitted != nullptr)
    {

        if(counterOpenBracketFile1 > counterOpenBracketFile2 && closedBracketFile2 == false) //Se ho aperto una funzione nel file1, vado avanti con le aggiunte in file2
        {
            cout<<"CODICE AGGIUNTO! rigaFile2: "<<rowFile2<< " File2-> "<<file2Splitted<<endl;
            file2Splitted = strtok_r(nullptr, "\n", &saveptr2);
            rowFile2++;
            if(file2Splitted != nullptr && strchr(file2Splitted, '{') != nullptr)
                counterOpenBracketFile2++;
            if(file2Splitted != nullptr && strchr(file2Splitted, '}') != nullptr)
                counterOpenBracketFile2--;
        }
        else if(counterOpenBracketFile1 > counterOpenBracketFile2 && closedBracketFile2 == true)//Se ho aperto una funzione nel file2, vado avanti con le righe cancellate in file1
        {                                                                                       //SE ho chiuso una } nel file 2 ho eliminato righe nel file1
            closedBracketFile2 = false;
            cout<<"CODICE ELIMINATO! rigaFile1: "<<rowFile1<<" File1-> "<<file1Splitted<<endl;
            file1Splitted = strtok_r(nullptr, "\n", &saveptr1);
            rowFile1++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '{') != nullptr)
                counterOpenBracketFile1++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '}') != nullptr)
                counterOpenBracketFile1--;
        }
        else if(counterOpenBracketFile1 < counterOpenBracketFile2 && closedBracketFile1 == false)//Se ho aperto una funzione nel file2, vado avanti con le righe cancellate in file1
        {
            cout<<"CODICE ELIMINATO! rigaFile1: "<<rowFile1<<" File1-> "<<file1Splitted<<endl;
            file1Splitted = strtok_r(nullptr, "\n", &saveptr1);
            rowFile1++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '{') != nullptr)
                counterOpenBracketFile1++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '}') != nullptr)
                counterOpenBracketFile1--;
        }
        else if(counterOpenBracketFile1 < counterOpenBracketFile2 && closedBracketFile1 == true) //Se ho aperto una funzione nel file1, vado avanti con le aggiunte in file2
        {                                                                                        //SE ho chiuso una } nel file 1 ho aggiunto righe nel file2
            closedBracketFile1 = false;
            cout<<"CODICE AGGIUNTO! rigaFile2: "<<rowFile2<< " File2-> "<<file2Splitted<<endl;
            file2Splitted = strtok_r(nullptr, "\n", &saveptr2);
            rowFile2++;
            if(file2Splitted != nullptr && strchr(file2Splitted, '{') != nullptr)
                counterOpenBracketFile2++;
            if(file2Splitted != nullptr && strchr(file2Splitted, '}') != nullptr)
                counterOpenBracketFile2--;
        }
        else if(file1Splitted == nullptr) //file1 è terminato prima quindi sono tutte righe nuove in file2
        {
            cout<<"CODICE AGGIUNTO! rigaFile2: "<<rowFile2<< " File2-> "<<file2Splitted<<endl;
            file2Splitted = strtok_r(nullptr, "\n", &saveptr2);
            rowFile2++;
            if(file2Splitted != nullptr && strchr(file2Splitted, '{') != nullptr)
                counterOpenBracketFile2++;
            if(file2Splitted != nullptr && strchr(file2Splitted, '}') != nullptr)
                counterOpenBracketFile2--;           
        }
        else if(file2Splitted == nullptr) //file2 è terminato prima quindi sono tutte righe cancellate in file1
        {
            cout<<"CODICE ELIMINATO! rigaFile1: "<<rowFile1<<" File1-> "<<file1Splitted<<endl;
            file1Splitted = strtok_r(nullptr, "\n", &saveptr1);
            rowFile1++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '{') != nullptr)
                counterOpenBracketFile1++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '}') != nullptr)
                counterOpenBracketFile1--;
        }
        else if(strcmp(file1Splitted,file2Splitted) && 
            (counterOpenBracketFile1 != 0 && counterOpenBracketFile2 != 0)) //se il codice è cambiato (aggiunto controllo per vedere se sono 
        {                                                                   //all'ultima } perchè l'output su windows e' sporco)
            cout<<"CODICE CAMBIATO! rigaFile1: "<<rowFile1<<" File1-> "<<file1Splitted<<" Vs "<<file2Splitted<<" <-File2 RigaFile2: "<<rowFile2<<endl;
            file1Splitted = strtok_r(nullptr, "\n", &saveptr1);
            file2Splitted = strtok_r(nullptr, "\n", &saveptr2);
            rowFile1++;
            rowFile2++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '{') != nullptr)
                counterOpenBracketFile1++;
            if(file2Splitted != nullptr && strchr(file2Splitted, '{') != nullptr)
                counterOpenBracketFile2++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '}') != nullptr)
            {
                closedBracketFile1 = true;
                counterOpenBracketFile1--;
            }

            if(file2Splitted != nullptr && strchr(file2Splitted, '}') != nullptr)
            {
                closedBracketFile2 = true;
                counterOpenBracketFile2--;
            }

        }
        else //righe uguali
        {
            file1Splitted = strtok_r(nullptr, "\n", &saveptr1);
            file2Splitted = strtok_r(nullptr, "\n", &saveptr2);
            rowFile1++;
            rowFile2++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '{') != nullptr)
                counterOpenBracketFile1++;
            if(file2Splitted != nullptr && strchr(file2Splitted, '{') != nullptr)
                counterOpenBracketFile2++;
            if(file1Splitted != nullptr && strchr(file1Splitted, '}') != nullptr)
            {
                closedBracketFile1 = true;
                counterOpenBracketFile1--;
            }
            if(file2Splitted != nullptr && strchr(file2Splitted, '}') != nullptr)
            {
                closedBracketFile2 = true;
                counterOpenBracketFile2--;
            }
        }
    }
}



int main(int argc, char **argv)
{
    int i, test;

    srand((unsigned)time(NULL));

    if (parse_cmd(argc, argv))
        return 1;

    // V = new int[100];
    // for (int i = 0; i < 100; i++)
    //     V[i] = 0;

    // printf("fib: %d (n chiamate %d)\n", fib(30), ct_fib);

    // allinea();



    // INIZIO CODICE CERINELLI

    file1 = getStringFromFileWithoutCarrelReturn("file1.cpp", sizeFile1);
    file2 = getStringFromFileWithoutCarrelReturn("file2.cpp", sizeFile2);

    longestCommonSubstring();
    cout<<endl<<endl<<endl<<endl<<endl;

    file1 = getStringFromFileWithCarrelReturn("file1.cpp", sizeFile1);
    file2 = getStringFromFileWithCarrelReturn("file2.cpp", sizeFile2);

    checkDifferences();

    return 0;
}
/*
TEST FATTI CON I SEGUENTI FILE:

file1:
int main(){
    cout<<"Topolino";
    for(int i = 0; i<10;i++){
        if(i ==1){
            cout<<"Pippo";
        }
        cout<<"Eliminami";
    }
}


file2:
int main(){
    cout<<"Paperino";
    cout<<"Pluto";
    for(int i = 0; i<10;i++){
        if(i ==1){
            cout<<"Errore";
            cout<<"Aggiungimi";
        }
    }
}
*/