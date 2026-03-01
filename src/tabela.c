#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h> 
#include <ctype.h>
#include <string.h>

#include "tabela.h"
#include "utils.h"   
#include "cores.h"   
#include "structs.h" 

//criando macros para evitar repetição

#define H3  TAB_HOR TAB_HOR TAB_HOR 
#define H6  TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR
#define H8  TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR
#define H12 TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR
#define H15 H12 H3
#define H20 TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR TAB_HOR
#define H144 H12 H12 H12 H12 H12 H12 H12 H12 H12 H12 H12 H12
#define QTD_TESTES 20
//132 caracteres
#define H132 H12 H12 H12 H12 H12 H12 H12 H12 H12 H12 H12


int selecionarProbabilidade();
int selecionarNFor();
void inicializarMetricas(BenchMetrics *m);
 

void exibirInfoGeral(BenchMetrics *m, ConfigItem *configs) { // informações se a o writebuffer, lip estão ativos, numero de instruções, tamanho do for
    
    printf("\n");
    printf(TAB_TL H132 TAB_TR "\n");
    printf(TAB_VER " Resumo da execucao:%112s" TAB_VER "\n", ""); 
    printf(TAB_ML H132 TAB_MR "\n");

    
    if (configs[ID_BUFFER].ativo) {
        printf(TAB_VER " %-18s%s%106s" TAB_VER "\n", "Write Buffer:", GREEN("ATIVADO"), "");
    } else {
        printf(TAB_VER " %-18s%s%103s" TAB_VER "\n", "Write Buffer:", RED("DESATIVADO"), "");
    }
    
    if(configs[POL_RRIP].ativo)
        printf(TAB_VER " Politica %s%-4s%s%106s" TAB_VER "\n", m->policy,":",  GREEN("ATIVADO"), "");
    else    
        printf(TAB_VER " Politica %s%-6s%s%106s" TAB_VER "\n", m->policy,":",  GREEN("ATIVADO"), "");
    printf(TAB_VER " %-18s%-3d %% %107s" TAB_VER "\n", "Prob. Repeticao:", m->N_PROB, "");
    printf(TAB_VER " %-18s%-10d%103s" TAB_VER "\n", "Instr. p/ Loop:", m->N_FOR, "");
    printf(TAB_BL H132 TAB_BR "\n\n");
}

void cabecalho(ConfigItem *configs) {
    // BORDA SUPERIOR: 17 segmentos exatos fechando com TAB_TR
    printf(TAB_TL H3 TAB_TJ H6 TAB_TJ H6 TAB_TJ H6); // ID e Tamanhos
    printf(TAB_TJ H12 TAB_TJ H12 TAB_TJ H12);       // L1
    printf(TAB_TJ H12 TAB_TJ H12 TAB_TJ H12);       // L2
    printf(TAB_TJ H12 TAB_TJ H12 TAB_TJ H12);       // L3
    printf(TAB_TJ H12 TAB_TJ H12);                  // RAM% e DISCO%
    printf(TAB_TJ H15);                             // COLUNA FIXA HD (H15)
    printf(TAB_TJ H20 TAB_TR "\n");                  // COLUNA FINAL

    // TÍTULOS (Sempre com TAB_VER abrindo e fechando)
    printf(TAB_VER " M " TAB_VER "  L1  " TAB_VER "  L2  " TAB_VER "  L3  ");
    printf(TAB_VER "    H.L1    " TAB_VER "   Hit L1%%  " TAB_VER "    M.L1    ");
    printf(TAB_VER "    H.L2    " TAB_VER "   Hit L2%%  " TAB_VER "    M.L2    ");
    printf(TAB_VER "    H.L3    " TAB_VER "   Hit L3%%  " TAB_VER "    M.L3    ");
    printf(TAB_VER "  Hit RAM%%  " TAB_VER "   DISCO%%   " TAB_VER "   T.REAL HD   ");

    if (configs[ID_INTERRUPCAO].ativo) 
        printf(TAB_VER "    T. TOTAL (s)    " TAB_VER "\n");
    else 
        printf(TAB_VER "        TEMPO       " TAB_VER "\n");

    // LINHA DIVISÓRIA INTERNA (Igual à superior, mas com TAB_ML e TAB_MJ)
    printf(TAB_ML H3 TAB_MJ H6 TAB_MJ H6 TAB_MJ H6);
    printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);
    printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);
    printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);
    printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H15 TAB_MJ H20 TAB_MR "\n");
}

