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
    double ;
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
}