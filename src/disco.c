#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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

void salvaHD(LinhaCache *dado, int endHd, FILE *file)
{
    int arqAberto = (file != NULL);
    if(!arqAberto)
        file = fopen(nomeHD, "rb+");

    if (dado->alterado) {
        fseek(file, endHd * sizeof(LinhaCache), SEEK_SET);
        fwrite(dado, sizeof(LinhaCache), 1 , file);
    }
    if (!arqAberto)
        fclose(file);
}

int transfereHD(LinhaCache* RAM, int endHD, long *relogio, ConfigItem *configs, double *tempoHD) {
    struct timespec inicio, fim;
    int posVazia = -1, posEncontrado = -1;
    
    buscarRAM(RAM, endHD, *relogio, configs, &posEncontrado, &posVazia);

    if (posEncontrado != -1) //Bloco já está na ram
        return posEncontrado;

    
    if (posVazia != -1) { //Pos vazia na ram, basta inserir
        clock_gettime(CLOCK_MONOTONIC, &inicio); 
        
        FILE *file = fopen(nomeHD, "rb");
        if (!file)
            return -1;
        fseek(file, endHD * sizeof(LinhaCache), SEEK_SET);
        LinhaCache linha;
        fread(&linha, sizeof(LinhaCache), 1, file);
        fclose(file);
        
        clock_gettime(CLOCK_MONOTONIC, &fim); 
        
        *tempoHD += (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
        RAM[posVazia] = linha;
        return posVazia;
    }
        
    // RAM Cheia, substituicao
    int posMenorPrioridade = 0;
    for (int i = 0; i < TAM_RAM_DEFAULT; i++) {
        if (RAM[i].prioridade < RAM[posMenorPrioridade].prioridade)
            posMenorPrioridade = i;
    }
    
    int endRAM = RAM[posMenorPrioridade].endBloco;

    clock_gettime(CLOCK_MONOTONIC, &inicio); 
    
    FILE *file = fopen(nomeHD, "rb+");
    if (!file)        
        return -1;
    

    if (RAM[posMenorPrioridade].alterado) 
        salvaHD(&RAM[posMenorPrioridade], endRAM, file);

    // Leitura novo bloco
    fseek(file, endHD * sizeof(LinhaCache), SEEK_SET);
    LinhaCache linha;
    fread(&linha, sizeof(LinhaCache), 1, file);
    fclose(file);
    
    clock_gettime(CLOCK_MONOTONIC, &fim);
    
    *tempoHD += (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    linha.prioridade = *relogio;
    RAM[posMenorPrioridade] = linha;
    return posMenorPrioridade;
}