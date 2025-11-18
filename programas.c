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
/*
 -1 - Fim do programa
 0 - soma                                   [OP, end1, end2, endSalvar]
 1 - subtracao                              [OP, end1, end2, endSalvar]
 2 - reg -> ram                             [OP, regFonte, posRam, -1]
 3 - ram -> reg                             [OP, regDestino, posRam, -1]
 4 - copia conteudo do exterior pro reg     [OP, regDestino, conteudo, -1]
 5 - copia conteudo do reg p/ exterior      [OP, regFonte, destino, -1]
*/ 
void programaMultiplica(int * RAM, int multiplicando, int multiplicador) { // RESULTADO = RAM[0]
    int ramLocal = 0; //indica se é um programa isolado ou que faz parte de outro maior (se faz, usa a ram do programa principal)
    if (RAM == NULL) {
        RAM = criaRam_vazia(2);
        ramLocal = 1;
    }

    //Limpa o RAM[0] pros casos de multiplica ser um programa auxiliar
    salvaUmValor(RAM, 0, 0);

    // Move multiplicando pro reg -> depois manda do reg pra ram
    salvaUmValor(RAM, 1, multiplicando);
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

    //[resultado, LIXO]

    int result;
    extraiRAM(RAM, 0, &result);
    printf("MULT: %d x %d = %d\n", multiplicando, multiplicador, result);
    

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
    salvaUmValor(RAM, 3, 0);


    salvaDoisValores(RAM, 0, dividendo, 1, divisor);
    //RAM = [dividendo, divisor, 0, 0]

    // Salva '1' na ram para atuar como somador do contador
    salvaUmValor(RAM, 2, 1);
    //RAM = [dividendo, divisor, 1, 0]

    //Subtrai o divisor do dividendo, incrementa 1 no contador e atualiza o dividendo
    while (dividendo >= divisor) {
        Instrucao div[3];

        div[0].opcode = 1;
        div[0].endereco1 = 0;
        div[0].endereco2 = 1;
        div[0].endereco3 = 0;

        div[1].opcode = 0;
        div[1].endereco1 = 2;
        div[1].endereco2 = 3;
        div[1].endereco3 = 3;

        div[2].opcode = -1;
        maquina(RAM, div);
        extraiRAM(RAM, 0, &dividendo);
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
    salvaDoisValores(RAM, 2, radicando, 6, 1);
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
        int y; 
        extraiRAM(RAM, 5, &y);
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

        extraiRAM(RAM, 4, &result);
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
    salvaDoisValores(RAM, 6, A1, 2, razao);
    salvaDoisValores(RAM, 5, 1, 3, 7);
    //RAM -> [0, 0, razao, contadorPos, 0, 1, A1, 0....]


    salvaUmValor(RAM, 4, qtdTermos);
    Instrucao subCounter[2];
    subCounter[0].opcode = 1;
    subCounter[0].endereco1 = 4;
    subCounter[0].endereco2 = 5;
    subCounter[0].endereco3 = 4;

    subCounter[1].opcode = -1;
    maquina(RAM, subCounter);

    int contadorWhile;
    extraiRAM(RAM, 4, &contadorWhile);
    //RAM -> [0, 0, razao, contadorPos, contadorWhile, 1, A1, 0....]

    while (contadorWhile > 0) {
        // Pega a posicao anterior para calcular pra proxima
        Instrucao copiaPOS[2];
        copiaPOS[0].opcode = 1;
        copiaPOS[0].endereco1 = 3;
        copiaPOS[0].endereco2 = 5;
        copiaPOS[0].endereco3 = 1;

        copiaPOS[1].opcode = -1;
        maquina(RAM, copiaPOS);
        int posAnterior;
        extraiRAM(RAM, 1, &posAnterior);

        int pos;
        extraiRAM(RAM, 3, &pos);

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
        Instrucao attContador[2];
        attContador[0].opcode = 1;
        attContador[0].endereco1 = 4;
        attContador[0].endereco2 = 5;
        attContador[0].endereco3 = 4;

        attContador[1].opcode = -1;
        maquina(RAM, attContador);
        //RAM -> [0, 0, razao, contadorPos, contadorWhile - 1, 1, A1, P.A....]
        extraiRAM(RAM, 4, &contadorWhile);
    }

    salvaUmValor(RAM, 4, 0);
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
        int termo;
        extraiRAM(RAM, i, &termo);
        printf("%d ", termo); 
    }
    printf(")\n");

    int somaPA;
    extraiRAM(RAM, 4, &somaPA);

    printf("Soma dos termos: %d\n", somaPA);
    // printRam(RAM, (6+qtdTermos));

    liberaRAM(RAM);
}

