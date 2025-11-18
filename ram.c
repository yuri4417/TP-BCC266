#include "ram.h"
#include <stdlib.h>
#include <stdio.h>
#include "maquina.h"

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
void extraiRAM(int* RAM, int posRam, int *conteudo) {
    
    Instrucao moveReg[3];
    moveReg[0].opcode = 3;
    moveReg[0].endereco1 = 1;
    moveReg[0].endereco2 = posRam;

    moveReg[1].opcode = 5;
    moveReg[1].endereco1 = 1;
    moveReg[1].endereco2 = -1;

    moveReg[2].opcode = -1;
    maquina(RAM, moveReg);
    *conteudo = moveReg[1].endereco2;
}
void salvaUmValor(int* RAM, int pos, int val) {
    Instrucao moveReg[3];
    moveReg[0].opcode = 4;
    moveReg[0].endereco1 = 1;
    moveReg[0].endereco2 = val;

    moveReg[1].opcode = 2;
    moveReg[1].endereco1 = 1;
    moveReg[1].endereco2 = pos;

    moveReg[2].opcode = -1;
    maquina(RAM, moveReg);
}

void salvaDoisValores(int* RAM, int pos1, int val1, int pos2, int val2) {

    Instrucao moveReg[5];

    moveReg[0].opcode = 4;
    moveReg[0].endereco1 = 1;      
    moveReg[0].endereco2 = val1;    

    moveReg[1].opcode = 4;
    moveReg[1].endereco1 = 2;       
    moveReg[1].endereco2 = val2;    

    moveReg[2].opcode = 2;
    moveReg[2].endereco1 = 1;       
    moveReg[2].endereco2 = pos1;    

    moveReg[3].opcode = 2;
    moveReg[3].endereco1 = 2;       
    moveReg[3].endereco2 = pos2;    


    moveReg[4].opcode = -1;
    maquina(RAM, moveReg);
}

void auxMovPotencia(int *RAM)
{
    Instrucao move2[5];
    move2[0].opcode = 3;
    move2[0].endereco1 = 1;
    move2[0].endereco2 = 0;// RAM[0] -> reg1
    
    move2[1].opcode = 3;
    move2[1].endereco1 = 2;
    move2[1].endereco2 = 1;// RAM[2] -> reg2
    
    move2[2].opcode = 5;
    move2[2].endereco1 = 1;// irá levar o reg1 para a posicao 3 do endereço 2 lá na máquina, que irá representar o expoente
    move2[2].endereco2 = -1;

    move2[3].opcode =5;
    move2[3].endereco1 = 2;
    move2[3].endereco2 = -1;

    move2[4].opcode = -1; 
    maquina(RAM, move2);
}
