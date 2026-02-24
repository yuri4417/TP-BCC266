
#include "pilha.h"
#include "structs.h"

#include <stdlib.h>


PilhaExecucao *criaPilha(int maxTam) {
    PilhaExecucao *ptr = (PilhaExecucao*) malloc(sizeof(PilhaExecucao));
    if (ptr) {
        ptr->itens = (ItemPilha*) malloc(sizeof(ItemPilha) * maxTam);
        if (!ptr->itens) {
            free(ptr);
            return NULL;
        }
        ptr->maxTam = maxTam;
        ptr->qtd = 0;
    }
    return ptr;
}

void destroiPilha(PilhaExecucao *pilha) {
    if (pilha) {
        free(pilha->itens);
        free(pilha);
    }
}

void PilhaPush(PilhaExecucao *pilha, ItemPilha item) {
    if (!pilha || !pilha->itens)
        return;

    if (pilha->qtd < pilha->maxTam) 
        pilha->itens[pilha->qtd++] = item;
}

void PilhaPop(PilhaExecucao *pilha) {
    if (!pilha || pilha->qtd == 0)
        return;

    pilha->qtd--;
}

void PilhaTopo(PilhaExecucao *pilha, ItemPilha *item) {
    if (!pilha || pilha->qtd == 0 || !item)
        return;

    *item = pilha->itens[pilha->qtd - 1];
}