void programaPG(int A1, int razao, int qtdTermos) {

    int *RAM = criaRam_vazia(6 + qtdTermos);

    //RAM -> [programamultiplica, programamultiplica, razao, contadorposPG, contadorWhile, 1, A1, pg....]
    
    salvaDoisValores(RAM, 5, 1, 2, razao);
    salvaDoisValores(RAM, 6, A1, 3, 7);
    salvaUmValor(RAM, 4, qtdTermos);
    //RAM -> [0, 0, razao, contadorPosPG, contadorwhile, 1, A1, 0....]

    Instrucao subCounter[2];
    subCounter[0].opcode = 1;
    subCounter[0].endereco1 = 4;
    subCounter[0].endereco2 = 5;
    subCounter[0].endereco3 = 4;

    subCounter[1].opcode = -1;
    maquina(RAM, subCounter);

    int contadorWhile;
    extraiRAM(RAM, 4, &contadorWhile);
    while (contadorWhile > 0) {

        // Pega pos anterior
        Instrucao copiaPos[2];
        copiaPos[0].opcode = 1;
        copiaPos[0].endereco1 = 3;
        copiaPos[0].endereco2 = 5;
        copiaPos[0].endereco3 = 0;

        copiaPos[1].opcode = -1;
        maquina(RAM, copiaPos);
    
        int posAnterior;
        extraiRAM(RAM, 0, &posAnterior);

        int termoAnterior;
        extraiRAM(RAM, posAnterior, &termoAnterior);

        int razaoRam;
        extraiRAM(RAM, 2, &razaoRam);

        programaMultiplica(RAM, termoAnterior, razaoRam);
        //RAM -> [termoAtual, lixo, razao, contadorPosPG, contadorwhile, 1, A1, 0....]

        int posAtual;
        extraiRAM(RAM, 3, &posAtual);

        Instrucao moveResultado[3];
        moveResultado[0].opcode = 3;
        moveResultado[0].endereco1 = 1;
        moveResultado[0].endereco2 = 0;

        moveResultado[1].opcode = 2;
        moveResultado[1].endereco1 = 1;
        moveResultado[1].endereco2 = posAtual;

        moveResultado[2].opcode = -1;
        maquina(RAM, moveResultado);
        
        Instrucao attPos[2]; //att pos prox loop
        attPos[0].opcode = 0;
        attPos[0].endereco1 = 3;
        attPos[0].endereco2 = 5;
        attPos[0].endereco3 = 3;

        attPos[1].opcode = -1;

        maquina(RAM, attPos);

        Instrucao decrementaContador[2]; //contador--
        decrementaContador[0].opcode = 1;
        decrementaContador[0].endereco1 = 4;
        decrementaContador[0].endereco2 = 5;
        decrementaContador[0].endereco3 = 4;

        decrementaContador[1].opcode = -1;
        maquina(RAM, decrementaContador);

        extraiRAM(RAM, 4, &contadorWhile);
    }


    salvaUmValor(RAM, 1, 0);

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

        int termo;
        extraiRAM(RAM, i, &termo);
        printf("%d", termo);
        if (i < (5 + qtdTermos))
            printf(", ");  
    }
    printf(")\n");

    int somaPA;
    extraiRAM(RAM, 1, &somaPA);
    printf("Soma dos termos: %d\n", somaPA);

    // printRam(RAM, (6 + qtdTermos));
    liberaRAM(RAM);
}

