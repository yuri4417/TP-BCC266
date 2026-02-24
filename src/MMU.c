
#include "ram.h"
#include "structs.h"
#include "disco.h"
#include "MMU.h"

#include "limits.h"
#include <stdlib.h>


Cache* criaCache(int tamanhoTotal) {
    Cache *cache = (Cache*) malloc(sizeof(Cache));
    if (cache) {
        cache->memoria = (LinhaCache*) malloc(sizeof(LinhaCache) * tamanhoTotal);
        if (!cache->memoria) {
            free(cache);
            return NULL; 
        }

        cache->nroConjuntos = (tamanhoTotal / 4 > 0) ? (tamanhoTotal / 4) : 1;
        cache->hit = 0;
        cache->miss = 0;

        for (int i = 0; i < tamanhoTotal; i++) {
            cache->memoria[i].preenchido = false;
            cache->memoria[i].alterado = false;
            cache->memoria[i].prioridade = 0;
            cache->memoria[i].endBloco = -1;
        }
        
    }
    return cache;
}

void destroiCache(Cache* c) {
    if (c) {
        if (c->memoria)
            free(c->memoria);
        free(c);
    }
}

int getPolicy(ConfigItem *configs) {
    if (configs[ID_LFU].ativo)
        return POL_LFU;
    if (configs[ID_LIP].ativo)
        return POL_LIP;
    if (configs[ID_RRIP].ativo) 
        return POL_RRIP;
    return POL_LRU; 
}

long atribuiPrioridade(ConfigItem *configs, long *relogioAtual) {
    switch (getPolicy(configs)) {
    case POL_LRU:
        return *relogioAtual;
        break;
    case POL_LIP:
        return 0;
        break;
    case POL_LFU:
        return 1;
        break;
    case POL_RRIP:
        return 2;
        break;
    }
    return -1;
}

void buffer_add(LinhaCache *RAM, WriteBuffer *wb, int endBloco, LinhaCache linha, long *relogio);
void salvaRAM(Cache* L3, int indice, LinhaCache *RAM, WriteBuffer *buffer, ConfigItem *configs, long int *relogio);

void buscarRAM(LinhaCache *RAM, int endHD, long int relogioAtual, ConfigItem *configs, int *posEncontrado, int *posVazia) {
    for (int i = 0; i < TAM_RAM_DEFAULT; i++) {
        if (RAM[i].preenchido && RAM[i].endBloco == endHD) {
            if (configs[ID_LFU].ativo) 
                RAM[i].prioridade++; 
            else if (configs[ID_RRIP].ativo)
                RAM[i].prioridade = 0;
            else
                RAM[i].prioridade = relogioAtual; //LRU / LIP
            
            *posEncontrado = i;
            return;
        }
        else if (*posVazia == -1 && !RAM[i].preenchido) 
            *posVazia = i;
    }
    *posEncontrado = -1;
}

int buscarCache(Cache* c, int endRAM, long int relogioAtual, ConfigItem *configs) {
    int idxConjunto = endRAM % c->nroConjuntos;

    int inicioBloco = idxConjunto * 4;
    for (int i = inicioBloco; i < inicioBloco + 4; i++) { // Caso encontre o endereços da ram na cache vai aumentar o número de hits
        if (c->memoria[i].preenchido && c->memoria[i].endBloco == endRAM) {
            c->hit++;
            
            if (configs[ID_LFU].ativo) 
                c->memoria[i].prioridade++; 
            else if (configs[ID_RRIP].ativo)
                c->memoria[i].prioridade = 0;
            else
                c->memoria[i].prioridade = relogioAtual; //LRU / LIP
            
            return i;
        }
    }

    c->miss++;
    return -1;
}

