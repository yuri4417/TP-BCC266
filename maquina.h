#ifndef MAQUINA_H
#define MAQUINA_H

typedef struct {
    int opcode;
    int endereco1;
    int endereco2;
    int endereco3;
} Instrucao;

void maquina(int *ram, Instrucao *programa);
#endif