void programaTrianguloRet(int catA, int catB) {
    int *RAM = criaRam_vazia(10);

    // Esquema Ram -> [LIXO, LIXO, LIXO, LIXO, hipotenusa, area, perimetro, cateto A, cateto B, 0]


    salvaDoisValores(RAM, 7, catA, 8, catB);
    //Ram -> [0, 0, 0, 0, 0, 0, 0, cateto A, cateto B, 0]

    int catA_maq;
    int catB_maq;
    extraiRAM(RAM, 7, &catA_maq);
    extraiRAM(RAM, 8, &catB_maq);

    programaMultiplica(RAM, catA_maq, catA_maq);
    //Ram -> [A², 0, 0, 0, 0, 0, 0, cateto A, cateto B, 0]

    Instrucao moveCAT[3];
    moveCAT[0].opcode = 3;
    moveCAT[0].endereco1 = 1;
    moveCAT[0].endereco2 = 0;

    moveCAT[1].opcode = 2;
    moveCAT[1].endereco1 = 1;
    moveCAT[1].endereco2 = 2;

    moveCAT[2].opcode = -1;
    maquina(RAM, moveCAT);
    programaMultiplica(RAM, catB_maq, catB_maq);
    //Ram -> [B², 0, A², 0, 0, 0, 0, cateto A, cateto B, 0]

    Instrucao somaCAT[2];

    somaCAT[0].opcode = 0;
    somaCAT[0].endereco1 = 0;
    somaCAT[0].endereco2 = 2;
    somaCAT[0].endereco3 = 9;

    somaCAT[1].opcode = -1;
    maquina(RAM, somaCAT);
    //Ram -> [B², 0, A², 0, 0, 0, 0, cateto A, cateto B, A²+B²]

    int soma;
    extraiRAM(RAM, 9, &soma); 

    programaRaizQuad(RAM, soma);
    //Ram -> [raiz, LIXO, LIXO, LIXO, LIXO, LIXO, LIXO, cateto A, cateto B, A²+B²]

    Instrucao moveResult[3];
    moveResult[0].opcode = 3;
    moveResult[0].endereco1 = 1;
    moveResult[0].endereco2 = 0;

    moveResult[1].opcode = 2;
    moveResult[1].endereco1 = 1;
    moveResult[1].endereco2 = 4;

    moveResult[2].opcode = -1;
    maquina(RAM, moveResult);
    //Ram -> [raiz, LIXO, LIXO, LIXO, hipotenusa, LIXO, LIXO, cateto A, cateto B, LIXO]

    programaMultiplica(RAM, catA_maq, catB_maq);
    //Ram -> [result, LIXO, LIXO, LIXO, hipotenusa, LIXO, LIXO, cateto A, cateto B, LIXO]

    int mult;
    extraiRAM(RAM, 0, &mult);
    programaDivisao(RAM, mult, 2);

    Instrucao moveArea[3];
    moveArea[0].opcode = 3;
    moveArea[0].endereco1 = 1;
    moveArea[0].endereco2 = 0;

    moveArea[1].opcode = 2;
    moveArea[1].endereco1 = 1;
    moveArea[1].endereco2 = 5;

    moveArea[2].opcode = -1;
    maquina(RAM, moveArea);
    //Ram -> [LIXO, LIXO, LIXO, LIXO, hipotenusa, area, LIXO, cateto A, cateto B, LIXO]

    Instrucao calcPerimetro[3];
    calcPerimetro[0].opcode = 0;
    calcPerimetro[0].endereco1 = 7;
    calcPerimetro[0].endereco2 = 8;
    calcPerimetro[0].endereco3 = 6;

    calcPerimetro[1].opcode = 0;
    calcPerimetro[1].endereco1 = 6;
    calcPerimetro[1].endereco2 = 4;
    calcPerimetro[1].endereco3 = 6;

    calcPerimetro[2].opcode = -1;
    maquina(RAM, calcPerimetro);
    //Ram -> [LIXO, LIXO, LIXO, LIXO, hipotenusa, area, perimetro, cateto A, cateto B, LIXO]

    int hip;
    int area;
    int perimetro;
    extraiRAM(RAM, 4, &hip);
    extraiRAM(RAM, 5, &area);
    extraiRAM(RAM, 6, &perimetro);

    printf("\t Triângulo Retangulo (a = %d, b = %d)\n", catA_maq, catB_maq);
    printf("Hipotenusa = %d\n", hip);
    printf("Area = %d\n", area);
    printf("Perimetro = %d\n", perimetro);

    liberaRAM(RAM);
}

void programaBhaskara(int a, int b, int c) {
    
}

