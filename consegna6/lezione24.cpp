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

// INIZIO CODICE CERINELLI

char *file1;
char *file2;

int sizeFile1;
int sizeFile2;

int maxLCS = 0;
int LCSXcoordinate;
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

int is_match(char temp1, char temp2)
{
    if ((temp1 == temp2) ||
        ((temp1 >= 'a' && temp1 <= 'z') &&
         (temp2 >= 'a' && temp2 <= 'z')))
        //  &&
        //     (temp1 < 'a' || temp1 > 'z') &&
        //     (temp2 < 'a' || temp2 > 'z'))
        return 1;
    return 0;
}

int fscore(char temp1, char temp2)
{

    /// match
    if (temp1 == temp2)
    {

        if (temp1 == '(' ||
            temp1 == ')' ||
            temp1 == '[' ||
            temp1 == ']' ||
            temp1 == '{' ||
            temp1 == '}' ||
            temp1 == ';')
            return 10;

        return 5;
    }

    /// mismatch

    //  &&
    //     (temp1 < 'a' || temp1 > 'z') &&
    //     (temp2 < 'a' || temp2 > 'z'))

    if ((temp1 >= 'a' && temp1 <= 'z') &&
        (temp2 >= 'a' && temp2 <= 'z'))
        return 3;

    return 0;
}

void allinea()
{
    const int s1 = 20;
    const int s2 = 20;

    char *str1;
    char *str2;

    str1 = new char[s1];
    str2 = new char[s2];

    char *temp2 = "if(a==0){b=3;} abc";
    char *temp1 = "xx(c==0){b=3;}   123";
    int i;

    int costo_match = 1;
    int costo_mismatch = -1;
    int costo_indel = -1;

    for (i = 0; i < s1; i++)
    {
        str1[i] = temp1[i];
    }
    for (i = 0; i < s2; i++)
    {
        str2[i] = temp2[i];
    }

    M = new int *[s1 + 1];
    for (i = 0; i < s1 + 1; i++)
        M[i] = new int[s2 + 1];

    for (int i = 0; i < s1 + 1; i++)
        M[i][0] = costo_indel;
    for (int j = 0; j < s2 + 1; j++)
        M[0][j] = costo_indel;

    // matrice previous
    char **P = new char *[s1 + 1];
    for (i = 0; i < s1 + 1; i++)
        P[i] = new char[s2 + 1];

    /// 0: vengo da i-1 (sopra)
    /// 1: vengo da j-1 (sinistra)
    /// 2: vengo da i-1, j-1 (diag)

    P[0][0] = -1;
    for (int i = 1; i < s1 + 1; i++)
        P[i][0] = 0;
    for (int j = 0; j < s2 + 1; j++)
        P[0][j] = 1;

    for (int i = 1; i < s1 + 1; i++)
        for (int j = 1; j < s2 + 1; j++)
        {

            /// sottosequenza
            int score = fscore(str1[i - 1], str2[j - 1]);

            if (score != 0)
            { // match/mismatch
                M[i][j] = score + M[i - 1][j - 1];
                P[i][j] = 2;
            }
            else
            { /// insertion/deletion

                int max = M[i - 1][j];
                char corrente = str1[i - 1];
                char precedente = str1[i - 2];

                P[i][j] = 0;
                if (max < M[i][j - 1])
                {
                    max = M[i][j - 1];
                    P[i][j] = 1;
                    char corrente = str2[j - 1];
                    char precedente = str2[j - 2];
                }
                costo_indel = -1;

                if (corrente == precedente && corrente == ' ')
                    costo_indel = 0;

                M[i][j] = max + costo_indel;
            }

            // /// calcolo costo sottostringa (con maiuscole/minuscole)
            // int match = 0;
            // if (str1[i - 1] == str2[j - 1])
            //     match = 1;
            // else {
            //     char temp1 = str1[i - 1];
            //     if (temp1 >= 'a' && temp1 <= 'z')
            //         temp1 -= abs('A' - 'a');
            //     char temp2 = str2[j - 1];
            //     if (temp2 >= 'a' && temp2 <= 'z')
            //         temp2 -= abs('A' - 'a');
            //     if (temp1 == temp2)
            //         match = 1;
            // }

            // /// sottosequenza
            // if (match) { // match
            //              /// M(i, j) ← 1 + M(i − 1, j − 1)
            //     M[i][j] = 1 + M[i - 1][j - 1];
            // } else { // mismatch
            //     // M(i, j) ← max(M(i − 1, j), M(i, j − 1))
            //     int max = M[i - 1][j];
            //     if (max < M[i][j - 1])
            //         max = M[i][j - 1];
            //     M[i][j] = max;
            // }

            /// sottostringa
            // if (match) { // match
            //              /// M(i, j) ← 1 + M(i − 1, j − 1)
            //     M[i][j] = 1 + M[i - 1][j - 1];
            // } else { // mismatch
            //     M[i][j] = 0;
            // }
        }

    for (int i = 0; i < s1 + 1; i++)
    {

        // if (i == 0) {
        //     printf("   ");
        //     for (int j = 0; j < s2 + 1; j++)
        //         printf("%d ", j);
        //     printf("\n");
        // }

        printf("%2d %c: ", i, i > 0 ? str1[i - 1] : ' ');

        for (int j = 0; j < s2 + 1; j++)
        {
            printf("%2d ", M[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < s1 + 1; i++)
    {

        // if (i == 0) {
        //     printf("   ");
        //     for (int j = 0; j < s2 + 1; j++)
        //         printf("%d ", j);
        //     printf("\n");
        // }

        printf("%2d %c: ", i, i > 0 ? str1[i - 1] : ' ');

        for (int j = 0; j < s2 + 1; j++)
        {
            printf("%d ", P[i][j]);
        }
        printf("\n");
    }

    i = s1;
    int j = s2;

    // ricostruzione sottosequenza
    while (i > 0 || j > 0)
    {
        // printf("posizione: %d %d\n", i, j);
        if (P[i][j] == 2)
        {
            printf("%c", str1[i - 1]);
            i--;
            j--;
        }
        else
        {
            // if M(i,j) = M(i−1,j) then i ← i−1 else j ← j−1
            printf("_");
            if (P[i][j] == 0)
                i--;
            else
                j--;
        }
    }
    printf("\n");
}
/// INIZIO CODICE CERINELLI

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

    //bufferReversedString[currentLength++]= file1[posizioneCarattereFinale - (i)]; //aggiungo ultimo carattere a mano perchè esce prima dal ciclo
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
    bool closedBracketFile1 = false;
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
    // for(int i =0;i<sizeFile1;i++)
    //     cout<<file1[i];
    longestCommonSubstring();
    cout<<endl<<endl<<endl<<endl<<endl;

    file1 = getStringFromFileWithCarrelReturn("file1.cpp", sizeFile1);
    file2 = getStringFromFileWithCarrelReturn("file2.cpp", sizeFile2);
    // for(int i =0;i<sizeFile1;i++)
    //     cout<<file1[i];

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