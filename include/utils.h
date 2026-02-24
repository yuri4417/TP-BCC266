#ifndef UTILS_H
#define UTILS_H

Instrucao* criarPadrao(int PROB_INTERRUPCAO, int N_FOR, int INICIO_RAM, int rangeMemoria, int N_WORD);
Instrucao* gerarInstrucoes(int N_INST, int N_MEM, int N_PROB, int PROB_INTERRUPCAO, int N_FOR, int N_WORD);
void setupBenchmark(BenchMetrics *metrics, ConfigItem *configs);
void CacheBenchmark(BenchMetrics *metrics, ConfigItem *configs);
void testeRapido(ConfigItem *configs); 

#endif 