#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "ram.h"
#include "menu.h"
#include "utils.h"
#include "structs.h"
#include "MMU.h"
#include "cores.h"
#include "pilha.h"

Instrucao* criarPadrao(int N_FOR, int N_OPCODE, int INICIO_RAM, int rangeMemoria, int N_WORD) {
    Instrucao* ptr = (Instrucao*) malloc(N_FOR * sizeof(Instrucao));
    if (!ptr) 
        exit(-1);

    for (int i = 0; i < N_FOR; i++) {
        ptr[i].opcode = rand() % N_OPCODE;

        ptr[i].add1.endBloco = INICIO_RAM + (rand() % rangeMemoria);
        ptr[i].add1.endPalavra = rand() % N_WORD;
        
        ptr[i].add2.endBloco = INICIO_RAM + (rand() % rangeMemoria);
        ptr[i].add2.endPalavra = rand() % N_WORD;
        
        ptr[i].add3.endBloco = INICIO_RAM + (rand() % rangeMemoria);
        ptr[i].add3.endPalavra = rand() % N_WORD;
    }
    return ptr;
}

Instrucao* geradorMultiplo(int N_INST, int N_PROB, int N_FOR, int N_OPCODE, int N_WORD) {
    Instrucao* programa = (Instrucao*) malloc((N_INST + 1) * sizeof(Instrucao));
    if (!programa)
        exit(-1);

    Instrucao* padraoA = criarPadrao(N_FOR, N_OPCODE, 0, 10, N_WORD);
    Instrucao* padraoB = criarPadrao(N_FOR, N_OPCODE, 200, 10, N_WORD);
    Instrucao* padraoC = criarPadrao(N_FOR, N_OPCODE, 400, 80, N_WORD);

    int indiceA = 0, indiceB = 0, indiceC = 0;

    for (int i = 0; i < N_INST; i++) {
        int random_prob = (rand() % 100) + 1;
        if (random_prob <= N_PROB) {
            int escolha = rand() % 100;
            if (escolha < 33) {
                programa[i] = padraoA[indiceA];
                indiceA = (indiceA + 1) % N_FOR;
            }
            else if (escolha < 66) {
                programa[i] = padraoB[indiceB];
                indiceB = (indiceB + 1) % N_FOR;
            }
            else {
                programa[i] = padraoC[indiceC];
                indiceC = (indiceC + 1) % N_FOR;
            }
        }
        else {
            int endAleatorios = 600;
            programa[i].opcode = rand() % N_OPCODE;
            
            programa[i].add1.endBloco = endAleatorios + (rand() % 400);
            programa[i].add1.endPalavra = rand() % N_WORD;
            
            programa[i].add2.endBloco = endAleatorios + (rand() % 400);
            programa[i].add2.endPalavra = rand() % N_WORD;
            
            programa[i].add3.endBloco = endAleatorios + (rand() % 400);
            programa[i].add3.endPalavra = rand() % N_WORD;
        }
    }

    free(padraoA);
    free(padraoB); 
    free(padraoC);

    programa[N_INST].opcode = -1; // HALT
    return programa;
}

Instrucao* gerarInstrucoes(int N_INST, int N_MEM, int N_PROB, int N_FOR, int N_OPCODE, int N_WORD) {

    Instrucao* programa = (Instrucao*) malloc((N_INST + 1) * sizeof(Instrucao));
    if (!programa)
        exit(-1);

    Instrucao* padrao_repeticao = criarPadrao(N_FOR, N_OPCODE, 0, N_MEM, N_WORD);

    int i = 0;
    while (i < N_INST) {
        int random_prob = (rand() % 100) + 1;

        if (random_prob <= N_PROB && (i + N_FOR) <= N_INST) {
            for (int j = 0; j < N_FOR; j++) {
                programa[i] = padrao_repeticao[j];
                i++;
            }
        } 
        else {
            programa[i].opcode = rand() % N_OPCODE;
            
            programa[i].add1.endBloco = rand() % N_MEM;
            programa[i].add1.endPalavra = rand() % N_WORD;
            
            programa[i].add2.endBloco = rand() % N_MEM;
            programa[i].add2.endPalavra = rand() % N_WORD;
            
            programa[i].add3.endBloco = rand() % N_MEM;
            programa[i].add3.endPalavra = rand() % N_WORD;
            
            i++;
        }
    }

    free(padrao_repeticao);
    programa[N_INST].opcode = -1; // HALT
    return programa;
}

