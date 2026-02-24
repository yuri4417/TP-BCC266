
#ifndef RAM_H
#define RAM_H
#include <stdio.h>
#include "structs.h"
#include <stdbool.h>

LinhaCache* criaRAM(int tamanho);
LinhaCache* criaRAM_aleatoria(int tamanho);
void liberaRAM(LinhaCache* ram);


#endif