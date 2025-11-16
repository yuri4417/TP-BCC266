#include "programas.h"
#include "ram.h"
#include "maquina.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
void programaAleatorio(int qtdeInstrucoes) {
    int tamRAM = 1000;
    int *RAM = criaRandomRam(tamRAM);
    Instrucao* programa = (Instrucao*) malloc(qtdeInstrucoes*sizeof(Instrucao));

    for (int i = 0; i < qtdeInstrucoes - 1; i++) {
        programa[i].opcode = rand() % 2;
        programa[i].endereco1 = rand() % (tamRAM - 1);
        programa[i].endereco2 = rand() % (tamRAM - 1);
        programa[i].endereco3 = rand() % (tamRAM - 1);
    }
    programa[qtdeInstrucoes-1].opcode = -1;

    maquina(RAM, programa);
    liberaRAM(RAM);
    free(programa);
}

// -1 - Fim do programa
// 0 - soma                                   [OP, end1, end2, endSalvar]
// 1 - subtracao                              [OP, end1, end2, endSalvar]
// 2 - reg -> ram                             [OP, regFonte, posRam, -1]
// 3 - ram -> reg                             [OP, regDestino, posRam, -1]
// 4 - copia conteudo do exterior pro reg     [OP, regDestino, conteudo, -1]
// 5 - copia conteudo do reg p/ exterior      [OP, regFonte, destino, -1]


void programaMultiplica(int * RAM, int multiplicando, int multiplicador) { // RESULTADO = RAM[0]
    int ramLocal = 0; //indica se é um programa isolado ou que faz parte de outro maior (se faz, usa a ram do programa principal)
    if (RAM == NULL) {
        RAM = criaRam_vazia(2);
        ramLocal = 1;
    }

    //Limpa o RAM[0] pros casos de multiplica ser um programa auxiliar
    Instrucao clean[3];

    clean[0].opcode = 4;
    clean[0].endereco1 = 1;
    clean[0].endereco2 = 0;
    
    clean[1].opcode = 2;
    clean[1].endereco1 = 1;
    clean[1].endereco2 = 0;

    clean[2].opcode = -1;

    maquina(RAM, clean);


    // Move multiplicando pro reg -> depois manda do reg pra ram
    Instrucao move[3];
    move[0].opcode = 4;
    move[0].endereco1 = 1;
    move[0].endereco2 = multiplicando;

    move[1].opcode = 2;
    move[1].endereco1 = 1;
    move[1].endereco2 = 1;

    move[2].opcode = -1;

    maquina(RAM, move);

    // RAM = [0, multiplicando]

    //soma continua da posicao 0 com a posicao 1 da ram
    for (int i = 0; i < multiplicador; i++) {
        Instrucao mult[2];

        mult[0].opcode = 0;
        mult[0].endereco1 = 0;
        mult[0].endereco2 = 1;
        mult[0].endereco3 = 0;

        mult[1].opcode = -1;
        maquina(RAM, mult);
    }

    //[resultado, multiplicando]

    //Move da ram pro reg1 -> depois do reg pro endereco2 da instrucao 2
    Instrucao move2[3];

    move2[0].opcode = 3;
    move2[0].endereco1 = 1;
    move2[0].endereco2 = 0;

    move2[1].opcode = 5;
    move2[1].endereco1 = 1;
    move2[1].endereco2 = -1;

    move2[2].opcode = -1;
    
    maquina(RAM, move2);
    printf("MULT: %d x %d = %d\n", multiplicando, multiplicador, move2[1].endereco2);
    

    // printRam(RAM, 2);
    if (ramLocal) {
        liberaRAM(RAM);
    }
}