void geraInterrupcao(int N_INTERRUPCAO, int PROB_INTERRUPCAO, int N_FOR, int N_OPCODE, int TAM_HD, int N_WORD) {
    FILE *file = fopen("TI.txt", "w");
    if (!file)
        return;

    Instrucao* padrao = criarPadrao(N_FOR, N_OPCODE, 0, TAM_HD, N_WORD);

    int i = 0;
    while (i < N_INTERRUPCAO) {
        fprintf(file, "%d %d %d %d %d %d %d\n", rand() % N_OPCODE,
                padrao[i % N_FOR].add1.endBloco, padrao[i % N_FOR].add1.endPalavra,
                padrao[i % N_FOR].add2.endBloco, padrao[i % N_FOR].add2.endPalavra,
                padrao[i % N_FOR].add3.endBloco, padrao[i % N_FOR].add3.endPalavra);
        i++;
    }
    fprintf(file, "-1 0 0 0 0 0 0\n"); // HALT
    fclose(file);
    free(padrao);
}

void setupBenchmark(BenchMetrics *metrics, ConfigItem *configs) {
    metrics->tamL1 = menu_valor("Tamanho da Cache L1 (em blocos)");
    metrics->tamL2 = menu_valor("Tamanho da Cache L2 (em blocos)");
    metrics->tamL3 = menu_valor("Tamanho da Cache L3 (em blocos)");
    if (configs[ID_BUFFER].ativo)
        metrics->tamWriteBuffer = menu_valor("Tamanho do WriteBuffer");

    metrics->relogio = 0;
    metrics->tamRAM = TAM_RAM_DEFAULT;

    if (configs[ID_INTERRUPCAO].ativo)
        metrics->PROB_INTERRUPCAO = menu_valor("Probabilidade de Interrupcao");


    metrics->N_PROB = menu_valor("Probabilidade de Repeticao");
    metrics->N_FOR = menu_valor("Numero de Instrucoes na Repeticao");
}

