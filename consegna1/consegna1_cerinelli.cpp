#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <filesystem>
#include <bits/stdc++.h>
using namespace std;
const int RUN = 32;
// compilazione: g++ consegna1-loader.c
//

// Il programma carica il file data.txt contenente 100 righe con dati da ordinare in modo crescente
// ./a.out
// In output viene mostrato il numero di accessi in read alla memoria per eseguire il sorting di ciascuna riga

// Obiettivo:
// Creare un algoritmo di sorting che minimizzi la somma del numero di accessi per ogni sorting di ciascuna riga del file

int ct_swap = 0;
int ct_cmp = 0;
int ct_read = 0;

int max_dim = 0;
int ntests = 100;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

void print_array(int *A, int dim)
{
    for (int j = 0; j < dim; j++)
    {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void swap(int &a, int &b)
{
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
}

int partition(int *A, int p, int r)
{

    /// copia valori delle due meta p..q e q+1..r
    ct_read++;
    int x = A[r];
    int i = p - 1;

    for (int j = p; j < r; j++)
    {
        ct_cmp++;
        ct_read++;
        if (A[j] <= x)
        {
            i++;
            ct_read++;
            ct_read++;
            swap(A[i], A[j]);
        }
    }
    ct_read++;
    ct_read++;
    swap(A[i + 1], A[r]);

    return i + 1;
}

void quick_sort(int *A, int p, int r)
{
    /// gli array L e R sono utilizzati come appoggio per copiare i valori: evita le allocazioni nella fase di merge
    if (p < r)
    {
        int q = partition(A, p, r);
        quick_sort(A, p, q - 1);
        quick_sort(A, q + 1, r);
    }
}

int parse_cmd(int argc, char **argv)
{

    /// parsing argomento
    max_dim = 1000;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][1] == 'd')
            ndiv = atoi(argv[i] + 3);
        if (argv[i][1] == 't')
            ntests = atoi(argv[i] + 3);
        if (argv[i][1] == 'v')
            details = 1;
        if (argv[i][1] == 'g')
        {
            graph = 1;
            ndiv = 1;
            ntests = 1;
        }
    }

    return 0;
}

void insertionSort(int arr[], int left, int right)
{
    for (int i = left + 1; i <= right; i++)
    {
        ct_read++;
        int temp = arr[i];
        int j = i - 1;
        ct_read++;
        while (j >= left && arr[j] > temp)
        {
            ct_read++;
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

// CODICE CERINELLI
void merge(int arr[], int l, int m, int r)
{

    int len1 = m - l + 1, len2 = r - m;
    int left[len1], right[len2];
    ct_read++;
    ct_read++; // read 2 volte?

    for (int i = 0; i < len1; i++)
        left[i] = arr[l + i];
    ct_read++; // popolo left
    for (int i = 0; i < len2; i++)
        right[i] = arr[m + 1 + i];
    ct_read++; // popolo right

    int i = 0;
    int j = 0;
    int k = l;

    while (i < len1 && j < len2)
    { // Inizio comparing di left e right. se left è < aggiunto left e vado avanti di 1 col contatore e viceversa per right
        ct_read++;
        ct_read++;
        if (left[i] <= right[j])
        {
            ct_read++;
            arr[k] = left[i];
            i++;
        }
        else
        {
            ct_read++;
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < len1)
    { // se sono rimasti elementi di left li metto
        ct_read++;
        arr[k] = left[i];
        k++;
        i++;
    }

    while (j < len2)
    { // se sono rimasti elementi di right li metto
        ct_read++;
        arr[k] = right[j];
        k++;
        j++;
    }
}

// CODICE CERINELLI
void timSort(int arr[], int n)
{

    for (int i = 0; i < n; i += RUN)
        insertionSort(arr, i, min((i + RUN - 1), (n - 1))); // chiamo insertion sort per sotto array lunghi RUN (32) [/|/|/]

    for (int size = RUN; size < n; size = 2 * size)
    { // inizio merge, prima di 32, poi 64, 128 ecc

        for (int left = 0; left < n; left += 2 * size)
        { // parto dall'inizio della sezione "nuova", poi salto un "blocco" facendo 2*size

            int mid = left + size - 1;                       // trovo la metà
            int right = min((left + 2 * size - 1), (n - 1)); // trovo l'inizio del blocco successivo

            if (mid < right) // se mid è < inizio blocco 2 /|/ mergio /
                merge(arr, left, mid, right);
        }
    }
}

int main(int argc, char **argv)
{
    int i, test;
    int *A;
    int *B; /// buffer per visualizzazione algoritmo

    int *toSort;

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];
    toSort = new int[max_dim];

    n = max_dim;

    ifstream input_data;
    input_data.open("data.txt");

    int read_min = -1;
    int read_max = -1;
    long read_avg = 0;

    //// lancio ntests volte per coprire diversi casi di input random
    for (test = 0; test < ntests; test++)
    {

        /// inizializzazione array: numeri random con range dimensione array
        for (i = 0; i < n; i++)
        {
            char comma;
            input_data >> A[i];
            input_data >> comma;
        }

        if (details)
        {
            printf("caricato array di dimensione %d\n", n);
            print_array(A, n);
        }

        ct_swap = 0;
        ct_cmp = 0;
        ct_read = 0;

        // algoritmo di sorting
        // quick_sort(A, 0, n - 1);

        // CODICE CERINELLI
        for (i = 0; i < n / 2; i++)
        {
            /*
                visto che i numeri da ordinare sono una gaussiana la prima idea è creare un array è oranizzare in modo "crescente" i dati in input, in modo
                che siano già all'incirca ordinati in modo crescente. Ho pensato di farlo perchè se InsertionSort è O(N) nel caso in cui l'array sia già ordinato
                sicuramente esistono algoritmi che operano bene nel caso in cui l'array in input sia quasi già ordinato.

                La mia prima versione del codice era questa:
                        for (i = 0; i < n; i++) {
                            if(i%2==0){
                                toSort[i] = A[i];
                            }else{
                                toSort[i] = A[n-i];
                            }
                        }
                Ma non risolveva la gaussiana, semplicemente rendeva un po' rumoroso l'array.
                per popolarlo come scritto qui sotto ho chiesto a chatGpt
            */
            toSort[2 * i] = A[i];           // Posizioni pari: dall'inizio di A
            toSort[2 * i + 1] = A[999 - i]; // Posizioni dispari: dalla fine di A
            ct_read++;
            ct_read++;
        }

        timSort(toSort, n);

        // FINE CODICE CERINELLI

        if (details)
        {
            printf("Output:\n");
            print_array(toSort, n);
        }

        /// statistiche
        read_avg += ct_read;
        if (read_min < 0 || read_min > ct_read)
            read_min = ct_read;
        if (read_max < 0 || read_max < ct_read)
            read_max = ct_read;
        printf("Test %d %d\n", test, ct_read);
    }

    printf("N test: %d, Min: %d, Med: %.1f, Max: %d\n",
           ntests,
           read_min, (0.0 + read_avg) / ntests, read_max);

    delete[] A;
    cout << std::filesystem::current_path();

    return 0;
}