void programaDivisao(int *RAM, int dividendo, int divisor) { //RESULTADO = RAM[0]

    int ramLocal = 0; //indica se é um programa isolado ou que faz parte de outro maior (se faz, usa a ram do programa principal)
    if (RAM == NULL) {
        RAM = criaRam_vazia(4);
        ramLocal = 1;
    }

    //Limpa o RAM[3] pros casos de Divisao ser um programa auxiliar
    Instrucao clean[3];

    clean[0].opcode = 4;
    clean[0].endereco1 = 1;
    clean[0].endereco2 = 0;
    
    clean[1].opcode = 2;
    clean[1].endereco1 = 1;
    clean[1].endereco2 = 3;

    clean[2].opcode = -1;

    maquina(RAM, clean);
    //Manda divisor e dividendo para o registrador -> depois manda do registrador pra ram
    Instrucao move[5];

    move[0].opcode = 4;
    move[0].endereco1 = 1;
    move[0].endereco2 = dividendo;

    move[1].opcode = 4;
    move[1].endereco1 = 2;
    move[1].endereco2 = divisor;

    move[2].opcode = 2;
    move[2].endereco1 = 1;
    move[2].endereco2 = 0;
    
    move[3].opcode = 2;
    move[3].endereco1 = 2;
    move[3].endereco2 = 1;

    move[4].opcode = -1;

    maquina(RAM, move);

    //RAM = [dividendo, divisor, 0, 0]

    // Salva '1' na ram para atuar como somador do contador
    Instrucao addCounter[3];

    addCounter[0].opcode = 4;
    addCounter[0].endereco1 = 1;
    addCounter[0].endereco2 = 1;

    addCounter[1].opcode = 2;
    addCounter[1].endereco1 = 1;
    addCounter[1].endereco2 = 2;

    addCounter[2].opcode = -1;

    maquina(RAM, addCounter);
    //RAM = [dividendo, divisor, 1, 0]

    //Subtrai o divisor do dividendo, incrementa 1 no contador e atualiza o dividendo
    while (dividendo >= divisor) {
        Instrucao div[5];

        div[0].opcode = 1;
        div[0].endereco1 = 0;
        div[0].endereco2 = 1;
        div[0].endereco3 = 0;

        div[1].opcode = 0;
        div[1].endereco1 = 2;
        div[1].endereco2 = 3;
        div[1].endereco3 = 3;

        div[2].opcode = 3;
        div[2].endereco1 = 1;
        div[2].endereco2 = 0;

        div[3].opcode = 5;
        div[3].endereco1 = 1;
        div[3].endereco2 = -1;

        div[4].opcode = -1;

        maquina(RAM, div);
        dividendo = div[3].endereco2;
    }

    //Manda o resultado final da ram pro reg -> depois do reg pro exterior
    Instrucao move2[4];

    move2[0].opcode = 3;
    move2[0].endereco1 = 1;
    move2[0].endereco2 = 3;

    move2[1].opcode = 2;
    move2[1].endereco1 = 1;
    move2[1].endereco2 = 0;

    move2[2].opcode = 5;
    move2[2].endereco1 = 1;
    move2[2].endereco2 = -1;

    move2[3].opcode = -1;

    maquina(RAM,move2);
    printf("DIV: Resultado da divisao: %d\n", move2[2].endereco2);

    // printRam(RAM, 4);
    if (ramLocal)
        liberaRAM(RAM);
}