void CacheBenchmark(BenchMetrics *metrics, ConfigItem *configs) {
    Cache *L1 = criaCache(metrics->tamL1); Cache *L2 = criaCache(metrics->tamL2); Cache *L3 = criaCache(metrics->tamL3);
    LinhaCache *RAM = (configs[ID_INTERRUPCAO].ativo) ? criaRAM(metrics->tamRAM) : criaRAM_aleatoria(metrics->tamRAM);
 
    Instrucao *programa = (configs[ID_MULT].ativo) ? geradorMultiplo(10000, metrics->N_PROB, metrics->N_FOR, 2, 4) : 
                gerarInstrucoes(10000, metrics->tamRAM, metrics->N_PROB, metrics->N_FOR, 2, 4);


    WriteBuffer buffer;
    if (configs[ID_BUFFER].ativo) {
        buffer.fila = (ItemBuffer*) malloc(metrics->tamWriteBuffer * sizeof(ItemBuffer));
        buffer.inicio = 0;
        buffer.fim = 0;
        buffer.qtdAtual = 0;
        buffer.tamMax = metrics->tamWriteBuffer;
        buffer.ultimoUso = 0;
        buffer.custoPorStore = CUSTO_RAM; 
    } 
    else
        metrics->tamWriteBuffer = -1;
    
    if (configs[ID_LIP].ativo) 
        strcpy(metrics->policy, "LIP");
    else if (configs[ID_LFU].ativo)
        strcpy(metrics->policy, "LFU");
    else if (configs[ID_RRIP].ativo)
        strcpy(metrics->policy, "SRRIP");
    else
        strcpy(metrics->policy, "LRU");

    Instrucao *TI = NULL;
    if (configs[ID_INTERRUPCAO].ativo) {
        FILE *pFile = fopen("TI.txt", "r");
        int i = 0;
        while (!feof(pFile)) {
            fscanf(pFile, "%d %d %d %d %d %d %d", &TI[i].opcode, &TI[i].add1.endBloco, &TI[i].add1.endPalavra, 
                &TI[i].add2.endBloco, &TI[i].add2.endPalavra, &TI[i].add3.endBloco, &TI[i].add3.endPalavra);
            i++;
        }
        fclose(pFile);
    }

    PilhaExecucao *pPilha = criaPilha(metrics->qtdInterrupcao);
    PilhaPush(pPilha, (ItemPilha){programa, 0});

    cpu(L1, L2, L3, RAM, &metrics, &buffer, configs, pPilha, TI);


    metrics->hitsL1 = L1->hit; metrics->missesL1 = L1->miss;
    metrics->hitsL2 = L2->hit; metrics->missesL2 = L2->miss;
    metrics->hitsL3 = L3->hit; metrics->missesL3 = L3->miss;



    destroiCache(L1); destroiCache(L2); destroiCache(L3); 
    liberaRAM(RAM);
    destroiPilha(pPilha);
    free(programa);
    if (configs[ID_BUFFER].ativo)
        free(buffer.fila);
    if (configs[ID_INTERRUPCAO].ativo)
        free(TI);
}

/*
void testeRapido(ConfigItem *configs) {
    BenchMetrics m;
    
    menu_close(); 


    m.tamL1 = 32; 
    m.tamL2 = 64;
    m.tamL3 = 256;
    m.tamRAM = TAM_RAM_DEFAULT;
    
    m.N_PROB = 90;    
    m.N_FOR = 5;  
    m.tamWriteBuffer = 4;

    configs[ID_MULT].ativo = 1; 

    printf("\n" BOLD(CYAN("=== Comparacão de Políticas  (L1=%d, MULTI=ON) ===")) "\n", m.tamL1);

    configs[ID_LIP].ativo = 0; 
    configs[ID_LFU].ativo = 0; 
    configs[ID_RRIP].ativo = 0;
    
    CacheBenchmark(&m, configs);
    printf("LRU  -> Hit L1: %5.1f%% (Tempo: %ld)\n", (float)m.hitsL1*100/(m.hitsL1+m.missesL1), m.relogio);

    configs[ID_LIP].ativo = 1; 
    configs[ID_LFU].ativo = 0; 
    configs[ID_RRIP].ativo = 0;
    
    CacheBenchmark(&m, configs);
    printf("LIP  -> Hit L1: %5.1f%% (Tempo: %ld)\n", (float)m.hitsL1*100/(m.hitsL1+m.missesL1), m.relogio);

    configs[ID_LIP].ativo = 0; 
    configs[ID_LFU].ativo = 1; 
    configs[ID_RRIP].ativo = 0;
    
    CacheBenchmark(&m, configs);
    printf("LFU  -> Hit L1: %5.1f%% (Tempo: %ld)\n", (float)m.hitsL1*100/(m.hitsL1+m.missesL1), m.relogio);

    configs[ID_LIP].ativo = 0; 
    configs[ID_LFU].ativo = 0; 
    configs[ID_RRIP].ativo = 1;
    
    CacheBenchmark(&m, configs);
    printf("RRIP -> Hit L1: %5.1f%% (Tempo: %ld)\n", (float)m.hitsL1*100/(m.hitsL1+m.missesL1), m.relogio);
    
    printf("\n" GREEN("Pressione ENTER para retornar ao menu...") "\n");
    getchar();
    iniciar_menu(); 
}
*/

