#include "structs.h"
#include "cpu.h"
#include "ram.h"
#include "pilha.h"

#include "MMU.h"


void cpu(Cache *L1, Cache *L2, Cache *L3, LinhaCache *RAM, BenchMetrics *metrics, WriteBuffer *buffer,
        ConfigItem *configs, PilhaExecucao *pPilha, Instrucao *TI, int *hitsRam, int *missesRam) {
    LinhaCache reg1, reg2, reg3;
    ItemPilha contextoAtual;
    int PC = 0; 
    int opcode;
    if (pPilha->qtd > 0) {
        PilhaTopo(pPilha, &contextoAtual);
        PC = contextoAtual.PC;
    }
    while (pPilha->qtd > 0) {

        Instrucao atual = contextoAtual.programa[PC];
        opcode = atual.opcode;
        (metrics->relogio)++;
        switch (opcode) {
            case -1: // HALT
                if (configs[ID_INTERRUPCAO].ativo) {
                    int nivelSaindo = pPilha->qtd;
                    PilhaPop(pPilha); // Remove o contexto atual

                    if (pPilha->qtd > 0) {
                        int pcAntigo = PC;
                        PilhaTopo(pPilha, &contextoAtual);
                        PC = contextoAtual.PC;
                        printf("\nHALT no Nivel %d - %d Instrucoes Executadas. Retornando para Nivel %d (PC %d)\n", nivelSaindo, pcAntigo, pPilha->qtd, PC);
                    } else {
                        printf("\nPROGRAMA PRINCIPAL FINALIZADO - %d Instrucoes Executadas\n", PC);
                        return;
                    }
                } else 
                    return;
                break;
            case 0:
                reg1 = MMU_Read(atual.add1, L1, L2, L3, RAM, buffer, &metrics->relogio, configs, hitsRam, missesRam);
                reg2 = MMU_Read(atual.add2, L1, L2, L3, RAM, buffer, &metrics->relogio, configs, hitsRam, missesRam);
                reg3.palavras[atual.add3.endPalavra] = reg1.palavras[atual.add1.endPalavra] + reg2.palavras[atual.add2.endPalavra];
                MMU_Write(L1, L2, L3, RAM, buffer, atual.add3, reg3.palavras[atual.add3.endPalavra], &metrics->relogio, configs, hitsRam, missesRam);
                PC++;
                break;
            case 1:
                reg1 = MMU_Read(atual.add1, L1, L2, L3, RAM, buffer, &metrics->relogio, configs, hitsRam, missesRam);
                reg2 = MMU_Read(atual.add2, L1, L2, L3, RAM, buffer, &metrics->relogio, configs, hitsRam, missesRam);
                reg3.palavras[atual.add3.endPalavra] = reg1.palavras[atual.add1.endPalavra] - reg2.palavras[atual.add2.endPalavra];
                MMU_Write(L1, L2, L3, RAM, buffer, atual.add3, reg3.palavras[atual.add3.endPalavra], &metrics->relogio, configs, hitsRam, missesRam);
                PC++;
                break;
            case 2:
                if (pPilha->qtd < pPilha->maxTam) {
                    pPilha->itens[pPilha->qtd - 1].PC = PC + 1;
                    PilhaPush(pPilha, (ItemPilha) {TI,0});
                    PilhaTopo(pPilha, &contextoAtual);
                    printf("INTERRUPCAO DETECTADA - PC %d Salvo na Pilha: Executando Interrupção de ordem %d\n\n", PC, pPilha->qtd - 1);
                    PC = 0;
                }
                else {
                    printf("INTERRUPCAO DETECTADA - LIMITE DE INTERRUPCOES ANINHADAS ATINGIDO - PC %d Ignorando interrupção\n", PC);
                    PC++;
                }
                break;
        }
    }
}