// vai imprimir os dados de cada configuração das caches, hits, miss, acesso na ram.
void imprimirLinha(int id, BenchMetrics *m, ConfigItem *configs) { 
    long totalAcessos = m->hitsL1 + m->missesL1;
    float pL1 = totalAcessos ? (float)m->hitsL1 * 100.0 / totalAcessos : 0.0;
    long tL2 = m->hitsL2 + m->missesL2;
    float pL2 = tL2 ? (float)m->hitsL2 * 100.0 / totalAcessos : 0.0;
    long tL3 = m->hitsL3 + m->missesL3;
    float pL3 = tL3 ? (float)m->hitsL3 * 100.0 / totalAcessos : 0.0;
    
    float pRAM = totalAcessos ? (float)m->hitsRAM * 100.0 / totalAcessos : 0.0; 
    float taxaHd = totalAcessos ? ((float)m->missesRAM / totalAcessos) * 100.0 : 0.0;

    printf(TAB_VER " M%-d" TAB_VER " %-4d " TAB_VER " %-4d " TAB_VER " %-4d ", id, m->tamL1, m->tamL2, m->tamL3);
    printf(TAB_VER " %10d " TAB_VER " %9.1f%% " TAB_VER " %10d ", m->hitsL1, pL1, m->missesL1);
    printf(TAB_VER " %10d " TAB_VER " %9.1f%% " TAB_VER " %10d ", m->hitsL2, pL2, m->missesL2);
    printf(TAB_VER " %10d " TAB_VER " %9.1f%% " TAB_VER " %10d ", m->hitsL3, pL3, m->missesL3);
    printf(TAB_VER " %9.1f%% " TAB_VER " %9.1f%% ", pRAM, taxaHd);
    
    // T.REAL HD (Sempre cercada por TAB_VER)
    printf(TAB_VER " %12.7fs ", m->tempoHD); 
    
    // COLUNA FINAL DINÂMICA (Sempre fechando com TAB_VER)
    if (configs[ID_INTERRUPCAO].ativo)
        printf(TAB_VER " %17.7fs " TAB_VER "\n", m->tempoTotal); // REAL (Double)
    else 
        printf(TAB_VER " %18ld " TAB_VER "\n", m->relogio);         // SIMULADO (Long Int)

}


void rodape() {
    // BL = Bottom Left, BJ = Bottom Junction, BR = Bottom Right
    printf(TAB_BL H3 TAB_BJ H6 TAB_BJ H6 TAB_BJ H6); // Base ID/Tamanhos
    printf(TAB_BJ H12 TAB_BJ H12 TAB_BJ H12);       // Base L1
    printf(TAB_BJ H12 TAB_BJ H12 TAB_BJ H12);       // Base L2
    printf(TAB_BJ H12 TAB_BJ H12 TAB_BJ H12);       // Base L3
    printf(TAB_BJ H12 TAB_BJ H12);                  // Base RAM/Disco
    printf(TAB_BJ H15);                             // Base HD
    printf(TAB_BJ H20 TAB_BR "\n");                  // Base Final
}