int buscaDadoAntigo(Cache* c, int endRAM, int policy) {
    int idxConjunto = endRAM % c->nroConjuntos;
    int inicioBloco = idxConjunto * 4;

    // busca por linha vazia primeiro
    for (int i = inicioBloco; i < inicioBloco + 4; i++) 
        if (!c->memoria[i].preenchido)
            return i;
    
    if (policy == POL_RRIP) {
        while (1) { 
            for (int i = inicioBloco; i < inicioBloco + 4; i++) //busca rrpv = 3
                if (c->memoria[i].prioridade >= 3) 
                    return i;

            for (int i = inicioBloco; i < inicioBloco + 4; i++) //incrementa rrpv
                if (c->memoria[i].prioridade < 3)
                    c->memoria[i].prioridade++;
        }
    }

    // LRU, LIP, LFU    
    int posMenorPrioridade = -1;
    long int menorPrioridade = LONG_MAX;

    for (int i = inicioBloco; i < inicioBloco + 4; i++) {  
        if (c->memoria[i].prioridade < menorPrioridade) {// vai percorrer todo o conjunto do end e acessar o dado que tem o menor valor no relogio
            menorPrioridade = c->memoria[i].prioridade;
            posMenorPrioridade = i;
        }
    }

    return posMenorPrioridade;

}

int transfereCache(Cache *cacheOrigem, Cache *cacheDestino, int indiceOrigem, int endRAM, long int *relogio, LinhaCache *RAM, WriteBuffer *buffer, ConfigItem *configs, Cache *cacheInferior) {
    
    LinhaCache dadoSobe = cacheOrigem->memoria[indiceOrigem];
    cacheOrigem->memoria[indiceOrigem].preenchido = false;

    int posDestino = buscaDadoAntigo(cacheDestino, endRAM, getPolicy(configs));

    if (cacheDestino->memoria[posDestino].preenchido) { //swap entre dados das caches
        LinhaCache dadoDesce = cacheDestino->memoria[posDestino];
        int destinoParaOrigem = buscaDadoAntigo(cacheOrigem, dadoDesce.endBloco, getPolicy(configs));

        if (cacheInferior) { //transferencia L1 -> L2, dado que desce da L1 expulsa dado para L3
            LinhaCache vitimaL2 = cacheOrigem->memoria[destinoParaOrigem];

            if (vitimaL2.preenchido) {
                int posL3 = buscaDadoAntigo(cacheInferior, vitimaL2.endBloco, getPolicy(configs));
                
                salvaRAM(cacheInferior, posL3, RAM, buffer, configs, relogio);

                cacheInferior->memoria[posL3] = vitimaL2;
                cacheInferior->memoria[posL3].prioridade = atribuiPrioridade(configs, relogio);
                cacheInferior->memoria[posL3].preenchido = true;
            }
        }
        else // Transferencia L3 -> L2, dado que desce da L2 expulsa dado para RAM
            salvaRAM(cacheOrigem, destinoParaOrigem, RAM, buffer, configs, relogio);

        cacheOrigem->memoria[destinoParaOrigem] = dadoDesce;
        cacheOrigem->memoria[destinoParaOrigem].prioridade = atribuiPrioridade(configs, relogio);
        cacheOrigem->memoria[destinoParaOrigem].preenchido = true;
    }

    cacheDestino->memoria[posDestino] = dadoSobe;
    cacheDestino->memoria[posDestino].prioridade = atribuiPrioridade(configs, relogio);
    cacheDestino->memoria[posDestino].preenchido = true;

    return posDestino;
}




