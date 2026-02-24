#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "disco.h"
#include "MMU.h"

bool criaHd()
{
    FILE *file = fopen(nomeHD, "wb");
    if(!file)
        return false;
    
    LinhaCache *linhaHd = calloc(1, sizeof(LinhaCache));
    if(!linhaHd) {
        fclose(file);
        return false;
    }
    for (int i = 0; i < TAM_HD_DEFAULT; i++) {
        linhaHd->endBloco = i;
        linhaHd->preenchido = true;
        linhaHd->alterado = false;
        linhaHd->prioridade = 0;
        for (int j = 0; j < 4; j++)
            linhaHd->palavras[j] = rand();
    
    
        fwrite(linhaHd, 1, sizeof(LinhaCache), file);
    }
        
        fclose(file);
        free(linhaHd);
        return true;
}

void salvaHD(LinhaCache *cache, int endHd, FILE *file)
{
    int arqAberto = (file != NULL);
    if(!arqAberto)
        file = fopen(nomeHD, "rb+");
    if (cache->alterado) {
        fseek(file, endHd * sizeof(LinhaCache), SEEK_SET);
        fwrite(cache, sizeof(LinhaCache), 1 , file);
    }
    if (!arqAberto)
        fclose(file);
}

int transfereHD(LinhaCache* RAM, int endHD, long *relogio, ConfigItem *configs) { //HD -> RAM
    int posVazia = -1, posEncontrado = -1;
    buscarRAM(RAM, endHD, *relogio, configs, &posEncontrado, &posVazia);
    if (posEncontrado != -1)
        return posEncontrado;
    else if (posVazia != -1) {
        FILE *file = fopen(nomeHD, "rb");
        if (!file)
            return -1;
        fseek(file, endHD * sizeof(LinhaCache), SEEK_SET);
        LinhaCache linha;
        fread(&linha, sizeof(LinhaCache), 1, file);
        fclose(file);

        RAM[posVazia] = linha;
        return posVazia;
    }
        
    int posMenorPrioridade = 0;
    for (int i = 0; i < TAM_RAM_DEFAULT; i++) 
        if (RAM[i].prioridade < RAM[posMenorPrioridade].prioridade)
            posMenorPrioridade = i;
    
    int endRAM = RAM[posMenorPrioridade].endBloco;

    FILE *file = fopen(nomeHD, "rb+");
    if (!file)        
        return -1;
    if (posMenorPrioridade != -1)
        salvaHD(&RAM[posMenorPrioridade], endRAM, file);


    fseek(file, endHD * sizeof(LinhaCache), SEEK_SET);
    LinhaCache linha;
    fread(&linha, sizeof(LinhaCache), 1, file);
    fclose(file);
    linha.prioridade = *relogio;
    RAM[posMenorPrioridade] = linha;
    return posMenorPrioridade;
}