void testePadrao(ConfigItem *configs) {
    endwin();
    setbuf(stdout, NULL);
    system("clear"); 

    int prob = selecionarProbabilidade();
    int nFor = selecionarNFor();
    
    int probInt = 0;
    int qtdInt = 1;
    if (configs[ID_INTERRUPCAO].ativo)  { // para gerar interrupções caso o modo esteja ativo
        system("clear");
        printf("Digite a Probabilidade de Interrupcao: ");
        scanf("%d", &probInt);
        printf("Tamanho do Programa de Interrupcao: ");
        scanf("%d", &qtdInt);
    }
    
    system("clear"); 

    int maquinas[5][3] = { 
        {8, 16, 32}, {32, 64, 128}, {16, 64, 256}, {8, 32, 128}, {16, 32, 64} 
    };
    
    BenchMetrics resultados[5]; 
    BenchMetrics totalizador;
    inicializarMetricas(&totalizador);

    totalizador.N_PROB = prob; 
    totalizador.N_FOR = nFor;
    
    totalizador.PROB_INTERRUPCAO = probInt; // passa os valores  lido para a struct que armazena as variaveis
    totalizador.qtdInterrupcao = qtdInt;

    Instrucao *programa = (configs[ID_INTERRUPCAO].ativo) ? 
            gerarInstrucoes(10000, TAM_HD_DEFAULT, totalizador.N_PROB, totalizador.PROB_INTERRUPCAO, totalizador.N_FOR, 4) :
            gerarInstrucoes(10000, TAM_RAM_DEFAULT, totalizador.N_PROB, 0, totalizador.N_FOR, 4);

    if (configs[ID_INTERRUPCAO].ativo)
        geraInterrupcao(totalizador.qtdInterrupcao, 5, totalizador.N_FOR, 3, TAM_HD_DEFAULT, 4);
    Instrucao *TI = malloc(totalizador.qtdInterrupcao * sizeof(Instrucao));
    if (configs[ID_INTERRUPCAO].ativo) {
        FILE *pFile = fopen("TI.txt", "r");
        int i = 0;
        while (i < totalizador.qtdInterrupcao && 
               fscanf(pFile, "%d %d %d %d %d %d %d", &TI[i].opcode, &TI[i].add1.endBloco, &TI[i].add1.endPalavra,
                &TI[i].add2.endBloco, &TI[i].add2.endPalavra, &TI[i].add3.endBloco, &TI[i].add3.endPalavra) == 7) {
            i++;
        }
        fclose(pFile);
    }
    

    for (int i = 0; i < 5; i++) {
        BenchMetrics *r = &resultados[i];
        inicializarMetricas(r);
        
        r->tamL1 = maquinas[i][0]; 
        r->tamL2 = maquinas[i][1]; 
        r->tamL3 = maquinas[i][2];
        r->N_PROB = prob; 
        r->N_FOR = nFor;
        r->tamWriteBuffer = configs[ID_BUFFER].ativo ? 4 : -1;
        

        r->PROB_INTERRUPCAO = probInt;
        r->qtdInterrupcao = qtdInt; // passa os valores para cacheBenchmark realizar as operações das caches, ram, hd

        CacheBenchmark(r, configs, programa, TI);
        totalizador.qtdStalls += r->qtdStalls;
        strcpy(totalizador.policy, r->policy);
    }
    if(configs[ID_INTERRUPCAO].ativo)
    {
        printf("\n" BOLD(YELLOW("--- FIM DA SIMULACAO ---")) "\n");
        printf("Pressione ENTER para visualizar o relatorio final...");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
    }
    system("clear");
    printf("\n" BOLD(CYAN("Resultados:")) "\n");
    exibirInfoGeral(&totalizador, configs);
    cabecalho(configs);

    for (int i = 0; i < 5; i++) {
        imprimirLinha(i + 1, &resultados[i], configs);

        // Se não for a última linha, imprime a divisória
        if (i < 4) { 
            printf(TAB_ML H3 TAB_MJ H6 TAB_MJ H6 TAB_MJ H6); // ID e Tamanhos
            printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);       // L1, L2, L3
            printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);       
            printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);       
            printf(TAB_MJ H12 TAB_MJ H12);                  // RAM% e DISCO%

            printf(TAB_MJ H15 TAB_MJ H20 TAB_MR "\n");                 // Coluna Final (Tempo)
        }
    }

    rodape();
    printf("\n" GREEN("Pressione ENTER para sair...") "\n");
    int c; 
    while ((c = getchar()) != '\n' && c != EOF);
    getchar(); 
    refresh();
    free(programa); free(TI);
}

