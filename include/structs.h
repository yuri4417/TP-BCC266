
#ifndef STRUCTS_H
#define STRUCTS_H

#include <time.h>
#include <stdbool.h>


#define TAM_RAM_DEFAULT 1000
#define TAM_HD_DEFAULT 10000
//Custos hipoteticos conforme a hierarquia 
#define CUSTO_L1 10
#define CUSTO_L2 30
#define CUSTO_L3 100
#define CUSTO_RAM 600
#define CUSTO_HD 10000

#define POL_LRU 0
#define POL_LIP 1
#define POL_LFU 2
#define POL_RRIP 3

#define ID_BUFFER 0
#define ID_LIP 1
#define ID_LFU 2
#define ID_RRIP 3  
#define ID_MULT 4
#define ID_INTERRUPCAO 5

#define nomeHD "HD.bin"

typedef struct { 
    long int relogio;
    long qtdStalls;
    int tamL1, tamL2, tamL3,tamWriteBuffer;
    int hitsL1, missesL1;
    int hitsL2, missesL2;
    int hitsL3, missesL3;
    int hitsRAM, missesRAM;
    char policy[5];

    int qtdInterrupcao;
    int PROB_INTERRUPCAO;
    int N_PROB;
    int N_FOR;
} BenchMetrics;

typedef struct {
    char *texto;
    int ativo;
    int flagBotao;    // 0 = Checkbox, 1 = Botão
} ConfigItem;


typedef struct {
    int palavras[4];
    int endBloco;

    long int prioridade;
    bool preenchido;
    bool alterado;

} LinhaCache;

typedef struct {
    LinhaCache *memoria;
    int nroConjuntos;

    long int hit;
    long int miss;
} Cache;

typedef struct {
    int endBloco;
    int endPalavra;
} Endereco;

typedef struct {
    int opcode;
    Endereco add1;
    Endereco add2;
    Endereco add3;
} Instrucao;

typedef struct {
    int endBloco;
    LinhaCache dado;
} ItemBuffer;

typedef struct {
    ItemBuffer *fila;
    int tamMax;
    int qtdAtual;

    int inicio, fim;

    long int ultimoUso;
    int custoPorStore;

} WriteBuffer;


typedef struct {
    Instrucao* programa;
    int PC;
} ItemPilha;

typedef struct PilhaExecucao {
    ItemPilha* itens;
    int maxTam;
    int qtd;
} PilhaExecucao;


#endif