void programaRaizQuad(int* RAM, int radicando) { //RESULTADO == RAM[0]
    int ramLocal = 0; //indica se é um programa isolado ou que faz parte de outro maior (se faz, usa a ram do programa principal)
    if (RAM == NULL) {
        RAM = criaRam_vazia(7);
        ramLocal = 1;
    }

    //Esquema final RAM -> [programaMultiplica, programaMultiplica, radicando, RESULTADO FINAL, result, counter, 1]



    //Move o radicando e '1' pros regs -> depois dos regs pra ram
    Instrucao move[5];
    move[0].opcode = 4;
    move[0].endereco1 = 1;
    move[0].endereco2 = radicando;

    move[1].opcode = 2;
    move[1].endereco1 = 1;
    move[1].endereco2 = 2;

    move[2].opcode = 4;
    move[2].endereco1 = 2;
    move[2].endereco2 = 1;

    move[3].opcode = 2;
    move[3].endereco1 = 2;
    move[3].endereco2 = 6;

    move[4].opcode = -1;

    maquina(RAM, move);

    //ram -> [0, 0, radicando, 0, 0, 0, 1]
    int result = INT_MAX;
    while (result >= 0) {

        Instrucao copia[3];
        //faz a copia do contador (ultimo valor valido) pro resultado final
        copia[0].opcode  = 3;
        copia[0].endereco1 = 1;
        copia[0].endereco2 = 5;

        copia[1].opcode = 2;
        copia[1].endereco1 = 1;
        copia[1].endereco2 = 3;

        copia[2].opcode = -1;
        maquina(RAM, copia);
        //RAM -> [X, X, radicando, counter valido, X, counter, 1]


        Instrucao sum[2];
        //Incrementa o contador p testar o proximo quadrado
        sum[0].opcode = 0;
        sum[0].endereco1 = 5;
        sum[0].endereco2 = 6;
        sum[0].endereco3 = 5;

        sum[1].opcode = -1;

        maquina(RAM, sum);
        //RAM -> [0, 0, radicando, counter, 0, counter + 1, result]

        // Extrai o valor a ser testado
        Instrucao calcula[3];
        calcula[0].opcode = 3;
        calcula[0].endereco1 = 1;
        calcula[0].endereco2 = 5;

        calcula[1].opcode = 5;
        calcula[1].endereco1 = 1;
        calcula[1].endereco2 = -1;

        calcula[2].opcode = -1;
        
        maquina(RAM, calcula);
        int y = calcula[1].endereco2;    
        programaMultiplica(RAM, y, y);
        //RAM -> [y^2, y, radicando, counter, 0, counter + 1, result]

        // Faz radicando - y^2
        Instrucao sub[2];
        sub[0].opcode = 1;
        sub[0].endereco1 = 2;
        sub[0].endereco2 = 0;
        sub[0].endereco3 = 4;

        sub[1].opcode = -1;

        maquina(RAM, sub);
        //RAM -> [y^2, y, radicando, counter, result, counter + 1, result]

        // Pega result e atualiza a variavel em C pro prox loop
        Instrucao att[3];
        att[0].opcode = 3;
        att[0].endereco1 = 1;
        att[0].endereco2 = 4;

        att[1].opcode = 5;
        att[1].endereco1 = 1;
        att[1].endereco2 = -1;

        att[2].opcode  = -1;
        maquina(RAM, att);
        result = att[1].endereco2;
    }

    Instrucao extract[4];
    extract[0].opcode = 3;
    extract[0].endereco1 = 1;
    extract[0].endereco2 = 3;

    extract[1].opcode = 2;
    extract[1].endereco1 = 1;
    extract[1].endereco2 = 0;

    extract[2].opcode = 5;
    extract[2].endereco1 = 1;
    extract[2].endereco2 = -1;

    extract[3].opcode = -1;
    maquina(RAM, extract);
    int sqrt = extract[2].endereco2;
    printf("SQRT(%d) = %d\n", radicando, sqrt);

    // printRam(RAM, 7);
    if (ramLocal)
        liberaRAM(RAM);
}