void salvaTabela(int *qtdSalva, BenchMetrics *tabelaSalva, BenchMetrics m ) {
    printf("\n" YELLOW("Deseja salvar esse resultado na tabela? [S/N]: ") " ");
    char resp = tolower(getchar()); 
    if (resp != '\n') {
        int c; 
        while ((c = getchar()) != '\n' && c != EOF);
    }
    if (resp == 's') { 
            if ((*qtdSalva) < 50) {
                tabelaSalva[(*qtdSalva)] = m; 
                (*qtdSalva)++;
                printf(GREEN("Resultado salvo com sucesso! ID: M%d") "\n", (*qtdSalva));
            } else 
                printf(RED("Memoria cheia! Nao é possivel salvar mais.") "\n");
            
        printf("Pressione ENTER para continuar...");
        getchar();
    }
}

void exibirRelatorioIndividual(BenchMetrics *m, ConfigItem *configs) { 
    endwin();
    setbuf(stdout, NULL); 
    system("clear"); 
    exibirInfoGeral(m, configs);
    cabecalho(configs);
    imprimirLinha(1, m, configs);
    rodape();
    printf("\n");
    printf("\n\nPressione ENTER para voltar ao menu...");
    getchar();
}

void inicializarMetricas(BenchMetrics *m) { 
    m->hitsL1 = 0; m->missesL1 = 0;
    m->hitsL2 = 0; m->missesL2 = 0;
    m->hitsL3 = 0; m->missesL3 = 0;
    m->missesRAM = 0;
    m->hitsRAM = 0; 
    m->qtdStalls = 0;
    m->relogio = 0;
    m->N_PROB = 0; 
    m->N_FOR = 0;

    m->tempoTotal = 0.0;
    m->tempoHD = 0.0;
}

int selecionarProbabilidade() { // leitura da probabilidade
    system("clear");
    printf("Escolha a Probabilidade de Repeticao:\n");
    printf("1 - 50%%\n");
    printf("2 - 75%%\n");
    printf("3 - 90%% \n");
    printf("\nEscolha: ");
    int op;
    scanf("%d", &op); 
    switch(op) {
        case 1: 
            return 50;
        case 2: 
            return 75;
        case 3: 
            return 90;
        default: 
            return 75;
    }
}

