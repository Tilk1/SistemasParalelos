#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

int main(int argc, char *argv[]){
    double *A,*B,*C,*R;
    int *D;
    double timeBloques,tick,maxA,minA,maxB,minB;
    int i,j,k,N,tam_bloque=8;

    //Verificar parametro 
    if ((argc != 2)){
        printf("El N debe ser: 512,1024,2048,4096\n");
        exit(1);
    }
    N = atoi(argv[1]);
    if((N < 511) || (N > 4097) || (((N % 2)!=0))){
        printf("El N debe ser: 512,1024,2048,4096\n");
        exit(1);
    }

    A=(double*) malloc(N*N*sizeof(double));
    B=(double*) malloc(N*N*sizeof(double));
    C=(double*) malloc(N*N*sizeof(double));
    R=(double*) malloc(N*N*sizeof(double));
    D=(int*) malloc(N*N*sizeof(int));

    //Inicializar matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]=rand() % 999999999 + 1; //Ordenada por FIlas
            B[j*N+i]=1; //Ordenada por Columnas
            C[i*N+j]=1; //Ordenada Por Filas
            D[j*N+i]=rand() % 40 + 1; //Ordenada por Columnas
        }
    }
    
    //EMPIEZA A CONTAR EL TIEMPO
    tick = dwalltime();

    //calcular Maximo de A
    for(i=0;i<N;i+= tam_bloque){
        for(j=0;j<N;j+= tam_bloque){
            int block_max = A[i*N+j];
            int k, l;
            for(k = i; k < i + tam_bloque; k++){
                for(l = j; l < j + tam_bloque; l++){
                    if(A[k*N+l] > block_max){
                        block_max = A[k*N+l];
                    }
                }
            }
            if(A[i*N+j] > maxA){
                maxA = A[i*N+j];
            }
        }
    }

    timeBloques = dwalltime() - tick;
    printf("Tiempo requerido solucion por bloques: %f y el resultado es %f\n",timeBloques,maxA);
}