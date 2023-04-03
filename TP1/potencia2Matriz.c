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
    int *A,*B,*C,*R;
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


    //Aloca memoria para la matriz
    A=(int*)malloc(sizeof(int)*N*N);

    //Inicializa la matriz A y B de 1 a 40 x filas
    
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j] = rand() % 40 + 1; //random 1..40
            }
        }

    
    //obtiene la potencia 2 de la matriz usando bit shift
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j] = A[i*N+j] << 1;
            }
        }
    time = dwalltime() - tick;
    printf("Tiempo requerido para elevar ala potencia a la matriz con bit shift: %f \n",time);


    //obtiene la potencia 2 de la matriz usando pow
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j] = pow(A[i*N+j],2);
            }
    }
    time = dwalltime() - tick;
    printf("Tiempo requerido para elevar ala potencia a la matriz con pow: %f \n",time);

    //printf("Tiempo requerido para elevar ala potencia a la matriz: %f con tam_bloque = %d\n",time,tam_bloque);


    time_t t;
    struct tm *tm = localtime(&t);
    printf("Fecha y hora de la ejecucion: %02d/%02d/%d %02d:%02d:%02d\n",
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    return 0;
};