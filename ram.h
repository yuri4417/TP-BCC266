#ifndef RAM_H
#define RAM_H

int* criaRam(int tam);
int* criaRam_vazia(int tam);
int* criaRandomRam(int tam);
void liberaRAM(int* ram);
void store(int* ram, int pos, int dado);
int load(int* ram, int pos);
void printRam(int* ram, int qtd);
#endif