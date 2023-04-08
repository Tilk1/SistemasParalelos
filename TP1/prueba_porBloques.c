#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//Para calcular tiempo
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

void multBloque(double *bloqueA, double *bloqueB, double *bloqueC, int N,int tam_bloque){
    int i, j, k;
    double valorC=0;
    for(i=0; i < tam_bloque; i++){
        for(j = 0; j < tam_bloque; j++){
            for(k = 0; k < tam_bloque; k++){
                valorC += bloqueA[i*N+ k] * bloqueB[j*N+k];
            }
            bloqueC[i*N+j]=valorC;
        }
    }
}

int main(int argc, char *argv[]){
    double *A,*B,*C;
    double time_bloque,tick;
    int i,j,k,l,N;
    int tam_bloque=32;

    N = atoi(argv[1]);
    //Verificar parametro 
    if (!(argc == 2 && (N == 512 || N == 1024 || N == 2048 || N == 4096))){
        printf("El N debe ser: 512, 1024, 2048, 4096\n");
        exit(1);
    }

    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]=i+j; //ordenado por fila
            B[j*N+i]=i*j; //ordenado por columna
        }
    }

    //Por bloques
    tick = dwalltime();
    for (i = 0; i < N; i += tam_bloque){
        for (j = 0; j < N; j += tam_bloque){
            for(k = 0; k < N; k += tam_bloque){
                multBloque(&A[i*N + k], &B[j*N + k], &C[i*N + j],N,tam_bloque);
            } 
        }
    }
    time_bloque = dwalltime() - tick;
    printf("Tiempo requerido para calcular por bloque: %f\n",time_bloque);
    free(A);
    free(B);
    free(C);
    
    return 0;
}