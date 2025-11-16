#include "ram.h"
#include <stdlib.h>
#include <stdio.h>


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

void liberaRAM(int* ram) {
    if (ram)
        free(ram);
}

void store(int* ram, int pos, int dado) {
    ram[pos] = dado;
}

int load(int* ram, int pos) {
    return ram[pos];
}

void printRam(int* ram, int qtd) {
    printf("RAM-> [");
    for(int i = 0; i < qtd; i++) {
        printf("%d", ram[i]);
        if (i < qtd -1)
            printf(", ");
    }
    printf("]\n");
}