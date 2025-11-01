#include "ram.h"
#include <stdlib.h>


int* criaRam(int tam) {
    int* ram = malloc(tam*sizeof(int));
    if (ram == NULL)
        return NULL;
    return ram;
}

int* criaRam_vazia(int tam) {
    int* ram = criaRam(tam);
    if (ram == NULL)
        return NULL;

    for (int i = 0; i < tam; i++)
        ram[i] = 0;
    return ram;
}

int* criaRandomRam(int tam) {
    int* ram = criaRam(tam);
    if (ram == NULL)
        return NULL;
    
        for (int i = 0; i < tam; i++) 
            ram[i] = rand();

    return ram;
}

void store(int* ram, int pos, int dado) {
    ram[pos] = dado;
}

int load(int* ram, int pos) {
    return ram[pos];
}