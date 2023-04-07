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
    double maxA,maxB,minA,minB,sumaA,sumaB,promedioA,promedioB,block_maxA,block_maxB;
    int tam_bloque = 128;

    N = atoi(argv[1]);
    //Verificar parametro 
    if (!(argc == 2 && (N == 512 || N == 1024 || N == 2048 || N == 4096))){
        printf("El N debe ser: 512, 1024, 2048, 4096\n");
        exit(1);
    }

    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]=i+j;
            B[i*N+j]=i*j;
        }
    }
    
    //buscar max y min en un par de for
    maxA=A[0];
    minA=A[0];
    maxB=B[0];
    minB=B[0];
    sumaA=0;
    sumaB=0;
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            int pos=i*N+j;
            if(A[pos] > maxA) //buscar el max de A
                maxA=A[pos];
            else 
                if(A[pos] < minA) //buscar el min de A
                    minA=A[pos];
            if(B[pos] > maxB) //buscar el max de B
                maxB=B[pos];
            else 
                if(B[pos] < minB) //buscar el min de B
                    minB=B[pos];
            sumaA += A[pos];
            sumaB += B[pos];
        }
    }
    promedioA=sumaA/N;
    promedioB=sumaB/N;
    time_unFor = dwalltime() - tick;
    printf("Tiempo requerido para calcular maximo, minimo y prom en el mismo for: %f\n",time_unFor);
    printf("Max de A:%f, max de B:%f\n",maxA,maxB);
    printf("Min de A:%f, min de B:%f\n",minA,minB);
    printf("Promedio de A:%f, promedio de B:%f\n",promedioA,promedioB);
    printf("--------------------\n");

    //buscar max y min en dos fors
    maxA=A[0];
    minA=A[0];
    maxB=B[0];
    minB=B[0];
    sumaA=0;
    sumaB=0;
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            int pos=i*N+j;
            if(A[pos] > maxA) maxA=A[pos];
            else if(A[pos] < minA) minA=A[pos];
            sumaA+=A[pos];
        }
    }
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            int pos=i*N+j;
            if(B[pos] > maxB) maxB=B[pos];
            else if(B[pos] < minB) minB=B[pos];
            sumaB+=B[pos];
        }
    }
    promedioA=sumaA/N;
    promedioB=sumaB/N;
    time_dosFor = dwalltime() - tick;
    printf("Tiempo requerido para calcular maximo, minimo y prom en diferentes for: %f\n",time_dosFor);
    printf("Max de A:%f, max de B:%f\n",maxA,maxB);
    printf("Min de A:%f, min de B:%f\n",minA,minB);
    printf("Promedio de A:%f, promedio de B:%f\n",promedioA,promedioB);
    printf("--------------------\n");
}