void programaPA(int A1, int razao, int qtdTermos) {
    int *RAM = criaRam_vazia(7 + (qtdTermos - 1)); // espacos fixos + tamanho da sequencia
    
    //RAM -> [programamultiplica, programamultiplica, razao, contadorPosPA, contadorWhile, 1, A1, P.A...]

    //Move A1, razao, constante 1, contadorPos pra RAM
    Instrucao moveTermos[9];
    moveTermos[0].opcode = 4;
    moveTermos[0].endereco1 = 1;
    moveTermos[0].endereco2 = A1;

    moveTermos[1].opcode = 4;
    moveTermos[1].endereco1 = 2;
    moveTermos[1].endereco2 = razao;

    moveTermos[2].opcode = 2;
    moveTermos[2].endereco1 = 1;
    moveTermos[2].endereco2 = 6;

    moveTermos[3].opcode = 2;
    moveTermos[3].endereco1 = 2;
    moveTermos[3].endereco2 = 2;

    moveTermos[4].opcode = 4;
    moveTermos[4].endereco1 = 1;
    moveTermos[4].endereco2 = 1;

    moveTermos[5].opcode = 4;
    moveTermos[5].endereco1 = 2;
    moveTermos[5].endereco2 = 7;

    moveTermos[6].opcode = 2;
    moveTermos[6].endereco1 = 1;
    moveTermos[6].endereco2 = 5;

    moveTermos[7].opcode = 2;
    moveTermos[7].endereco1 = 2;
    moveTermos[7].endereco2 = 3;

    moveTermos[8].opcode = -1;
    maquina(RAM, moveTermos);
    //RAM -> [0, 0, razao, contadorPos, 0, 1, A1, 0....]

    // Inicializa e move o contador pra ram
    int contadorWhile = qtdTermos - 1;
    Instrucao moveCounter[3];

    moveCounter[0].opcode = 4;
    moveCounter[0].endereco1 = 1;
    moveCounter[0].endereco2 = contadorWhile;

    moveCounter[1].opcode = 2;
    moveCounter[1].endereco1 = 1;
    moveCounter[1].endereco2 = 4;

    moveCounter[2].opcode = -1;
    maquina(RAM, moveCounter);
    //RAM -> [0, 0, razao, contadorPos, contadorWhile, 1, A1, 0....]

    while (contadorWhile > 0) {
        // Pega a posicao anterior para calcular pra proxima
        Instrucao copiaPOS[4];
        copiaPOS[0].opcode = 1;
        copiaPOS[0].endereco1 = 3;
        copiaPOS[0].endereco2 = 5;
        copiaPOS[0].endereco3 = 1;

        copiaPOS[1].opcode = 3;
        copiaPOS[1].endereco1 = 1;
        copiaPOS[1].endereco2 = 1;
        
        copiaPOS[2].opcode = 5;
        copiaPOS[2].endereco1 = 1;
        copiaPOS[2].endereco2 = -1;

        copiaPOS[3].opcode = -1;
        maquina(RAM, copiaPOS);
        int posAnterior = copiaPOS[2].endereco2;

        //Pega posicao atual
        Instrucao posAtual[3];
        posAtual[0].opcode = 3;
        posAtual[0].endereco1 = 1;
        posAtual[0].endereco2 = 3;

        posAtual[1].opcode = 5;
        posAtual[1].endereco1 = 1;
        posAtual[1].endereco2 = -1;

        posAtual[2].opcode = -1;
        maquina(RAM, posAtual);
        int pos = posAtual[1].endereco2;

        //Calcula o novo termo
        Instrucao calculaTermo[2];
        calculaTermo[0].opcode = 0;
        calculaTermo[0].endereco1 = posAnterior;
        calculaTermo[0].endereco2 = 2;
        calculaTermo[0].endereco3 = pos;

        calculaTermo[1].opcode = -1;
        maquina(RAM, calculaTermo);
        
        // Atualiza o contador +1 pro proximo loop
        Instrucao attPos[2];
        attPos[0].opcode = 0;
        attPos[0].endereco1 = 3;
        attPos[0].endereco2 = 5;
        attPos[0].endereco3 = 3;

        attPos[1].opcode = -1;
        maquina(RAM, attPos);
        //RAM -> [0, 0, razao, contadorPos + 1, contadorWhile, 1, A1, P.A....]
        // Decremento contador
        Instrucao attContador[4];
        attContador[0].opcode = 1;
        attContador[0].endereco1 = 4;
        attContador[0].endereco2 = 5;
        attContador[0].endereco3 = 4;

        attContador[1].opcode = 3;
        attContador[1].endereco1 = 1;
        attContador[1].endereco2 = 4;

        attContador[2].opcode = 5;
        attContador[2].endereco1 = 1;
        attContador[2].endereco2 = -1;

        attContador[3].opcode = -1;
        maquina(RAM, attContador);
        //RAM -> [0, 0, razao, contadorPos, contadorWhile - 1, 1, A1, P.A....]
        
        contadorWhile = attContador[2].endereco2;
    }

    Instrucao zeraRAM[3];
    zeraRAM[0].opcode = 4;
    zeraRAM[0].endereco1 = 1;
    zeraRAM[0].endereco2 = 0;

    zeraRAM[1].opcode = 2;
    zeraRAM[1].endereco1 = 1;
    zeraRAM[1].endereco2 = 4;

    zeraRAM[2].opcode = -1;
    maquina(RAM, zeraRAM);
    //RAM -> [0, 0, razao, contadorPos + 1, 0, 1, A1, P.A....]


    for (int i = 6; i < (6 + qtdTermos); i++) {

        Instrucao somaPA[2];
        somaPA[0].opcode = 0;
        somaPA[0].endereco1 = 4;
        somaPA[0].endereco2 = i;
        somaPA[0].endereco3 = 4;

        somaPA[1].opcode = -1;
        maquina(RAM, somaPA);
    }
    //RAM -> [0, 0, razao, contadorPos + 1, somaPA, 1, A1, P.A....]

    printf("P.A. (a1 = %d, r = %d) -> (", A1, razao);
    for (int i = 6; i < (6 + qtdTermos); i++) {
        Instrucao pegaTermo[3];
        pegaTermo[0].opcode = 3;
        pegaTermo[0].endereco1 = 1;
        pegaTermo[0].endereco2 = i;

        pegaTermo[1].opcode = 5;
        pegaTermo[1].endereco1 = 1;
        pegaTermo[1].endereco2 = -1;

        pegaTermo[2].opcode = -1;
        maquina(RAM, pegaTermo);
        int termo = pegaTermo[1].endereco2;
        printf("%d", termo);
        if (i < (5 + qtdTermos))
            printf(", "); 
    }
    printf(")\n");

    Instrucao pegaSoma[3];
    pegaSoma[0].opcode = 3;
    pegaSoma[0].endereco1 = 1;
    pegaSoma[0].endereco2 = 4;

    pegaSoma[1].opcode = 5;
    pegaSoma[1].endereco1 = 1;
    pegaSoma[1].endereco2 = -1;

    pegaSoma[2].opcode = -1;
    maquina(RAM, pegaSoma);
    int somaPA = pegaSoma[1].endereco2;

    printf("Soma dos termos: %d\n", somaPA);
    printRam(RAM, (6+qtdTermos));

    liberaRAM(RAM);
}

