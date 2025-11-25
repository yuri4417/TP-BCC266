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
    int ramLocal = 0;
    if (RAM == NULL) {
        RAM = criaRam_vazia(2);
        ramLocal = 1;
    }

    if (multiplicador > multiplicando) {
        int temp = multiplicador;
        multiplicador = multiplicando;
        multiplicando = temp;
    }
    salvaUmValor(RAM, 0, 0);
    salvaUmValor(RAM, 1, multiplicando);
    // RAM = [0, multiplicando]

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

    int ramLocal = 0;
    if (RAM == NULL) {
        RAM = criaRam_vazia(4);
        ramLocal = 1;
    }

    salvaUmValor(RAM, 3, 0);


    salvaDoisValores(RAM, 0, dividendo, 1, divisor);
    //RAM = [dividendo, divisor, 0, 0]

    salvaUmValor(RAM, 2, 1);
    //RAM = [dividendo, divisor, 1, 0]

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

    Instrucao organiza[6]; 
    organiza[0].opcode = 3;   
    organiza[0].endereco1 = 1;
    organiza[0].endereco2 = 0;

    organiza[1].opcode = 3;   
    organiza[1].endereco1 = 2;
    organiza[1].endereco2 = 3;

    
    organiza[2].opcode = 2;   
    organiza[2].endereco1 = 1;
    organiza[2].endereco2 = 1;

    
    organiza[3].opcode = 2;   
    organiza[3].endereco1 = 2;
    organiza[3].endereco2 = 0;

    
    organiza[4].opcode = 5;   
    organiza[4].endereco1 = 2;
    organiza[4].endereco2 = -1;

    organiza[5].opcode = -1;
    maquina(RAM, organiza);
    // RAM = [resultado, resto, lixo, lixo]

    int result = organiza[4].endereco2;
    printf("DIV: Resultado da divisao: %d\n", result);

    // printRam(RAM, 4);
    if (ramLocal)
        liberaRAM(RAM);
}

void programaSQRT(int* RAM, int radicando) { //RESULTADO == RAM[0]
    int ramLocal = 0; 
    if (RAM == NULL) {
        RAM = criaRam_vazia(7);
        ramLocal = 1;
    }

    //Esquema final RAM -> [programaMultiplica, programaMultiplica, radicando, RESULTADO FINAL, result, counter, 1]



    salvaDoisValores(RAM, 2, radicando, 6, 1);
    //ram -> [0, 0, radicando, 0, 0, 0, 1]

    int result = INT_MAX;
    while (result >= 0) {

        Instrucao copia[3];
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
        sum[0].opcode = 0;
        sum[0].endereco1 = 5;
        sum[0].endereco2 = 6;
        sum[0].endereco3 = 5;

        sum[1].opcode = -1;

        maquina(RAM, sum);

        int y; 
        extraiRAM(RAM, 5, &y);
        programaMultiplica(RAM, y, y);
        //RAM -> [y^2, y, radicando, counter, 0, counter + 1, 1]

        Instrucao sub[2];
        sub[0].opcode = 1;
        sub[0].endereco1 = 2;
        sub[0].endereco2 = 0;
        sub[0].endereco3 = 4;

        sub[1].opcode = -1;

        maquina(RAM, sub);
        //RAM -> [y^2, y, radicando, counter, result, counter + 1, 1]

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
    int *RAM = criaRam_vazia(7 + (qtdTermos - 1)); 
    
    //RAM -> [programamultiplica, programamultiplica, razao, contadorPosPA, contadorWhile, 1, A1, P.A...]

  
    salvaDoisValores(RAM, 6, A1, 2, razao);
    salvaDoisValores(RAM, 5, 1, 3, 7);


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
        
        Instrucao attPos[2];
        attPos[0].opcode = 0;
        attPos[0].endereco1 = 3;
        attPos[0].endereco2 = 5;
        attPos[0].endereco3 = 3;

        attPos[1].opcode = -1;
        maquina(RAM, attPos);

        Instrucao attContador[2];
        attContador[0].opcode = 1;
        attContador[0].endereco1 = 4;
        attContador[0].endereco2 = 5;
        attContador[0].endereco3 = 4;

        attContador[1].opcode = -1;
        maquina(RAM, attContador);
        //RAM -> [0, 0, razao, contadorPos + 1, contadorWhile - 1, 1, A1, P.A....]
        extraiRAM(RAM, 4, &contadorWhile);
    }

    salvaUmValor(RAM, 4, 0);

    for (int i = 6; i < (6 + qtdTermos); i++) {
        Instrucao somaPA[2];
        somaPA[0].opcode = 0;
        somaPA[0].endereco1 = 4;
        somaPA[0].endereco2 = i;
        somaPA[0].endereco3 = 4;

        somaPA[1].opcode = -1;
        maquina(RAM, somaPA);
    }
    //RAM -> [0, 0, razao, contadorPos, somaPA, 1, A1, P.A....]

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
        //RAM -> [termoAtual, LIXO, razao, contadorPosPG, contadorwhile, 1, A1, 0....]

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
        
        Instrucao attPos[2]; 
        attPos[0].opcode = 0;
        attPos[0].endereco1 = 3;
        attPos[0].endereco2 = 5;
        attPos[0].endereco3 = 3;

        attPos[1].opcode = -1;

        maquina(RAM, attPos);

        Instrucao decrementaContador[2];
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

    int somaPG;
    extraiRAM(RAM, 1, &somaPG);
    printf("Soma dos termos: %d\n", somaPG);

    // printRam(RAM, (6 + qtdTermos));
    liberaRAM(RAM);
}

