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

void imprimir_fecha_hora_actual() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char fecha_hora[100];
    strftime(fecha_hora, sizeof(fecha_hora), "%A %d %B %Y %H:%M:%S", tm);
    printf("Fecha y hora actual: %s\n", fecha_hora);
}

int main(int argc, char *argv[]){
    double *A,*B,*C;//*R;
    int *D;
    double timeTotal,tick,maxB,minB;
    int i,j,k,l,N,tam_bloque=4;
    double maxA = -INFINITY;
    double block_maxA = -INFINITY;
    double minA = +INFINITY;
    double block_minA = +INFINITY;

    // para los promedios
    double sumaA = 0; 
    double sumaB = 0;
    double promedioA = 0; 
    double promedioB = 0;
    double cantElementos;
    

    //Verificar parametro 
/*     if ((argc != 2)){
        printf("El N debe ser: 512,1024,2048,4096\n");
        exit(1);
    } */
    N = atoi(argv[1]);
    cantElementos = N*N;
/*     if((N < 1) || (N > 4097) || (((N % 2)!=0))){
        printf("El N debe ser: 512,1024,2048,4096\n");
        exit(1);
    } */

    A=(double*) malloc(N*N*sizeof(double));
    B=(double*) malloc(N*N*sizeof(double));
    C=(double*) malloc(N*N*sizeof(double));
    //R=(double*) malloc(N*N*sizeof(double));
    D=(int*) malloc(N*N*sizeof(int));


    //Inicializar matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]=i+j; //Ordenada por Filas
            B[j*N+i]=i+j; //Ordenada por Columnas
            C[i*N+j]=1; //Ordenada Por Filas
            D[j*N+i]=rand() % 40 + 1; //Ordenada por Columnas
        }
    }

    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            //printf("[%i][%i]= %0.0f\n",i,j,A[i*N+j]);
        }
    }
    
    //EMPIEZA A CONTAR EL TIEMPO
    tick = dwalltime();

    //Calcular maximo y minimo de A y B todo junto
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double posA = A[i*N+j];
            if(posA> maxA){
                maxA = posA;
            }    
            if(posA < minA){
                minA = posA;
            }
            double posB = B[j*N+i];
            if(posB> maxB){
                maxB = posB;
            }
            if(posB < minB){
                minB = posB;
            }
        }
    }

    //Calcular maximo y minimo de A y B separado
/*     for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double posA = A[i*N+j];
            if(posA> maxA){
                maxA = posA;
            }    
            if(posA < minA){
                minA = posA;
            }
        }
    }
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double posB = B[j*N+i];
            if(posB> maxB){
                maxB = posB;
            }
            if(posB < minB){
                minB = posB;
            }
        }
    } */


    //calculo de promedios de A y B
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            sumaA += A[i*N+j];
            sumaB += B[j*N+i];
        }
    }
    promedioA = sumaA/cantElementos;
    promedioB = sumaB/cantElementos;




    //calcular las potencias de 2 de D
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double valor = D[j*N+i];
            D[j*N+i] = valor*valor;
            //D[j*N+i] = pow(D[j*N+i],2);
        }
    }


    double R;
    R = (maxA * maxB - minA * minB) / (promedioA * promedioB);
    timeTotal = dwalltime() - tick;
    printf("resultados parciales maxA:%0.0f minA:%0.0f promA:%0.0f maxB:%0.0f minB:%0.1f promB:%0.1f \n",maxA,minA,promedioA,maxB,minB,promedioB);
    printf("R = %0.2f\n",R);
    //fin del programa
    printf("Tiempo requerido total de la ecuacion: %f\n",timeTotal);
    imprimir_fecha_hora_actual();
    printf("-----------------------------------------\n");
    free(A);
    free(B);
    free(C);
    //free(R);
    free(D);
    return 0;
}
