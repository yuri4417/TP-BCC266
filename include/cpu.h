#ifndef CPU_H
#define CPU_H

#include "structs.h"


void cpu(Cache *L1, Cache *L2, Cache *L3, LinhaCache *RAM, BenchMetrics *metrics, WriteBuffer *buffer, 
        ConfigItem *configs, PilhaExecucao *pPilha, Instrucao *TI,int *hitsRam, int *missesRam);





#endif