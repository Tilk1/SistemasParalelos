#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//Para calcular tiempo
/* double dwalltime(){
        double sec;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
} */

double dwalltime() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

int main(int argc, char *argv[]){
    double *A,*B,*C,*R;
    int *D;
    double time,tick;
    int i,N,j;

    double max;
    int tam_bloque = 8;

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

    tick = dwalltime();

    printf("hello");

    time = dwalltime() - tick;
    printf("Tiempo requerido solucion: %f\n",time);


    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    R=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices A y B de 1 a 40 x filas
    
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j] = rand() % 40 + 1; //random 1..40
            }
        }



    //obtiene el valor maximo de toda la matriz
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            if(A[i*N+j] > max){
                max = A[i*N+j];
            }
        }
    }
    time = dwalltime() - tick;
    printf("Tiempo requerido para calcular maximo comun: %f\n",time);
    
    //obtiene el valor maximo de toda la matriz dividida por bloques
    tick = dwalltime();
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
            if(A[i*N+j] > max){
                max = A[i*N+j];
            }
        }
    }
    time = dwalltime() - tick;
    printf("Tiempo requerido para calcular maximo por bloques: %f con tam_bloque = %d\n",time,tam_bloque);


    printf("Maximo: %f\n",max);

    time_t t;
    struct tm *tm = localtime(&t);
    printf("Fecha y hora de la ejecucion: %02d/%02d/%d %02d:%02d:%02d\n",
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    return 0;
};