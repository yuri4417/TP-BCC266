#ifndef TABELA_H
#define TABELA_H

#include "structs.h"
 
void cabecalho();
void imprimirLinha(int idMaquina, BenchMetrics *m);
void rodape();
void salvaTabela(int *qtdSalva, BenchMetrics *tabelaSalva, BenchMetrics m);
//fuções para imprimir a tabela
void testePadrao(ConfigItem *configs);               // Fixa (M1 - M5)
void inicializarMetricas(BenchMetrics *m);
void imprimirTabelaSalva(BenchMetrics *lista, int qtd);       // Variável (M1 - Mn)

void exibirRelatorioIndividual(BenchMetrics *m, ConfigItem *configs);
#endif