void programaTrianguloRet(int catA, int catB) {
    int *RAM = criaRam_vazia(10);

    // Esquema Ram -> [LIXO, LIXO, LIXO, LIXO, hipotenusa, area, perimetro, cateto A, cateto B, 0]


    salvaDoisValores(RAM, 7, catA, 8, catB);

    int catA_maq;
    int catB_maq;
    extraiRAM(RAM, 7, &catA_maq);
    extraiRAM(RAM, 8, &catB_maq);

    programaMultiplica(RAM, catA_maq, catA_maq);
    //Ram -> [A², LIXO, 0, 0, 0, 0, 0, cateto A, cateto B, 0]

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

    Instrucao somaCAT[2];
    somaCAT[0].opcode = 0;
    somaCAT[0].endereco1 = 0;
    somaCAT[0].endereco2 = 2;
    somaCAT[0].endereco3 = 9;

    somaCAT[1].opcode = -1;
    maquina(RAM, somaCAT);
    //Ram -> [B², LIXO, A², 0, 0, 0, 0, cateto A, cateto B, A²+B²]

    int soma;
    extraiRAM(RAM, 9, &soma); 

    programaSQRT(RAM, soma);

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

void programaConverteBinario(int numeroDec) {
    int *RAM = criaRam_vazia(100);

    // Esquema RAM -> [LIXO, LIXO, LIXO, LIXO, ptr, numeroDec, 1, LSB, ....]

    salvaDoisValores(RAM, 5, numeroDec, 6, 1);
    salvaUmValor(RAM, 4, 7); 

    //RAM -> [0, 0, 0, 0, 7, numeroDec, 1, 0, ....]

    int numero;
    extraiRAM(RAM, 5, &numero);

    while (numero > 0) {
        programaDivisao(RAM, numero, 2);
        //RAM -> [result, resto, LIXO, LIXO, ptr, numeroDec, 1, 0, ....]

        extraiRAM(RAM, 0, &numero); 

        int resto;
        extraiRAM(RAM, 1, &resto);

        int ptr_preencher;
        extraiRAM(RAM, 4, &ptr_preencher);

        salvaUmValor(RAM, ptr_preencher, resto);

        Instrucao addPtr[2];
        addPtr[0].opcode = 0;
        addPtr[0].endereco1 = 4;
        addPtr[0].endereco2 = 6;
        addPtr[0].endereco3 = 4;

        addPtr[1].opcode = -1;
        maquina(RAM, addPtr);
    }
    
    Instrucao subPtr[2];
    subPtr[0].opcode = 1;
    subPtr[0].endereco1 = 4;
    subPtr[0].endereco2 = 6;
    subPtr[0].endereco3 = 4;

    subPtr[1].opcode = -1;
    maquina(RAM, subPtr);

    int ptr;
    extraiRAM(RAM, 4, &ptr);

    //RAM -> [LIXO, LIXO, LIXO, LIXO, posMSB, numeroDec, 1, LSB, ....]

    printf("Conversao (%d)_10 = (", numeroDec);
    for (int i = ptr; i >= 7; i--) {
        int bit;
        extraiRAM(RAM, i, &bit);
        printf("%d", bit);
    }
    printf(")_2\n");

    liberaRAM(RAM);
}

void programaArranjoSimples(int n, int k) {
    
    // [LIXO, LIXO, LIXO, LIXO, n, k, n!, (n-k)!]
    int *RAM = criaRam_vazia(8);

    salvaDoisValores(RAM, 4, n, 5, k);


    int n_maq;
    extraiRAM(RAM, 4, &n_maq);
    programaFatorial(RAM, n_maq);

    int nFat;
    extraiRAM(RAM, 0, &nFat);
    
    salvaUmValor(RAM, 6, nFat);

    Instrucao sub[2];
    sub[0].opcode = 1;
    sub[0].endereco1 = 4;
    sub[0].endereco2 = 5;
    sub[0].endereco3 = 0;

    sub[1].opcode = -1;
    maquina(RAM, sub);
    int subResult;
    extraiRAM(RAM, 0, &subResult);

    programaFatorial(RAM, subResult);
    int subFat;
    extraiRAM(RAM, 0, &subFat);

    programaDivisao(RAM, nFat, subFat);

    int arranjo;
    extraiRAM(RAM, 0, &arranjo);
    printf("A(%d, %d) = %d\n", n, k, arranjo);

    liberaRAM(RAM);
}

void programaCombSimples(int n, int k) {

    //RAM -> [LIXO, LIXO, LIXO, LIXO, n, k, n!, k!, (n - k)!, resultado]
    int *RAM = criaRam_vazia(10);

    salvaDoisValores(RAM, 4, n, 5, k);

    
    int nFat;
    programaFatorial(RAM, n);
    extraiRAM(RAM, 0, &nFat);
    
    int kFat;
    programaFatorial(RAM, k);
    extraiRAM(RAM, 0, &kFat);

    salvaDoisValores(RAM, 6, nFat, 7, kFat);

    Instrucao sub[2];
    sub[0].opcode = 1;
    sub[0].endereco1 = 4;
    sub[0].endereco2 = 5;
    sub[0].endereco3 = 8;

    sub[1].opcode = -1;
    maquina(RAM, sub);
    int result;
    extraiRAM(RAM, 8, &result);

    programaFatorial(RAM, result);


    int nkFat;
    extraiRAM(RAM, 0, &nkFat);

    programaMultiplica(RAM, kFat, nkFat);

    int fat;
    extraiRAM(RAM, 0, &fat);

    programaDivisao(RAM, nFat, fat);

    int comb;
    extraiRAM(RAM, 0, &comb);
    printf("C(%d, %d) = %d\n", n, k, comb);

    liberaRAM(RAM);
}

void programaFormulaHeron(int a, int b, int c) {

    // [LIXO, LIXO, LIXO, LIXO, LIXO, LIXO, LIXO, LIXO, a, b, c, s]
    int *RAM = criaRam_vazia(12);

    salvaDoisValores(RAM, 8, a, 9, b);
    salvaUmValor(RAM, 10, c);

    Instrucao soma[2];
    soma[0].opcode = 0; 
    soma[0].endereco1 = 8; 
    soma[0].endereco2 = 9; 
    soma[0].endereco3 = 0; 
    soma[1].opcode = -1;
    maquina(RAM, soma);

    Instrucao soma2[2];
    soma2[0].opcode = 0; 
    soma2[0].endereco1 = 0; 
    soma2[0].endereco2 = 10; 
    soma2[0].endereco3 = 0; 
    soma2[1].opcode = -1;
    maquina(RAM, soma2);

    int perimetro;
    extraiRAM(RAM, 0, &perimetro);
    
    programaDivisao(RAM, perimetro, 2);

    Instrucao moveS[3];
    moveS[0].opcode = 3; 
    moveS[0].endereco1 = 1; 
    moveS[0].endereco2 = 0; 
    
    moveS[1].opcode = 2; 
    moveS[1].endereco1 = 1; 
    moveS[1].endereco2 = 11; 
    
    moveS[2].opcode = -1;
    maquina(RAM, moveS);

    Instrucao sub[4];
    
    sub[0].opcode = 1; 
    sub[0].endereco1 = 11; 
    sub[0].endereco2 = 8;  
    sub[0].endereco3 = 8;  

    sub[1].opcode = 1;
    sub[1].endereco1 = 11; 
    sub[1].endereco2 = 9;  
    sub[1].endereco3 = 9;  

    sub[2].opcode = 1;
    sub[2].endereco1 = 11; 
    sub[2].endereco2 = 10; 
    sub[2].endereco3 = 10; 

    sub[3].opcode = -1;
    maquina(RAM, sub);

    int S, SA, SB, SC;
    extraiRAM(RAM, 11, &S);
    extraiRAM(RAM, 8, &SA);
    extraiRAM(RAM, 9, &SB);
    extraiRAM(RAM, 10, &SC);

    programaMultiplica(RAM, S, SA);
    extraiRAM(RAM, 0, &S);

    programaMultiplica(RAM, S, SB);
    extraiRAM(RAM, 0, &S);

    programaMultiplica(RAM, S, SC);
    extraiRAM(RAM, 0, &S);
    
    programaSQRT(RAM, S);

    int area;
    extraiRAM(RAM, 0, &area);
    
    printf("Área do triângulo (%d, %d, %d) = %d\n", a, b, c, area);

    liberaRAM(RAM);
}

void programaConverteSegundos(int segundos) {

    int *RAM = criaRam_vazia(4);

    programaDivisao(RAM, segundos, 86400);
    int dias;
    extraiRAM(RAM, 0, &dias);
    int resto1;
    extraiRAM(RAM, 1, &resto1);

    programaDivisao(RAM, resto1, 3600); 
    int horas;
    extraiRAM(RAM, 0, &horas);

    int resto2;
    extraiRAM(RAM, 1, &resto2);
    programaDivisao(RAM, resto2, 60);
    int minutos;
    extraiRAM(RAM, 0, &minutos);
    int segs;
    extraiRAM(RAM, 1, &segs);
    printf("Conversão de %d segundos = %d dias, %d horas, %d minutos e %d segundos\n", segundos, dias, horas, minutos, segs);

    liberaRAM(RAM);
}

void programaMultiplicaMatrizes(int *matA, int *matB, int N) {

    int *RAM = criaRam_vazia(200);

    int INICIO_A = 20;
    int INICIO_B = 60;
    int INICIO_C = 100;

    int counter = 0;
    salvaUmValor(RAM, 4, 0);
    for (int i = 0; i < N; i++) 
        for (int j = 0; j < N; j++) {

            // pos ram = inicio + (I * n) + j

            programaMultiplica(RAM, i, N);
            salvaUmValor(RAM, 1, j);

            Instrucao somaJ[2];
            somaJ[0].opcode = 0;
            somaJ[0].endereco1 = 0;
            somaJ[0].endereco2 = 1;
            somaJ[0].endereco3 = 0;
            
            somaJ[1].opcode = -1;
            maquina(RAM, somaJ);

            salvaUmValor(RAM, 1, INICIO_A);
            maquina(RAM, somaJ);

            int posMatA;
            extraiRAM(RAM, 0, &posMatA);

            salvaUmValor(RAM, posMatA, matA[counter]);


            salvaUmValor(RAM, 3, 1);
            Instrucao somaCount[2];
            somaCount[0].opcode = 0;
            somaCount[0].endereco1 = 3;
            somaCount[0].endereco2 = 4;
            somaCount[0].endereco3 = 4;

            somaCount[1].opcode = -1;
            maquina(RAM, somaCount);
            extraiRAM(RAM, 4, &counter);
        }
    

    salvaUmValor(RAM, 4, 0);
    extraiRAM(RAM, 4, &counter);
    for (int i = 0; i < N; i++) 
        for (int j = 0; j < N; j++) {

            // pos ram = inicio + (I * n) + j

            programaMultiplica(RAM, i, N);
            salvaUmValor(RAM, 1, j);

            Instrucao somaJ[2];
            somaJ[0].opcode = 0;
            somaJ[0].endereco1 = 0;
            somaJ[0].endereco2 = 1;
            somaJ[0].endereco3 = 0;
            
            somaJ[1].opcode = -1;
            maquina(RAM, somaJ);

            salvaUmValor(RAM, 1, INICIO_B);
            maquina(RAM, somaJ);

            int posMatB;
            extraiRAM(RAM, 0, &posMatB);

            salvaUmValor(RAM, posMatB, matB[counter]);


            salvaUmValor(RAM, 3, 1);
            Instrucao somaCount[2];
            somaCount[0].opcode = 0;
            somaCount[0].endereco1 = 3;
            somaCount[0].endereco2 = 4;
            somaCount[0].endereco3 = 4;

            somaCount[1].opcode = -1;
            maquina(RAM, somaCount);
            extraiRAM(RAM, 4, &counter);
        }
    

    //Calculo

    for (int i = 0; i < N; i++) 
        for (int j = 0; j < N; j++) {

            // pos ram = inicio + (i * n) + j
            programaMultiplica(RAM, i, N);
            salvaUmValor(RAM, 1, j);


            Instrucao somaJ[2];
            somaJ[0].opcode = 0;
            somaJ[0].endereco1 = 0;
            somaJ[0].endereco2 = 1;
            somaJ[0].endereco3 = 0;
            
            somaJ[1].opcode = -1;
            maquina(RAM, somaJ);

            salvaUmValor(RAM, 1, INICIO_C);
            maquina(RAM, somaJ);

            int posMatC;
            extraiRAM(RAM, 0, &posMatC);

            salvaUmValor(RAM, posMatC, 0);

            for (int k = 0; k < N; k++) {

                programaMultiplica(RAM, i, N);
                salvaUmValor(RAM, 1, k);

                // INICIO_A + (i * N) + k
                Instrucao somaK[2];
                somaK[0].opcode = 0;
                somaK[0].endereco1 = 0;
                somaK[0].endereco2 = 1;
                somaK[0].endereco3 = 0;
                
                somaK[1].opcode = -1;
                maquina(RAM, somaK);

                salvaUmValor(RAM, 1, INICIO_A);
                maquina(RAM, somaK);

                int posA;
                extraiRAM(RAM, 0, &posA);


                // inicios + (k * N) + j
                programaMultiplica(RAM, k, N);
                salvaUmValor(RAM, 1, j);

                maquina(RAM, somaK);

                salvaUmValor(RAM, 1, INICIO_B);
                maquina(RAM, somaK);

                int posB;
                extraiRAM(RAM, 0, &posB);

                int celA, celB;
                extraiRAM(RAM, posA, &celA);
                extraiRAM(RAM, posB, &celB);

                programaMultiplica(RAM, celA, celB);

                Instrucao somaCel[2];
                somaCel[0].opcode = 0;
                somaCel[0].endereco1 = 0;
                somaCel[0].endereco2 = posMatC;
                somaCel[0].endereco3 = posMatC;

                somaCel[1].opcode = -1;
                maquina(RAM, somaCel);

            }
        }
    
    printf("MAT A x B:\n");
    for (int i = 0; i < N; i++)  {
        for (int j = 0; j < N; j++) {
            //inicio + (i * N) + j

            programaMultiplica(RAM, i, N);
            salvaUmValor(RAM, 1, j);

            Instrucao somaJ[2];
            somaJ[0].opcode = 0;
            somaJ[0].endereco1 = 0;
            somaJ[0].endereco2 = 1;
            somaJ[0].endereco3 = 0;

            somaJ[1].opcode = -1;
            maquina(RAM, somaJ);

            salvaUmValor(RAM, 1, INICIO_C);

            maquina(RAM, somaJ);

            int posC;
            extraiRAM(RAM, 0, &posC);

            int celC;
            extraiRAM(RAM, posC, &celC);

            printf("%d ", celC);
        }
        printf("\n");
    }
    liberaRAM(RAM);
}



void programaPotencia(int *RAM, int base, int expoente)//RESULTADO RAM[0]
{
    int ramLocal = 0;
    if (RAM == NULL) {
        ramLocal = 1;
        RAM = criaRam_vazia(5);
        
    }
    salvaDoisValores(RAM, 1, base, 2 , expoente);

    int externoBase, externoExpoente;

    extraiRAM(RAM,1, &externoBase);

    extraiRAM(RAM,2, &externoExpoente);
    int basePositiva;
    
    if (externoBase == 0) {

        if (externoExpoente <0) 
            printf("Erro: %d ^ %d Divisao por zero\n", externoBase, externoExpoente); 
        else if (externoExpoente == 0) 
            printf("Erro: %d ^ %d Resultado indeterminado\n", externoBase, externoExpoente); 
        else 
            printf("%d ^ %d = %d\n", externoBase, externoExpoente, 0); 
        
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }

    if (externoExpoente == 0) {

        printf("%d ^ %d = %d\n", externoBase, externoExpoente, 1);
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }

    if (externoBase == 1) {

        printf("%d ^ %d = %d\n", externoBase, externoExpoente, 1);
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }

    if (externoBase == -1) {

        if (RAM[2] % 2 == 0) 
            printf("%d ^ %d = %d\n", externoBase, externoExpoente, 1);
        else 
            printf("%d ^ %d = %d\n", externoBase, externoExpoente, -1); 
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }

    if (externoExpoente < 0) {

        printf("%d ^ %d = %d\n", externoBase, externoExpoente, 0);
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }
    programaValorAbsoluto(RAM, externoBase);
    extraiRAM(RAM, 0,&basePositiva);
    
    salvaUmValor(RAM, 0, basePositiva);

    salvaDoisValores(RAM, 1, externoBase, 2, externoExpoente);
    salvaUmValor(RAM, 3, externoBase);

    for(int i = 0; i < externoExpoente - 1; i++)
    {
        int rAtual = RAM[0]; //vai pegar o resultado da última multiplicação, antes de perder o valor
        programaMultiplica(RAM, rAtual, basePositiva);
    }
    extraiRAM(RAM,2, &externoExpoente);

    int resultado;
    
    extraiRAM(RAM,0, &resultado);

    salvaUmValor(RAM, 4, resultado);

    if (externoBase < 0 && (externoExpoente % 2 != 0)) {
        
        extraiRAM(RAM, 0, &resultado);
        programaMultiplica(RAM, resultado, 2);
        Instrucao sub[2];

        sub[0].opcode = 1;
        sub[0].endereco1 = 4;
        sub[0].endereco2 = 0;
        sub[0].endereco3 = 0;

        sub[1].opcode = -1;
        maquina(RAM, sub);
        extraiRAM(RAM, 0, &resultado);
        
    }
    printf("\nValor da potência  %d ^ %d = %d\n\n",externoBase, externoExpoente,  resultado);
     if(ramLocal)
        liberaRAM(RAM);
}

void programaFatorial(int *RAM, int n)//R  = Ram[0]
{
    int ramLocal = 0;
    if (RAM == NULL) {
        RAM = criaRam_vazia(4);
        ramLocal = 1;
    }
    salvaUmValor(RAM, 1, n);
    int externoN;
    extraiRAM(RAM, 1, &externoN);
    
    if (RAM[1] < 0) {
        printf("Fatorial de %d não existe\n", externoN);
        if (ramLocal)
            liberaRAM(RAM);
        return;
    }
    if (RAM[1] == 0) {
        printf("Fatorial: %d! = 1\n", externoN);
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }
    
    salvaUmValor(RAM, 0, 1);
    salvaDoisValores(RAM, 2, 0, 3, 1);
    int contador;
    for (int i = 1; i <= externoN; i++) {
        int rAtual;
        extraiRAM(RAM, 0, &rAtual); 
        salvaUmValor(RAM, 0,0);
        Instrucao movPSomar[2];
        
        movPSomar[0].opcode = 0;
        movPSomar[0].endereco1 = 2;
        movPSomar[0].endereco2 = 3;
        movPSomar[0].endereco3 = 2;
        
        movPSomar[1].opcode = -1;
        maquina(RAM,movPSomar);
        extraiRAM(RAM, 2, &contador);
        programaMultiplica(RAM, rAtual, contador);

    }

    int resultado;
    extraiRAM(RAM, 0, &resultado);

    
    printf("Fatorial: %d! = %d\n", externoN, resultado);

    if (ramLocal)
        liberaRAM(RAM);

}

void programaFibonacci(int *RAM, int n) //R = ram[1]

{
    int ramLocal = 0;
    if (RAM == NULL) {
        RAM = criaRam_vazia(4);
        ramLocal = 1;
    }
    salvaUmValor(RAM, 3, n); // ram[3], recebe o número de termos
    int externoN;
    extraiRAM(RAM, 3,&externoN);
    if(externoN < 0)
    {
        printf("Não existe termo negativo na sequência de Fibonacci!");
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }
    if (externoN == 0) {
        printf("Fibonacci(%d) = 0\n", externoN);
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }
    salvaDoisValores(RAM, 0, 0, 1, 1);
    Instrucao soma[6];
    for(int i = 2; i<=externoN; i++)
    {
        soma[0].opcode  = 0;
        soma[0].endereco1 =0;
        soma[0].endereco2 = 1;
        soma[0].endereco3 = 2;

        soma[1].opcode = 3;   //move o termo 1 para ram[0]
        soma[1].endereco1 = 1; 
        soma[1].endereco2 = 1; 
        
        
        soma[2].opcode = 2;
        soma[2].endereco1 = 1;
        soma[2].endereco2 = 0;

        soma[3].opcode = 3;//move o resultado para ram [1]
        soma[3].endereco1 = 1;
        soma[3].endereco2 = 2;

        soma[4].opcode = 2;
        soma[4].endereco1 = 1;
        soma[4].endereco2 = 1;

        soma[5].opcode = -1;

        maquina(RAM, soma);
    }
    int resultado;
    extraiRAM(RAM, 1, &resultado);

    printf("Fibonacci(%d) = %d\n", externoN, resultado);

    if (ramLocal)
        liberaRAM(RAM);
}

void programaValorAbsoluto(int *RAM, int valor)//Resultado RAM[0]

{   
    int ramLocal = 0;
    if(RAM == NULL)
    {

        RAM = criaRam_vazia(3);
        ramLocal =1;
        if(RAM == NULL)
        {
            printf("Erro ao alocar memória\n\n");
            return ;
        }
    }
    salvaDoisValores(RAM, 1,0,2,valor);
    Instrucao sub[2];
    if(RAM[2] < 0)
    {
        sub[0].opcode = 1;
        sub[0].endereco1 = 1;
        sub[0].endereco2 = 2;
        sub[0].endereco3 = 0;

        sub[1].opcode = -1;
        maquina(RAM, sub);
        extraiRAM(RAM, 0,&valor);
    }    
    else 
    {
        Instrucao soma[2];
        
        soma[0].opcode = 0;
        soma[0].endereco1 = 1;
        soma[0].endereco2 =2;
        soma[0].endereco3 = 0;

        soma[1].opcode = -1;
        maquina(RAM, soma);
        extraiRAM(RAM, 0, &valor);

        printf("\nValor após o absoluto: %d\n", valor);
    }
    if(ramLocal)
        liberaRAM(RAM);




}

void programaSomatorio(int *RAM, int indiceInicial, int nTermos, int valorInicial, int Razao)// Resultado RAM[0]
{
    int criaRam=0;
    if(RAM == NULL) 
    {
        criaRam = 1;
        RAM = criaRam_vazia(6); 
        //RAM[0] = RESULTADO, RAM[1]  = indiceInicial RAM[2] = Razão, RAM[3] =nTermos, RAM[4] = valor inicial 
          
        if(RAM == NULL)
        {
            printf("Erro ao criar a ram, encerrando o programa!");
            return ;
        } 
    }
    salvaUmValor(RAM, 1, indiceInicial);
    Instrucao obterNTermos[2];    
    if(RAM[1] < 0 )
    {
        programaValorAbsoluto(RAM, RAM[1]);
        // o modulo será salvo na posição RAM[0]
        obterNTermos[0].opcode = 0;
        obterNTermos[0].endereco1 = 0;
        salvaUmValor(RAM, 3,nTermos);
        obterNTermos[0].endereco2 = 3;
        obterNTermos[0].endereco3 = 3;
        obterNTermos[1].opcode = -1;
        maquina(RAM, obterNTermos);// em RAM[3] ESTÁ O TOTAL DE TERMOS
    }
    
    else
    {
        obterNTermos[0].opcode=1;
         
        salvaUmValor(RAM, 3, nTermos);
        obterNTermos[0].endereco1 =   3;
        obterNTermos[0].endereco2 =1;
        obterNTermos[0].endereco3 = 3;

        obterNTermos[1].opcode = -1;
        maquina(RAM, obterNTermos);
        //resultado do numero de termos absoluto será salvo na posição 3
        // isso vai obeter o numero de vezes que vai ser realizada a  sub, obtendo o númeor de incremento "Real"

    }
    extraiRAM(RAM, 3, &nTermos);
    salvaDoisValores(RAM, 0, valorInicial, 2, Razao);
    extraiRAM(RAM, 0,&valorInicial);
    salvaUmValor(RAM, 5, valorInicial);
    Instrucao soma[3];
    printf("\n");
    for(int i=0;i<nTermos-1;i++)
    {
        
        soma[0].opcode = 0;
        soma[0].endereco1 = 0;
        soma[0].endereco2 = 2;
        soma[0].endereco3 = 0;
        
        soma[1].opcode = 0;
        soma[1].endereco1 = 0;
        soma[1].endereco2 = 5;
        soma[1].endereco3 = 5; // o somatório está salvo no indice 5 da ram
        
        soma[2].opcode = -1;
        maquina(RAM, soma);

    }
    Instrucao moverR[3];
    moverR[0].opcode = 3;
    moverR[0].endereco1 = 1;
    moverR[0].endereco2 = 5;

    moverR[1].opcode = 2;
    moverR[1].endereco1 =1;
    moverR[1].endereco2 =0;

    moverR[2].opcode = -1;
    maquina(RAM, moverR);
    int resultado;
    extraiRAM(RAM, 0, &resultado);
    printf("\nResultado do somatório: %d\n\n", resultado);
    if(criaRam )
    {
        liberaRAM(RAM);
        RAM = NULL;
    }
}



void programaMDC(int *RAM, int a, int b) {
    int local = 0;
    if (RAM == NULL) {
        RAM = criaRam_vazia(4);
        local = 1;
    }

    // RAM[0] = a
    // RAM[1] = b
    {
        Instrucao init[5];

        init[0].opcode = 4; init[0].endereco1 = 1; init[0].endereco2 = a;
        init[1].opcode = 2; init[1].endereco1 = 1; init[1].endereco2 = 0;

        init[2].opcode = 4; init[2].endereco1 = 1; init[2].endereco2 = b;
        init[3].opcode = 2; init[3].endereco1 = 1; init[3].endereco2 = 1;

        init[4].opcode = -1;
        maquina(RAM, init);
    }

    // LOOP EUCLIDES (por subtrações)
    while (1) {
        int A = RAM[0];
        int B = RAM[1];

        if (A == B) break;

        Instrucao prog[2];

        if (A > B) {
            // RAM[0] = RAM[0] - RAM[1]
            prog[0].opcode = 1;
            prog[0].endereco1 = 0;
            prog[0].endereco2 = 1;
            prog[0].endereco3 = 0;
        } else {
            // RAM[1] = RAM[1] - RAM[0]
            prog[0].opcode = 1;
            prog[0].endereco1 = 1;
            prog[0].endereco2 = 0;
            prog[0].endereco3 = 1;
        }

        prog[1].opcode = -1;

        maquina(RAM, prog);
    }

    // imprimir
    Instrucao out[3];
    out[0].opcode = 3; out[0].endereco1 = 1; out[0].endereco2 = 0; // RAM[0] → reg1
    out[1].opcode = 5; out[1].endereco1 = 1; out[1].endereco2 = -1; // print
    out[2].opcode = -1;

    maquina(RAM, out);

    printf("MDC = %d\n", out[1].endereco2);

    if (local) liberaRAM(RAM);
}

void programaMultiplicaInterno(int *RAM, int x, int y, int destino) {
    // resultado ficará em RAM[destino]
    // RAM[2] = acumulador
    // RAM[3] = contador

    int local = 0;
    if (RAM == NULL) {
        RAM = criaRam_vazia(8);
        local = 1;
    }

    // zera acumulador
    {
        Instrucao init[6];

        // RAM[2] = 0
        init[0].opcode = 4; init[0].endereco1 = 1; init[0].endereco2 = 0;
        init[1].opcode = 2; init[1].endereco1 = 1; init[1].endereco2 = 2;

        // RAM[3] = y (contador)
        init[2].opcode = 4; init[2].endereco1 = 1; init[2].endereco2 = y;
        init[3].opcode = 2; init[3].endereco1 = 1; init[3].endereco2 = 3;

        // RAM[4] = x
        init[4].opcode = 4; init[4].endereco1 = 1; init[4].endereco2 = x;
        init[5].opcode = 2; init[5].endereco1 = 1; init[5].endereco2 = 4;

        maquina(RAM, init);
    }

    // loop: acum = acum + x  (feito via subtração negativa)
    while (RAM[3] > 0) {

        // acum = acum - ( -x )
        Instrucao somar[3];

        somar[0].opcode = 1; // subtrai
        somar[0].endereco1 = 2; // RAM[2] = ...
        somar[0].endereco2 = 4; // RAM[2] - RAM[4]
        somar[0].endereco3 = 2;

        somar[1].opcode = -1;

        maquina(RAM, somar);

        // decrementa contador
        Instrucao dec[3];
        dec[0].opcode = 1;
        dec[0].endereco1 = 3;
        dec[0].endereco2 = 4; // subtrai x? não — precisamos RAM[5] = 1
        dec[0].endereco3 = 3;
        dec[1].opcode = -1;

        maquina(RAM, dec);

        RAM[3]--; // para controle do while
    }

    // copiar acumulador para destino
    Instrucao fim[4];
    fim[0].opcode = 3; fim[0].endereco1 = 1; fim[0].endereco2 = 2; // RAM[2] → reg1
    fim[1].opcode = 2; fim[1].endereco1 = 1; fim[1].endereco2 = destino; // reg1 → RAM[destino]
    fim[2].opcode = -1;
    maquina(RAM, fim);

    if (local) liberaRAM(RAM);
}

void programaMMC(int *RAM, int a, int b) {
    int local = 0;
    if (RAM == NULL) {
        RAM = criaRam_vazia(8);
        local = 1;
    }

    // 1) MDC usando a máquina
    programaMDC(RAM, a, b);

    int mdc = RAM[0];

    // 2) multiplicação usando máquina
    programaMultiplicaInterno(RAM, a, b, 5); // RAM[5] = a*b

    int produto = RAM[5];

    // 3) divisão produto/mdc só com subtrações
    RAM[6] = 0;     // quociente
    RAM[7] = produto;

    while (RAM[7] > 0) {
        // RAM[7] = RAM[7] - mdc
        Instrucao sub[3];
        sub[0].opcode = 4; sub[0].endereco1 = 1; sub[0].endereco2 = mdc;
        sub[1].opcode = 1; sub[1].endereco1 = 7; sub[1].endereco2 = 1; sub[1].endereco3 = 7;
        sub[2].opcode = -1;
        maquina(RAM, sub);

        RAM[7] -= mdc;
        RAM[6]++;  // quociente++
    }

    printf("MMC = %d\n", RAM[6]);

    if (local) liberaRAM(RAM);
}