void buffer_add(LinhaCache *RAM, WriteBuffer *wb, int endBloco, LinhaCache linha, long *relogio) {

    // Background
    long delta = *relogio - wb->ultimoUso;
    if (delta > 0) {
        long qtdStores = delta / wb->custoPorStore;

        if (qtdStores > 0 && wb->qtdAtual > 0) {
            int endRAM = wb->fila[wb->inicio].endBloco;
            RAM[endRAM] = wb->fila[wb->inicio].dado;

            wb->inicio = (wb->inicio + 1) % wb->tamMax;
            wb->qtdAtual--;
            wb->ultimoUso += wb->custoPorStore;
        }
    }

    if (wb->qtdAtual == 0)
        wb->ultimoUso = *relogio;
    
    // busca
    if (wb->qtdAtual > 0) {
        int i = wb->inicio;
        int count = 0;
        while (count < wb->qtdAtual) {
            if (wb->fila[i].endBloco == endBloco) {
                wb->fila[i].dado = linha;
                return;
            }
            i = (i + 1) % wb->tamMax;
            count++;
        }
    }

    // stalls
    while (wb->qtdAtual >= wb->tamMax) {
        *relogio += wb->custoPorStore;
        wb->ultimoUso = *relogio;
        
        int endRAM = wb->fila[wb->inicio].endBloco;
        RAM[endRAM] = wb->fila[wb->inicio].dado;

        wb->qtdAtual--;
        wb->inicio = (wb->inicio + 1) % wb->tamMax;

    }

    // Add buffer
    wb->fila[wb->fim].endBloco = endBloco;
    wb->fila[wb->fim].dado = linha;

    wb->fim = (wb->fim + 1) % wb->tamMax;
    wb->qtdAtual++;
}

void salvaRAM(Cache* L3, int indice, LinhaCache *RAM, WriteBuffer *buffer, ConfigItem *configs, long int *relogio) {
    if(!L3 || !RAM)
        return;
    if (L3->memoria[indice].preenchido && L3->memoria[indice].alterado) {
        int blocoID = L3->memoria[indice].endBloco;

        int posEncontrada = -1, posVazia = -1;
        buscarRAM(RAM, blocoID, *relogio, configs, &posEncontrada, &posVazia);

        if (posEncontrada != -1) {
            if (configs[ID_BUFFER].ativo) {
                buffer_add(RAM, buffer, posEncontrada, L3->memoria[indice], relogio); 
            } else {
                RAM[posEncontrada] = L3->memoria[indice]; 
                *relogio += CUSTO_RAM;
            }
        }
        L3->memoria[indice].preenchido = false;
        L3->memoria[indice].alterado = false;
        }      
}

int procuraBuffer(WriteBuffer *wb, int endBloco, LinhaCache *linha) {
    if (!wb || wb->qtdAtual == 0)
        return 0;

    int i = wb->inicio;
    int achou = 0;

    for (int cnt = 0; cnt < wb->qtdAtual; cnt++) {
        if (wb->fila[i].endBloco == endBloco) {// procura se já está no buffer
            *linha = wb->fila[i].dado;
            achou = 1;
            break;
        }
        i = (i + 1) % wb->tamMax;
    }
    return achou;
}

int carregaRAM(Cache* L3, int blocoID, LinhaCache *RAM, WriteBuffer *buffer, ConfigItem *configs, long int *relogio) { // Da RAM pra cache l3
    if(!RAM || !L3)
        return -1;

    int indiceAntigo = buscaDadoAntigo(L3, blocoID, getPolicy(configs)); //pega qual valor não está sendo usado para passar pra RAM
    
    salvaRAM(L3, indiceAntigo, RAM, buffer, configs, relogio);

    int posEncontrada = -1, posVazia = -1;
    buscarRAM(RAM, blocoID, *relogio, configs, &posEncontrada, &posVazia);
    if (posEncontrada == -1)
        return -1;

    LinhaCache linhaNova;
    int achouBuffer = 0;

    if (configs[ID_BUFFER].ativo) // quando writebuffer ativado vai buscar para ver se o endereco está no buffer
        achouBuffer = procuraBuffer(buffer, blocoID, &linhaNova);
    
    if (!achouBuffer) { 
        linhaNova = RAM[posEncontrada];
        *relogio += CUSTO_RAM;
    }

    L3->memoria[indiceAntigo] = linhaNova;
    L3->memoria[indiceAntigo].endBloco = blocoID;
    L3->memoria[indiceAntigo].preenchido = true;
    L3->memoria[indiceAntigo].alterado = false;
    L3->memoria[indiceAntigo].prioridade = atribuiPrioridade(configs, relogio);

    return indiceAntigo;
}



