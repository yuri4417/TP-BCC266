#ifndef PROGRAMAS_H
#define PROGRAMAS_H


void programaAleatorio(int qtdeInstrucoes);
void programaMultiplica(int * RAM, int multiplicando, int multiplicador);
void programaDivisao(int *RAM, int dividendo, int divisor);


void programaSQRT(int* RAM, int radicando);
void programaPA(int A1, int razao, int qtdTermos);
void programaPG(int A1, int razao, int qtdTermos);
void programaTrianguloRet(int catA, int catB);
void programaConverteBinario(int numeroDec);
void programaArranjoSimples(int n, int k);
void programaCombSimples(int n, int k);
void programaFormulaHeron(int a, int b, int c);
void programaConverteSegundos(int segundos);
void programaMultiplicaMatrizes(int *matA, int *matB, int N);

void programaPotencia(int *RAM, int base, int expoente);
void programaFatorial(int *RAM, int n);
void programaFibonacci(int *RAM, int n);
void programaSomatorio(int *RAM, int indiceInicial, int nTermos,int valorInicial, int Razao);
void programaValorAbsoluto(int *RAM, int valor);
void programaCriptografarCifraCesar( int *RAM, int rotacao, char *palavra );
void programaMinusculapMaiuscula(int *RAM, char *palavra);


void programaMDC(int *RAM, int a, int b);
void programaMultiplicaInterno(int *RAM, int x, int y, int destino);
void programaMMC(int *RAM, int a, int b);
void programaPrimo(int *RAM, int n);

#endif