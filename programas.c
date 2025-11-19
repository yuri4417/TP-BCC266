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

void programaRaizQuad(int* RAM, int radicando) { //RESULTADO == RAM[0]
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

    programaRaizQuad(RAM, soma);

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

    // Esquema RAM -> [LIXO, LIXO, LIXO, LIXO, posPreencher, numeroDec, 1, LSB, ....]

    salvaDoisValores(RAM, 5, numeroDec, 6, 1);
    salvaUmValor(RAM, 4, 7); 

    //RAM -> [0, 0, 0, 0, 7, numeroDec, 1, 0, ....]

    int numero;
    extraiRAM(RAM, 5, &numero);

    while (numero > 0) {
        programaDivisao(RAM, numero, 2);
        //RAM -> [result, resto, LIXO, LIXO, posPreencher, numeroDec, 1, 0, ....]

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

void programaPotencia(int *RAM, int base, int expoente)
{
    int ramLocal = 0;
    if (RAM == NULL) {
        RAM = criaRam_vazia(4);
        ramLocal = 1;
    }
    
    salvaDoisValores(RAM, 0, base, 0 , expoente);
    Instrucao move[3];
    move[0].opcode = 2;
    move[0].endereco1 = 1;
    move[0].endereco2 = 0; //na posição 0 da ram está salva a base

    move[1].opcode = 2;
    move[1].endereco1 = 2;
    move[1].endereco2 = 1;//na posição 1 da ram está salva o expoente

    move[2].opcode = -1;
    
    maquina(RAM, move);
    int externoBase, externoExpoente;
    if (RAM[0] == 0) {
        extraiRAM(RAM,0, &externoBase);
        extraiRAM(RAM,1, &externoExpoente);
        if (RAM[1] <0) 
            printf("Erro: %d ^ %d Divisao por zero\n", externoBase, externoExpoente); 
        else if (RAM[1] == 0) 
            printf("Erro: %d ^ %d Resultado indeterminado\n", externoBase, externoExpoente); 
        else 
            printf("%d ^ %d = %d\n", externoBase, externoExpoente, 0); 
        
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }
    if (RAM[1] == 0) {
        extraiRAM(RAM,0, &externoBase);
        extraiRAM(RAM,1, &externoExpoente);
        printf("%d ^ %d = %d\n", externoBase, externoExpoente, 1);
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }
    if (RAM[0] == 1) {
        extraiRAM(RAM,0, &externoBase);
        extraiRAM(RAM,1, &externoExpoente);
        printf("%d ^ %d = %d\n", externoBase, externoExpoente, 1);
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }
    if (RAM[0] == -1) {
        extraiRAM(RAM,0, &externoBase);
        extraiRAM(RAM,1, &externoExpoente);
        if (RAM[1] % 2 == 0) 
            printf("%d ^ %d = %d\n", externoBase, externoExpoente, 1);
        else 
            printf("%d ^ %d = %d\n", externoBase, externoExpoente, -1); 
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    }
    if (RAM[1] < 0) {
        extraiRAM(RAM,0, &externoBase);
        extraiRAM(RAM,1, &externoExpoente);
        printf("%d ^ %d = %d\n", externoBase, externoExpoente, 0);
        if (ramLocal) 
            liberaRAM(RAM);
        return;
    
    }
    
    Instrucao moverPCalcular[3];//Essa struct irá pegar o valor  da base na RAM[0] mover para o reg1 e depois levá-la para a RAM[2]
    moverPCalcular[0].opcode = 3; 
    moverPCalcular[0].endereco1 = 1; 
    moverPCalcular[0].endereco2 = 0; // RAM[0] base -> reg1

    moverPCalcular[1].opcode = 2; 
    moverPCalcular[1].endereco1 = 1; 
    moverPCalcular[1].endereco2 = 2; // reg1 -> RAM[2] 

    moverPCalcular[2].opcode = -1;
    maquina(RAM, moverPCalcular);
    for(int i = 0; i < RAM[1] - 1; i++)
    {
        int rAtual = RAM[0]; //vai pegar o resultado da última multiplicação, antes de perder o valor
        salvaUmValor(RAM, 0, 0);
        programaMultiplica(RAM, rAtual, RAM[2]);
    }
    extraiRAM(RAM,2, &externoBase);
    extraiRAM(RAM,1, &externoExpoente);
    int resultado;
    extraiRAM(RAM,0, &resultado);
     
    printf("Valor da operação %d ^ %d = %d",base, expoente,  resultado);
     if(ramLocal)
        liberaRAM(RAM);
}
