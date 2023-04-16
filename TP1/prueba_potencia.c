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
    int *A,*B,*C;
    double *A2,*B2,*C2;
    double timeMult,timePow,timeVector,tick;
    int i,N,j;

    N = atoi(argv[1]);
    //Verificar parametro 
    if (!(argc == 2 && (N == 512 || N == 1024 || N == 2048 || N == 4096))){
        printf("El N debe ser: 512, 1024, 2048, 4096\n");
        exit(1);
    }


    //Aloca memoria para la matriz
    A=(int*)malloc(sizeof(int)*N*N);
    A2=(double*)malloc(sizeof(double)*N*N);

    B=(int*)malloc(sizeof(int)*N*N);
    B2=(double*)malloc(sizeof(double)*N*N);

    C=(int*)malloc(sizeof(int)*N*N);
    C2=(double*)malloc(sizeof(double)*N*N);
    
    //Inicializar matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j] = rand()%40+1;
            B[i*N+j] = rand()%40+1;
            C[i*N+j] = rand()%40+1;
        }
    }

    //calcular la potencia multiplicando
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double valor=B[i*N+j];
            B2[i*N+j]=valor*valor;
        }
    }
    timeMult = dwalltime() - tick;
    printf("Tiempo requerido para elevar multiplicando es: %f \n",timeMult);


    //obtiene la potencia 2 de la matriz usando pow
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A2[i*N+j] = pow(A[i*N+j],2);
        }
    }
    timePow = dwalltime() - tick;
    printf("Tiempo requerido para elevar a la potencia a la matriz con pow: %f \n",timePow);

    //obtiene la potencia 2 con un vector con los valores precargados
    tick = dwalltime();
    int resultados[40];
    for(i=0;i<40;i++){
        resultados[i+1]= (i+1)*(i+1);
    }

    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            int valor = C[i*N+j];
            double v = resultados[valor];
            C2[i*N+j] = v;
        }
    }
    timeVector = dwalltime() - tick;
    printf("Tiempo requerido para elevar a la potencia a la matriz con vector: %f \n",timeVector);

    free(A);
    free(B);
    free(C);
    free(A2);
    free(B2);
    free(C2);
    return 0;
};