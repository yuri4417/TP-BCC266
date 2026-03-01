#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "menu.h"
#include "utils.h"
#include "structs.h"
#include "cores.h"
#include "tabela.h"
#include "disco.h"
#include "utils.h"

int main() {
    srand(time(NULL));
    BenchMetrics tabela[50];
    ConfigItem configs[] = {
        {"WriteBuffer",                    0, 0},
        {"LRU Insertion Policy (LIP)",     0, 0},
        {"LFU Policy ",                    1, 0},
        {"SRRIP Policy",                   0, 0},
        {"Gerador Multiplo",               0, 0},
        {"HD e Interrupcoes",              1, 0},
        {"SALVAR E VOLTAR",                0, 1} 
    };

    iniciar_menu();
    int rodando = 1;
    char *opcoes_principal[] = {"Benchmark Unico", "Configuracoes", "Tabelas", "Sair"};
    int qtdSalva = 0;

    if(!criaHd())
        return -1;

    while(rodando) {
        system("clear");
        int escolha = menu_run(opcoes_principal, 4, "CacheBenchmark - BCC 266");

        switch(escolha) {
            case 1: // Manual
                BenchMetrics m;
                setupBenchmark(&m, configs);
                CacheBenchmark(&m, configs, NULL, NULL);
                if(configs[ID_INTERRUPCAO].ativo)
                {
                    printf("\n" BOLD(YELLOW("--- FIM DA SIMULACAO ---")) "\n");
                    printf("Pressione ENTER para visualizar o relatorio final...");
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);

                }
                exibirRelatorioIndividual(&m,configs);
                salvaTabela(&qtdSalva, tabela, m);
                //testeRapido(configs);
                break;
            case 2: // configs
                menu_checkbox(configs, 7, "Configuracoes");
                break;
            case 3: // Bancada Padrão
                char *opcoesSubMenu []= {"Tabelas Locais", "Tabelas Padrao (M1 - M5)"};
                int opcao = menu_run(opcoesSubMenu, 2, "Opcoes de tabelas");
                switch(opcao) {
                    case 1:
                        imprimirTabelaSalva(tabela, qtdSalva, configs);
                        break; 
                    case 2:
                        testePadrao(configs);
                }
                break;
            case 4:
                rodando = 0;
                break;
        }
    }

    menu_close();
    system("clear");
    return 0;
}
