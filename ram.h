#ifndef RAM_H
#define RAM_H

int* criaRam(int tam);
int* criaRam_vazia(int tam);
int* criaRandomRam(int tam);
void liberaRAM(int* ram);
void store(int* ram, int pos, int dado);
int load(int* ram, int pos);
void printRam(int* ram, int qtd);
void extraiRAM(int* RAM, int posRam, int *conteudo);
void salvaUmValor(int* RAM, int pos, int val);
void salvaDoisValores(int* RAM, int pos1, int val1, int pos2, int val2);
void auxMovPotencia(int *RAM);
#endif