int selecionarNFor() { // leitura do numero de repetições, caso caia na probabilidade
    system("clear");
    printf("\n" BOLD(YELLOW("Tamanho do for ")) "\n\n");  
    printf("1 - 5 inst.\n");
    printf("2 - Personalizado\n");
    printf("\nEscolha: ");
    int op;
    scanf("%d", &op); 
    switch(op) {
        case 1: 
            return 5;
        case 2: {
            printf("\nDigite o tamanho do loop desejado: ");
            int val;
            scanf("%d", &val);
            return (val > 0) ? val : 5;
        }
        default: 
            return 5;
    }
}
void imprimirTabelaSalva(BenchMetrics *lista, int qtd, ConfigItem *configs) {
    endwin();
    setbuf(stdout, NULL);
    system("clear");

    printf("\n" BOLD(MAGENTA("Tabela de resultados salvos")) "\n\n");

    if (qtd == 0) {
        printf(RED("Nenhum resultado foi morto/salvo ainda.") "\n");
    } else {
        // --- CABEÇALHO SUPERIOR ---
        printf(TAB_TL H3 TAB_TJ H8 TAB_TJ H6 TAB_TJ H6 TAB_TJ H6); // Fixos
        printf(TAB_TJ H12 TAB_TJ H12 TAB_TJ H12); // Caches
        printf(TAB_TJ H12 TAB_TJ H12 TAB_TJ H12); 
        printf(TAB_TJ H12 TAB_TJ H12); // RAM e Disco
        
        if (configs[ID_INTERRUPCAO].ativo) printf(TAB_TJ H12); // COLUNA DINÂMICA
        
        printf(TAB_TJ H20 TAB_TR "\n");

        // --- TÍTULOS DAS COLUNAS ---
        printf(TAB_VER " M " TAB_VER " Pol.   " TAB_VER "  L1  " TAB_VER "  L2  " TAB_VER "  L3  ");
        printf(TAB_VER "    H.L1    " TAB_VER "   Hit L1%%  " TAB_VER "    M.L1    ");
        printf(TAB_VER "    H.L2    " TAB_VER "   Hit L2%%  " TAB_VER "    M.L2    ");
        printf(TAB_VER "    H.L3    " TAB_VER "   Hit L3%%  " TAB_VER "    M.L3    ");
        printf(TAB_VER "  Hit RAM%%  " TAB_VER "   DISCO%%   ");
        
        if (configs[ID_INTERRUPCAO].ativo) printf(TAB_VER "  T.REAL HD ");
        
        printf(TAB_VER "        TEMPO       " TAB_VER "\n");

        // --- DIVISÓRIA DO CABEÇALHO ---
        printf(TAB_ML H3 TAB_MJ H8 TAB_MJ H6 TAB_MJ H6 TAB_MJ H6);
        printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);
        printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);
        printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);
        printf(TAB_MJ H12 TAB_MJ H12);
        
        if (configs[ID_INTERRUPCAO].ativo) printf(TAB_MJ H12);
        
        printf(TAB_MJ H20 TAB_MR "\n");

        for (int i = 0; i < qtd; i++) {
            BenchMetrics *m = &lista[i];
            long int totalAcessos = m->hitsL1 + m->missesL1;

            float pL1 = totalAcessos ? (float)m->hitsL1 * 100.0 / totalAcessos : 0.0;
            long int tL2 = m->hitsL2 + m->missesL2;
            float pL2 = tL2 ? (float)m->hitsL2 * 100.0 / totalAcessos : 0.0;
            long int tL3 = m->hitsL3 + m->missesL3;
            float pL3 = tL3 ? (float)m->hitsL3 * 100.0 / totalAcessos : 0.0;
            float pRAM = totalAcessos ? (float)m->hitsRAM * 100.0 / totalAcessos : 0.0;
            float taxaHd = totalAcessos ? ((float)m->missesRAM / totalAcessos) * 100.0 : 0.0;

            // --- DADOS DA LINHA ---
            printf(TAB_VER "M%-2d" TAB_VER " %-7s" TAB_VER " %-4d " TAB_VER " %-4d " TAB_VER " %-4d ", i + 1, m->policy, m->tamL1, m->tamL2, m->tamL3);
            printf(TAB_VER " %10d " TAB_VER " %9.1f%% " TAB_VER " %10d ", m->hitsL1, pL1, m->missesL1);
            printf(TAB_VER " %10d " TAB_VER " %9.1f%% " TAB_VER " %10d ", m->hitsL2, pL2, m->missesL2);
            printf(TAB_VER " %10d " TAB_VER " %9.1f%% " TAB_VER " %10d ", m->hitsL3, pL3, m->missesL3);
            printf(TAB_VER " %9.1f%% " TAB_VER " %9.1f%% ", pRAM, taxaHd);
            
            if (configs[ID_INTERRUPCAO].ativo) printf(TAB_VER " %9.4fs ", m->tempoHD);
            
            printf(TAB_VER " %18ld " TAB_VER "\n", m->relogio);
                   
            // --- SEPARADOR ENTRE LINHAS ---
            if (i < qtd - 1) {
               printf(TAB_ML H3 TAB_MJ H8 TAB_MJ H6 TAB_MJ H6 TAB_MJ H6); 
               printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);
               printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);
               printf(TAB_MJ H12 TAB_MJ H12 TAB_MJ H12);
               printf(TAB_MJ H12 TAB_MJ H12);
               if (configs[ID_INTERRUPCAO].ativo) printf(TAB_MJ H12);
               printf(TAB_MJ H20 TAB_MR "\n");
            }
        }
        
        // --- RODAPÉ FINAL ---
        printf(TAB_BL H3 TAB_BJ H8 TAB_BJ H6 TAB_BJ H6 TAB_BJ H6);
        printf(TAB_BJ H12 TAB_BJ H12 TAB_BJ H12);
        printf(TAB_BJ H12 TAB_BJ H12 TAB_BJ H12);
        printf(TAB_BJ H12 TAB_BJ H12 TAB_BJ H12);
        printf(TAB_BJ H12 TAB_BJ H12);
        if (configs[ID_INTERRUPCAO].ativo) printf(TAB_BJ H12);
        printf(TAB_BJ H20 TAB_BR "\n");
    }

    printf("\n" GREEN("Pressione ENTER para voltar ao menu...") "\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    refresh();
}
