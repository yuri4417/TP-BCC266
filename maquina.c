#include "maquina.h"
#include "ram.h"
#include <stdlib.h>
#include <stdio.h>


void maquina(int *ram, Instrucao* programa) {
    int reg1, reg2;
    int PC = 0;
    int opcode = 0;
    while (opcode != -1) {

        opcode = programa[PC].opcode;

        switch (opcode) {
            
            // 0 -> soma
            // 1 -> subtracao
            
            case 0:
                reg1 = load(ram, programa[PC].endereco1);
                reg2 = load(ram, programa[PC].endereco2);
                reg1 += reg2;
                store(ram, programa[PC].endereco3, reg1);
                printf("Resultado %d salvo no endereço %d\n", reg1, programa[PC].endereco3);
                break;
            case 1:
                reg1 = load(ram, programa[PC].endereco1);
                reg2 = load(ram, programa[PC].endereco2);
                reg1 -= reg2;
                store(ram, programa[PC].endereco3, reg1);
                printf("Resultado %d salvo no endereço %d\n", reg1, programa[PC].endereco3);
                break;

        }
        PC++;
    }
}