void programaPG(int A1, int razao, int qtdTermos) {

    int *RAM = criaRam_vazia(6 + qtdTermos);

    int contadorWhile = qtdTermos - 1;
    //RAM -> [programamultiplica, programamultiplica, razao, contadorposPG, contadorWhile, 1, A1, pg....]

    // Move pra ram
    Instrucao moveTermos[11];
    moveTermos[0].opcode = 4;
    moveTermos[0].endereco1 = 1;
    moveTermos[0].endereco2 = 1;
    
    moveTermos[1].opcode = 4;
    moveTermos[1].endereco1 = 2;
    moveTermos[1].endereco2 = razao;

    moveTermos[2].opcode = 2;
    moveTermos[2].endereco1 = 1;
    moveTermos[2].endereco2 = 5;

    moveTermos[3].opcode = 2;
    moveTermos[3].endereco1 = 2;
    moveTermos[3].endereco2 = 2;

    moveTermos[4].opcode = 4;
    moveTermos[4].endereco1 = 1;
    moveTermos[4].endereco2 = A1;

    moveTermos[5].opcode = 4;
    moveTermos[5].endereco1 = 2;
    moveTermos[5].endereco2 = 7;

    moveTermos[6].opcode = 2;
    moveTermos[6].endereco1 = 1;
    moveTermos[6].endereco2 = 6;

    moveTermos[7].opcode = 2;
    moveTermos[7].endereco1 = 2;
    moveTermos[7].endereco2 = 3;

    moveTermos[8].opcode = 4;
    moveTermos[8].endereco1 = 1;
    moveTermos[8].endereco2 = contadorWhile;

    moveTermos[9].opcode = 2;
    moveTermos[9].endereco1 = 1;
    moveTermos[9].endereco2 = 4;

    moveTermos[10].opcode = -1;
    maquina(RAM, moveTermos);
    //RAM -> [0, 0, razao, contadorPosPG, contadorwhile, 1, A1, 0....]

    while (contadorWhile > 0) {

        // Pega pos anterior
        Instrucao copiaPos[4];
        copiaPos[0].opcode = 1;
        copiaPos[0].endereco1 = 3;
        copiaPos[0].endereco2 = 5;
        copiaPos[0].endereco3 = 0;

        copiaPos[1].opcode = 3;
        copiaPos[1].endereco1 = 1;
        copiaPos[1].endereco2 = 0;

        copiaPos[2].opcode = 5;
        copiaPos[2].endereco1 = 1;
        copiaPos[2].endereco2 = -1;

        copiaPos[3].opcode = -1;
        maquina(RAM, copiaPos);
        int posAnterior = copiaPos[2].endereco2;

        // Pega o termo anterior
        Instrucao pegaTermoAnterior[3];
        pegaTermoAnterior[0].opcode = 3;
        pegaTermoAnterior[0].endereco1 = 1;
        pegaTermoAnterior[0].endereco2 = posAnterior;

        pegaTermoAnterior[1].opcode = 5;
        pegaTermoAnterior[1].endereco1 = 1;
        pegaTermoAnterior[1].endereco2 = -1;

        pegaTermoAnterior[2].opcode = -1;

        maquina(RAM, pegaTermoAnterior);
        int termoAnterior = pegaTermoAnterior[1].endereco2;

        //razao
        Instrucao pegaRazao[3];
        pegaRazao[0].opcode = 3;
        pegaRazao[0].endereco1 = 1;
        pegaRazao[0].endereco2 = 2;

        pegaRazao[1].opcode = 5;
        pegaRazao[1].endereco1 = 1;
        pegaRazao[1].endereco2 = -1;

        pegaRazao[2].opcode = -1;
        maquina(RAM, pegaRazao);
        int razaoRam = pegaRazao[1].endereco2;

        programaMultiplica(RAM, termoAnterior, razaoRam);
        //RAM -> [termoAtual, lixo, razao, contadorPosPG, contadorwhile, 1, A1, 0....]

        Instrucao pegaPosAtual[3];
        pegaPosAtual[0].opcode = 3;
        pegaPosAtual[0].endereco1 = 1;
        pegaPosAtual[0].endereco2 = 3;

        pegaPosAtual[1].opcode = 5;
        pegaPosAtual[1].endereco1 = 1;
        pegaPosAtual[1].endereco2 = -1;
        
        pegaPosAtual[2].opcode = -1;;  
        maquina(RAM, pegaPosAtual);
        int posAtual = pegaPosAtual[1].endereco2;

        Instrucao moveResultado[3];
        moveResultado[0].opcode = 3;
        moveResultado[0].endereco1 = 1;
        moveResultado[0].endereco2 = 0;

        moveResultado[1].opcode = 2;
        moveResultado[1].endereco1 = 1;
        moveResultado[1].endereco2 = posAtual;

        moveResultado[2].opcode = -1;
        maquina(RAM,moveResultado);
        
        Instrucao attPos[2]; //att pos prox loop
        attPos[0].opcode = 0;
        attPos[0].endereco1 = 3;
        attPos[0].endereco2 = 5;
        attPos[0].endereco3 = 3;

        attPos[1].opcode = -1;

        maquina(RAM, attPos);

        Instrucao decrementaContador[4]; //contador--
        decrementaContador[0].opcode = 1;
        decrementaContador[0].endereco1 = 4;
        decrementaContador[0].endereco2 = 5;
        decrementaContador[0].endereco3 = 4;

        decrementaContador[1].opcode = 3;
        decrementaContador[1].endereco1 = 1;
        decrementaContador[1].endereco2 = 4;

        decrementaContador[2].opcode = 5;
        decrementaContador[2].endereco1 = 1;
        decrementaContador[2].endereco2 = -1;

        decrementaContador[3].opcode = -1;
        maquina(RAM, decrementaContador);
        contadorWhile = decrementaContador[2].endereco2;
    }

    Instrucao zeraRam[3];
    zeraRam[0].opcode = 4;
    zeraRam[0].endereco1 = 1;
    zeraRam[0].endereco2 = 0;

    zeraRam[1].opcode = 2;
    zeraRam[1].endereco1 = 1;
    zeraRam[1].endereco2 = 1;

    zeraRam[2].opcode = -1;
    maquina(RAM, zeraRam);

    for (int i = 6; i < (6 + qtdTermos); i++) {

        Instrucao somaPG[2];
        somaPG[0].opcode = 0;
        somaPG[0].endereco1 = 1;
        somaPG[0].endereco2 = i;
        somaPG[0].endereco3 = 1;

        somaPG[1].opcode = -1;
        maquina(RAM, somaPG);
    }

    printf("P.G. (a1 = %d, r = %d) -> (", A1, razao);
    for (int i = 6; i < (6 + qtdTermos); i++) {

        Instrucao pegaTermo[3]
        pegaTermo[0].opcode = 3;
        pegaTermo[0].endereco1 = 1;
        pegaTermo[0].endereco2 = i;

        pegaTermo[1].opcode = 5;
        pegaTermo[1].endereco1 = 1;
        pegaTermo[1].endereco2 = -1;

        pegaTermo[2].opcode = -1;
        maquina(RAM, pegaTermo);
        int termo = pegaTermo[1].endereco2;
        printf("%d", termo);
        if (i < (5 + qtdTermos))
            printf(", ");  
    }
    printf(")\n");

    Instrucao pegaSoma[3];
    pegaSoma[0].opcode = 3;
    pegaSoma[0].endereco1 = 1;
    pegaSoma[0].endereco2 = 1;

    pegaSoma[1].opcode = 5;
    pegaSoma[1].endereco1 = 1;
    pegaSoma[1].endereco2 = -1;

    pegaSoma[2].opcode = -1;
    maquina(RAM, pegaSoma);
    int somaPA = pegaSoma[1].endereco2;

    printf("Soma dos termos: %d\n", somaPA);

    // printRam(RAM, (6 + qtdTermos));
    liberaRAM(RAM);
}

