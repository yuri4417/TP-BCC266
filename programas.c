#include "programas.h"
#include "ram.h"
#include "maquina.h"

#include <stdlib.h>

void programaAleatorio(int qtdeInstrucoes) {
    int tamRAM = 1000;
    int *RAM = criaRandomRam(tamRAM);
    Instrucao* programa = (Instrucao*) malloc(qtdeInstrucoes*sizeof(Instrucao));

    for (int i = 0; i < qtdeInstrucoes - 1; i++) {
        programa[i].opcode = rand() % 2;
        programa[i].endereco1 = rand() % (tamRAM + 1);
        programa[i].endereco2 = rand() % (tamRAM + 1);
        programa[i].endereco3 = rand() % (tamRAM + 1);
    }
    programa[qtdeInstrucoes-1].opcode = -1;

    maquina(RAM, programa);
}