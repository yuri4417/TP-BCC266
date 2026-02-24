#ifndef DISCO_H
#define DISCO_H
#include "structs.h"
#include "stdbool.h"

bool criaHd();
void salvaHD(LinhaCache *cache, int endHd, FILE *file);
int transfereHD(LinhaCache* RAM, int endHD, long *relogio, ConfigItem *configs);

#endif 