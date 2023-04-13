#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void imprimir_fecha_hora_actual() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char fecha_hora[100];
    strftime(fecha_hora, sizeof(fecha_hora), "%A %d %B %Y %H:%M:%S", tm);
    printf("Fecha y hora actual: %s\n", fecha_hora);
}

//Para calcular tiempo
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

/* void multBloque(double *bloqueA, double *bloqueB, double *bloqueC, int N,int tam_bloque){
    int i, j, k;
    double valorC=0;
    for(i=0; i < tam_bloque; i++){
        for(j = 0; j < tam_bloque; j++){
            valorC = 0.0; // reiniciar el valor de valorC
            for(k = 0; k < tam_bloque; k++){
                valorC += bloqueA[i*N+ k] * bloqueB[j*N+tam_bloque];
            }
            bloqueC[i*N+j]+=valorC; //aca += en vez de =
        }
    }
} */

int main(int argc, char *argv[]){
    double *A1,*A2,*B1,*B2,*C1,*C2,*A3,*B3,*C3;
    double time_ordenado,time_desordenado,time,tick;
    int i,j,k,l,N;
    int tam_bloque=128;

    N = atoi(argv[1]);
    //Verificar parametro 
    if (!(argc == 2 && (N == 512 || N == 1024 || N == 2048 || N == 4096))){
        printf("El N debe ser: 512, 1024, 2048, 4096\n");
        exit(1);
    }

    //Aloca memoria para las matrices
    A1=(double*)malloc(sizeof(double)*N*N);
    A2=(double*)malloc(sizeof(double)*N*N);
    A3=(double*)malloc(sizeof(double)*N*N);
    B1=(double*)malloc(sizeof(double)*N*N);
    B2=(double*)malloc(sizeof(double)*N*N);
    B3=(double*)malloc(sizeof(double)*N*N);
    C1=(double*)malloc(sizeof(double)*N*N);
    C2=(double*)malloc(sizeof(double)*N*N);
    C3=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices reservando posicion en memoria
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double num = rand() % 3;
            A1[i*N+j]= num; // por filas
            A2[i*N+j]= num; // por filas 
            A3[i*N+j]= num; // por filas 
            B1[j*N+i]= num; // por columnas
            B2[i*N+j]= num; // por filas
            B3[j*N+i]= num; // por columnas
        }
    }
    //----------------------------------------------------------------------------------------------
    //multiplicacion de matrices sin bloques, ORDENADA bien
   tick = dwalltime();
     for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double valorC = 0; 
            for(k=0;k<N;k++){
                valorC += A1[i*N+k]*B1[j*N+k]; 
            }
            C1[i*N+j] = valorC;
        }
    }
    time = dwalltime() - tick;
    printf("Tiempo requerido para calcular la multiplicacion sin bloques respetando orden: %f\n",time);
    printf("imprimo primer y ultimo elemento arreglo : [%0.0f] [%0.0f] \n",C1[0],C1[N*N-1]);
    //----------------------------------------------------------------------------------------------
    //multiplicacion de matrices sin bloques, DESORDENADA mal
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double valorD = 0; 
            for(k=0;k<N;k++){
                valorD += A2[i*N+k]*B2[k*N+j]; //enzo
            }
            C2[i*N+j] = valorD;
        }
    }
    time = dwalltime() - tick;
    printf("Tiempo requerido para calcular la multiplicacion desordenada MAL: %f\n",time);
    printf("imprimo primer y ultimo elemento arreglo : [%0.0f] [%0.0f] \n",C2[0],C2[N*N-1]); 
    //----------------------------------------------------------------------------------------------
    //multiplicacion Por bloques con funcion
    //NO FUNCIONA!
/*     for (i = 0; i < N; i += tam_bloque){
        for (j = 0; j < N; j += tam_bloque){
            for(k = 0; k < N; k += tam_bloque){
                multBloque(&A[i*N + k], &B[j*N + k], &C[i*N + j],N,tam_bloque);
            }
        }
    } */

    //----------------------------------------------------------------------------------------------
    //multiplicacion matrices por bloques,  furiosa 6 fors
    tick = dwalltime();
    int ii, jj, kk;
    for (i = 0; i < N; i += tam_bloque) {
        for (j = 0; j < N; j += tam_bloque) {
            for  (k = 0; k < N; k += tam_bloque) {
                for (ii = i; ii < i + tam_bloque; ii++) {
                    int valorii=ii*N;
                    for (jj = j; jj < j + tam_bloque; jj++){
                        int valorjj=jj*N;
                        double temp = 0.0;
                        for (kk = k; kk < k + tam_bloque; kk++) {
                            temp += A3[valorii+kk] * B3[valorjj+kk];
                        }
                        C3[valorii+jj] += temp;
                    }
                }
            }
        }
    } 
    time = dwalltime() - tick;
    printf("Tiempo requerido para calcular la multiplicacion por bloques: %f\n",time);
    printf("imprimo primer y ultimo elemento arreglo : [%0.0f] [%0.0f] \n",C3[0],C3[N*N-1]);
    //----------------------------------------------------------------------------------------------

    printf("matriz: %dx%d\n",N,N);
    printf("tam_bloque: %d\n",tam_bloque);
    printf("N = %d\n",N);
    printf("imprimo C1 (primeros 10 elementos)\n");
     for(i=0;i<10;i++){
        for(j=0;j<10;j++){
            printf(" [%i][%i]= %0.0f ",i,j,C1[i*N+j]);
        }
        printf("\n");
    } 
    printf("imprimo C2 (primeros 10 elementos) \n");
    for(i=0;i<10;i++){
        for(j=0;j<10;j++){
            printf(" [%i][%i]= %0.0f ",i,j,C2[i*N+j]);
        }
        printf("\n");
    } 
    printf("imprimo C3 (primeros 10 elementos) \n");
    for(i=0;i<10;i++){
        for(j=0;j<10;j++){
            printf(" [%i][%i]= %0.0f ",i,j,C3[i*N+j]);
        }
        printf("\n");
    } 

    imprimir_fecha_hora_actual();
    free(A1);
    free(A2);
    free(A3);
    free(B1);
    free(B2);
    free(B3);
    free(C1);
    free(C2);
    free(C3);
    return 0;
}


  //multiplicacion de matrices , matriz A ordenada por filas, B por columna
    //fila y columnas mal sin bloqe
    //fila y columna bien sin bloque
    //fila y columna bien con bloque tam n 32
    //fila y columna bien con bloque tam n 64
    //fila y columna bien con bloque tam n 128
    //fila y columna bien con bloque tam n 256
    //fila y columna bien con bloque tam optimo CON o1
    //fila y columna bien con bloque tam optimo CON o2
    //fila y columna bien con bloque tam optimo CON o3