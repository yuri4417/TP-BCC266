#include "ram.h"
#include "structs.h"
#include <stdlib.h>
#include <stdio.h>


LinhaCache* criaRAM(int tamanho) {
    LinhaCache *ptr = (LinhaCache*) calloc(tamanho, sizeof(LinhaCache));
    return ptr;
}

LinhaCache* criaRAM_aleatoria(int tamanho) {
    LinhaCache *ptr = (LinhaCache*) malloc(sizeof(LinhaCache) * tamanho);

    if (ptr) {
        for (int i = 0; i < tamanho; i++) {
            ptr[i].endBloco = i;
            ptr[i].preenchido = true;
            ptr[i].alterado = false;
            ptr[i].prioridade = 0;
            
            for (int j = 0; j < 4; j++)
                ptr[i].palavras[j] = rand();
        }
    }
    return ptr;
}

void liberaRAM(LinhaCache* ram) {
    if (ram)
        free(ram);
}

