#ifndef RAM_H
#define RAM_H

int* criaRam(int tam);
int* criaRam_vazia(int tam);
int* criaRandomRam(int tam);
void store(int* ram, int pos, int dado);
int load(int* ram, int pos);

#endif