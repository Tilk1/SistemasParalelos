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
    double *A,*B,*C;
    double time_bloque,tick,temp;
    int i,j,k,l,N, ii, jj, kk;
    int tam_bloque=512;

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
    for (i = 0; i < N; i += tam_bloque) {
        for (j = 0; j < N; j += tam_bloque) {
            for  (k = 0; k < N; k += tam_bloque) {
                for (ii = i; ii < i + tam_bloque; ii++) {
                    int valorii=ii*N;
                    for (jj = j; jj < j + tam_bloque; jj++) {   
                        int valorjj=jj*N;
                        double temp = 0.0; //acumula
                        for (kk = k; kk < k + tam_bloque; kk++) {
                            temp += A[valorii+kk] * B[valorjj+kk];
                        }
                        C[valorii+jj] += temp;
                    }
                }
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