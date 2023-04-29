#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

//Para calcular tiempo
double dwalltime();
void imprimir_fecha_hora_actual();
void mult_matrices(double *a, double *b, double *c, int n, int bs); //multiplicar matriz
void mult_bloques(double *ablk, double *bblk, double *cblk, int n, int bs); //multiplicar bloque

int main(int argc, char *argv[]){
    double *A,*B,*C,*R; 
    double *D2,*CD,*AB;
    int *D;
    double timeTotal,tick,maxA,minA,maxB,minB;
    int i,j,k,l,N,tam_bloque;
    int ii, jj, kk, resultados[40];

    // para los promedios
    double sumaA = 0; 
    double sumaB = 0;
    double promedioA = 0; 
    double promedioB = 0;
    double cantElementos;
    

    //Verificar parametro 
    if ((argc != 3)){
        printf("Param1:512|1024|2048|4096 - Param2:tambloque \n");
        exit(1);
    }
    N = atoi(argv[1]);
    tam_bloque = atoi(argv[2]);
    cantElementos = N*N;


    A=(double*) malloc(N*N*sizeof(double));
    B=(double*) malloc(N*N*sizeof(double));
    C=(double*) malloc(N*N*sizeof(double));
    R=(double*) malloc(N*N*sizeof(double));
    D=(int*) malloc(N*N*sizeof(int));
    CD=(double*) malloc(N*N*sizeof(double));
    AB=(double*) malloc(N*N*sizeof(double));
    D2=(double*) malloc(N*N*sizeof(double));

    //Inicializar matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]=1; //Ordenada por Filas
            B[j*N+i]=1; //Ordenada por Columnas
            C[i*N+j]=1; //Ordenada Por Filas
            D[j*N+i]=rand()%40+1; //Ordenada por Columnas 1..40
        }
    }

    
    /*  PASOS

    1)  MinA = mínimo de A
        MaxA = máximo de A
        PromA = promedio de A
        MinB = mínimo de B
        MaxB = máximo de B
        PromB = promedio de B 
    2)  A x B = AB
    3)  D^2 = D2
    4)  C x D2 = CD
    5)  (MaxA x MaxB - MinA x MinB) / (PromA x PromB) = RP 
    6)  RP x AB = AB
    7)  AB + CD2 = R

    */

    //------------EMPIEZA A CONTAR EL TIEMPO-----------------------
    tick = dwalltime();

    //1) calcula max de A y B , min de A y B y promedio de A y B
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
    promedioA=sumaA/cantElementos;
    promedioB=sumaB/cantElementos;

    //2) A x B = AB(ordenado x filas)
    mult_matrices(A,B,AB,N,tam_bloque);


    //precargo un array con las potencias de 1..40
    for(i=0;i<40;i++){
        resultados[i+1]= (i+1)*(i+1);
    }

    //3) D2 = D^2 (ordenado x columnas), D2 es double, deja de ser int
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            int valor = D[i*N+j];
            double v = resultados[valor];
            D2[i*N+j] = v;
        }
    }

    //4) C x D2 = CD(ordenado x filas)
    mult_matrices(C,D2,CD,N,tam_bloque);
    
    //5) calculo de la primera parte de la ecuacion
    double RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB)); //RP es un solo numero

    //6) AB = AB x RP 
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            AB[j*N+i] = AB[j*N+i]*RP;
        }
    }

    //AB + CD = R
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            R[i*N+j] = AB[i*N+j] + CD[j*N+i];
        }
    }

    timeTotal = dwalltime() - tick;

    //printf("resultados parciales maxA:%0.0f minA:%0.0f promA:%0.0f maxB:%0.0f minB:%0.1f promB:%0.1f \n",maxA,minA,promedioA,maxB,minB,promedioB);
    printf("matriz: %dx%d\n",N,N);
    printf("tam_bloque: %d\n",tam_bloque);
    printf("Tiempo requerido total de la ecuacion: %f\n",timeTotal);
    imprimir_fecha_hora_actual();

    free(A);
    free(B);
    free(C);
    free(R);
    free(D);
    free(AB);
    free(CD);
    free(D2);
    return 0;
}

void mult_matrices(double *A, double *B, double *C, int N, int bs){
    int i, j, k;
    for(i = 0; i < N; i += bs){
        for(j = 0; j < N; j += bs){
            for(k = 0; k < N; k += bs){
                mult_bloques(&A[i*N+k], &B[j*N+k], &C[i*N+j],N,bs);
            }
        }
    }
}

void mult_bloques(double *ablk, double *bblk, double *cblk, int N, int bs){
    int i, j, k; 
    for(i = 0; i < bs; i++){
        for(j = 0; j < bs; j++){
            int temp=0;
            for(k = 0; k < bs; k++){
                temp += ablk[i*N+k] * bblk[j*N+k];
            }
            cblk[i*N+j]+=temp;
        }
    }
}

void imprimir_fecha_hora_actual() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char fecha_hora[100];
    strftime(fecha_hora, sizeof(fecha_hora), "%A %d %B %Y %H:%M:%S", tm);
    printf("Fecha y hora actual: %s\n", fecha_hora);
}

double dwalltime(){
        double sec;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}