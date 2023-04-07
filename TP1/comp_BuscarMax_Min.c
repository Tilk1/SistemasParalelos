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
    double *A,*B;
    double time_unFor,time_dosFor,time_Bloque,tick;
    int i,j,k,l,N;
    double maxA=-INFINITY,maxB=-INFINITY,block_maxA,block_maxB;
    double minA=+INFINITY, minB=+INFINITY;
    int tam_bloque = 128;

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

    //Inicializa las matrices A
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]=i+j;
            B[i*N+j]=i*j;
        }
    }
    
    //buscar max y min en un par de for
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            if(A[i*N+j] > maxA){
                maxA=A[i*N+j];
            }
            if(A[i*N+j] < minA){
                minA=A[i*N+j];
            }
            if(B[i*N+j] > maxB){
                maxB=B[i*N+j];
            }
            if(B[i*N+j] < minB){
                minB=B[i*N+j];
            }
        }
    }
    time_unFor = dwalltime() - tick;
    printf("Tiempo requerido para calcular maximo y minimo con un par de fors: %f\n",time_unFor);
    printf("Max de A:%f, max de B:%f\n",maxA,maxB);
    printf("Min de A:%f, min de B:%f\n",minA,minB);
    printf("--------------------\n");

    //buscar max y min en dos fors
    maxA=-INFINITY;
    maxB=-INFINITY;
    minA=+INFINITY; 
    minB=+INFINITY
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            if(A[i*N+j] > maxA){
                maxA=A[i*N+j];
            }
            if(A[i*N+j] < minA){
                minA=A[i*N+j];
            }
        }
    }
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            if(B[i*N+j] > maxB){
                maxB=B[i*N+j];
            }
            if(B[i*N+j] < minB){
                minB=B[i*N+j];
            }
        }
    }

    time_dosFor = dwalltime() - tick;
    printf("Tiempo requerido para calcular maximo con dos par de fors: %f\n",time_dosFor);
    printf("Max de A:%f, max de B:%f\n",maxA,maxB);
    printf("Min de A:%f, min de B:%f\n",minA,minB);
    printf("--------------------\n");


/*     //buscar max por bloques
    maxA=-INFINITY;
    maxB=-INFINITY;
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
    for(i=0; i < N ; i += tam_bloque){
        for(j=0; j < N ; j += tam_bloque){
            block_maxB = B[i*N+j];
            for(k = i; k < i + tam_bloque; k++){
                for(l = j; l < j + tam_bloque; l++){
                    if(B[k*N+l] > block_maxB){
                        block_maxB = B[k*N+l];
                    }
                }
            }
            if(block_maxB > maxB){
                maxB = block_maxB;
            }
        }
    }
    time_Bloque = dwalltime() - tick;
    printf("Tiempo requerido para calcular maximo por bloque: %f\n",time_Bloque); 
    printf("Max de A:%f, max de B:%f\n",maxA,maxB);*/
}