#include "structs.h"
#include "cpu.h"
#include "ram.h"
#include "pilha.h"

#include "MMU.h"


void cpu(Cache *L1, Cache *L2, Cache *L3, LinhaCache *RAM, BenchMetrics *metrics, WriteBuffer *buffer,
        ConfigItem *configs, PilhaExecucao *pPilha, Instrucao *TI) {
    LinhaCache reg1, reg2, reg3;
    ItemPilha contextoAtual;
    int PC; 
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
                    printf("CONTEXTO %d FINALIZADO COM %d INSTRUCOES EXECUTADAS", pPilha->qtd - 1, PC);
                    PilhaPop(pPilha);
                    if (pPilha->qtd > 0) {
                        PilhaTopo(pPilha, &contextoAtual);
                        PC = contextoAtual.PC;
                    }
                }
                else
                    return;
                break;
            case 0:
                reg1 = MMU_Read(atual.add1, L1, L2, L3, RAM, buffer, &metrics->relogio, configs);
                reg2 = MMU_Read(atual.add2, L1, L2, L3, RAM, buffer, &metrics->relogio, configs);
                reg3.palavras[atual.add3.endPalavra] = reg1.palavras[atual.add1.endPalavra] + reg2.palavras[atual.add2.endPalavra];
                MMU_Write(L1, L2, L3, RAM, buffer, atual.add3, reg3.palavras[atual.add3.endPalavra], &metrics->relogio, configs);
                PC++;
                break;
            case 1:
                reg1 = MMU_Read(atual.add1, L1, L2, L3, RAM, buffer, &metrics->relogio, configs);
                reg2 = MMU_Read(atual.add2, L1, L2, L3, RAM, buffer, &metrics->relogio, configs);
                reg3.palavras[atual.add3.endPalavra] = reg1.palavras[atual.add1.endPalavra] - reg2.palavras[atual.add2.endPalavra];
                MMU_Write(L1, L2, L3, RAM, buffer, atual.add3, reg3.palavras[atual.add3.endPalavra], &metrics->relogio, configs);
                PC++;
                break;
            case 2:
                pPilha->itens[pPilha->qtd - 1].PC = PC;
                PilhaPush(pPilha, (ItemPilha) {TI,0});
                PilhaTopo(pPilha, &contextoAtual);
                printf("INTERRUPCAO DETECTADA - PC %d Salvo na Pilha: Executando Interrupção no nível %d", PC, pPilha->qtd);
                PC = 0;
                break;
        }
    }
}