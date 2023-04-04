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
    int *A,*B;
    double timeMult,timePow,tick;
    int i,N,j;
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
    B=(int*)malloc(sizeof(int)*N*N);
    
    //Inicializar matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j] = i;
            B[i*N+j] = i;
        }
    }

    //calcular la potencia multiplicando
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double valor=B[i*N+j];
            B[i*N+j]=valor*valor;
        }
    }
    timeMult = dwalltime() - tick;
    printf("Tiempo requerido para elevar multiplicando es: %f \n",timeMult);


    //obtiene la potencia 2 de la matriz usando pow
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j] = pow(A[i*N+j],2);
        }
    }
    timePow = dwalltime() - tick;
    printf("Tiempo requerido para elevar a la potencia a la matriz con pow: %f \n",timePow);

    return 0;
};