int moveL1(Endereco add, Cache *L1, Cache *L2, Cache *L3, LinhaCache *RAM, WriteBuffer *buffer, long int *relogio, ConfigItem *configs, int *hitsRam, int *missesRam) { // vai buscar o conteúdo nas caches ou na ram, e onde encontrar vai transferir pra l1
    // Já está na L1
    *relogio += CUSTO_L1;
    int pos = buscarCache(L1, add.endBloco, *relogio, configs);
    if (pos != -1) 
        return pos;

    // L2 -> L1
    *relogio += CUSTO_L2;
    int posL2 = buscarCache(L2, add.endBloco, *relogio, configs);
    if (posL2 != -1) 
        return transfereCache(L2, L1, posL2, add.endBloco, relogio, RAM, buffer, configs, L3);


    // L3 -> L2 -> L1
    *relogio += CUSTO_L3;
    int posL3 = buscarCache(L3, add.endBloco, *relogio, configs);
    if (posL3 != -1) {
        int novoposL2 = transfereCache(L3, L2, posL3, add.endBloco, relogio, RAM, buffer, configs, NULL);
        return transfereCache(L2, L1, novoposL2, add.endBloco, relogio, RAM, buffer, configs, L3);
    }

    // RAM -> L3 -> L2 -> L1
    *relogio += CUSTO_RAM;
    int posEncontrado, posVazia;
    buscarRAM(RAM, add.endBloco, *relogio, configs, &posEncontrado, &posVazia);
    if (posEncontrado != -1) {
        (*hitsRam)++;
        int novoposL3 = carregaRAM(L3, add.endBloco, RAM, buffer, configs, relogio);
        int novoposL2 = transfereCache(L3, L2, novoposL3, add.endBloco, relogio, RAM, buffer, configs, NULL);
        return transfereCache(L2, L1, novoposL2, add.endBloco, relogio, RAM, buffer, configs, L3);
    }

    // HD -> RAM -> L3 -> L2 -> L1
    // *relogio += CUSTO_HD;
    (*missesRam)++;
    int posRAM = transfereHD(RAM, add.endBloco, relogio, configs);
    if (posRAM != -1) {
        int novoposL3 = carregaRAM(L3, add.endBloco, RAM, buffer, configs, relogio);
        int novoposL2 = transfereCache(L3, L2, novoposL3, add.endBloco, relogio, RAM, buffer, configs, NULL);
        return transfereCache(L2, L1, novoposL2, add.endBloco, relogio, RAM, buffer, configs, L3);
    }



    return -1;
}

LinhaCache MMU_Read(Endereco add, Cache *L1, Cache *L2, Cache *L3, LinhaCache *RAM, WriteBuffer *buffer, long int *relogio, ConfigItem *configs, int *hitsRam, int *missesRam) {
    int posL1 = moveL1(add, L1, L2, L3, RAM, buffer, relogio, configs, hitsRam, missesRam);

    if (posL1 != -1)
        return L1->memoria[posL1];
    else
        exit(-1);

}

void MMU_Write(Cache *L1, Cache *L2, Cache *L3, LinhaCache *RAM, WriteBuffer *buffer, Endereco add, int valor, long int *relogio, ConfigItem *configs, int *hitsRam, int *missesRam) {
    int endpalavra = add.endPalavra;

    int posL1 = moveL1(add, L1, L2, L3, RAM, buffer, relogio, configs, hitsRam, missesRam);
    if (posL1 != -1) {
        L1->memoria[posL1].palavras[endpalavra] = valor;
        L1->memoria[posL1].alterado = true;
        L1->memoria[posL1].preenchido = true;
        
        // Escrita no dado considerada como acesso/hit
        if (configs[ID_LFU].ativo) 
            L1->memoria[posL1].prioridade++;  
        else if (configs[ID_RRIP].ativo)
            L1->memoria[posL1].prioridade = 0; 
        else 
            L1->memoria[posL1].prioridade = *relogio; // LRU/LIP
    }
    else
        exit(-1);
}


