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

int main(int argc, char *argv[]){
    double *A,*B,*C,*R;
    int *D;
    double time,time_Bloque,tick;
    int i,j,k,l,N;
    double maxComun = -INFINITY;
    double maxA = -INFINITY;
    double block_maxA = -INFINITY;
    double minA = +INFINITY;
    double block_minA = +INFINITY;
    int tam_bloque = 256;

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

    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    R=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices A
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j] =i+j;
        }
    }

    //obtiene el valor maximo de toda la matriz
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            if(A[i*N+j] > maxComun){
                maxComun= A[i*N+j];
            }
        }
    }
    time = dwalltime() - tick;
    printf("Tiempo requerido para calcular maximo comun: %f\n",time);
    
    //obtiene el valor maximo de toda la matriz dividida por bloques
    tick = dwalltime();
    for(i=0; i < N ; i += tam_bloque){
        for(j=0; j < N ; j += tam_bloque){
            block_maxA = A[i*N+j];
            for(k = i; k < i + tam_bloque; k++){
                for(l = j; l < j + tam_bloque; l++){
                    if(A[k*N+l] > block_maxA){
                        block_maxA = A[k*N+l];
                    }
                }
            }
            if(block_maxA > maxA){
                maxA = block_maxA;
            }
        }
    }
    time_Bloque = dwalltime() - tick;
    printf("Tiempo requerido para calcular maximo por bloques: %f con tamanio bloque = %d\n",time_Bloque,tam_bloque);
    maxComun++;
    maxA++;
    return 0;
};