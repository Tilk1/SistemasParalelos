#include <stdio.h>
#include <stdlib.h>
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
    double *A,*B,*C,*R; double *D2,*CD,*AB;
    int *D;
    double timeTotal,tick,maxA,minA,maxB,minB;
    int i,j,k,l,N,tam_bloque;
    int ii, jj, kk;

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
            A[i*N+j]=1.0; //Ordenada por Filas
            B[j*N+i]=1.0; //Ordenada por Columnas
            C[i*N+j]=1.0; //Ordenada Por Filas
            D[j*N+i]=1;//rand() % 40 + 1; //Ordenada por Columnas 1..40
            R[i*N+j]=0; 
            CD[j*N+i]=0; //Si yo pongo 1 da mal
            AB[i*N+j]=0; //si yo pongo 1 da mal
            D2[j*N+i]=0;//Ordenada por Columnas 1..40
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
    4)  C x D = CD
    5)  ((MaxA x MaxB - MinA x MinB) / PromA x PromB) = RP 
    6)  RP x AB = AB
    7)  AB + CD2 = R

    */

    //------------EMPIEZA A CONTAR EL TIEMPO-----------------------
    tick = dwalltime();

    //calcula max de A y B , min de A y B y promedio de A y B
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
            if(A[pos] > maxA)     //buscar el max de A
                maxA=A[pos];
            else 
                if(A[pos] < minA) //buscar el min de A
                    minA=A[pos];
            if(B[pos] > maxB)     //buscar el max de B
                maxB=B[pos];
            else 
                if(B[pos] < minB) //buscar el min de B
                    minB=B[pos];
            sumaA += A[pos];
            sumaB += B[pos];
        }
    }
    promedioA=sumaA/cantElementos;
    promedioB=sumaB/cantElementos;

    // A x B = AB(ordenado x filas)
    for (i = 0; i < N; i += tam_bloque) {
        for (j = 0; j < N; j += tam_bloque) {
            for  (k = 0; k < N; k += tam_bloque) {
                for (ii = i; ii < i + tam_bloque; ii++) {
                    int valorii=ii*N;
                    for (jj = j; jj < j + tam_bloque; jj++){
                        int valorjj=jj*N;
                        double temp = 0.0;
                        for (kk = k; kk < k + tam_bloque; kk++) {
                            temp += A[valorii+kk] * B[valorjj+kk];
                        }
                        AB[valorii+jj] += temp;
                    }
                }
            }
        }
    } 

    //D^2= D2 (ordenado x columnas), D2 es double, deja de ser int
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double valor = D[j*N+i];
            D2[j*N+i] = valor*valor;
        }
    }
 
    // C x D2 = CD(ordenado x filas)
    for (i = 0; i < N; i += tam_bloque) {
        for (j = 0; j < N; j += tam_bloque) {
            for  (k = 0; k < N; k += tam_bloque) {
                for (ii = i; ii < i + tam_bloque; ii++) {
                    int valorii=ii*N;
                    for (jj = j; jj < j + tam_bloque; jj++){
                        int valorjj=jj*N;
                        double temp = 0.0;
                        for (kk = k; kk < k + tam_bloque; kk++) {
                            temp += C[valorii+kk] * D2[valorjj+kk];
                        }
                        CD[valorii+jj] += temp;
                    }
                }
            }
        }
    }


/*     for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double valorC = 0; 
            for(k=0;k<N;k++){
                valorC += C[i*N+k]*D2[j*N+k]; 
            }
            CD[i*N+j] = valorC;
        }
    } */
    
    double RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB)); //RP es un solo numero

    //RP x AB = AB
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            AB[j*N+i] = AB[j*N+i]*RP;
        }
    }

    //AB + CD = R
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            R[i*N+j] = AB[i*N + j] + CD[j*N + i];
        }
    }

    timeTotal = dwalltime() - tick;
    //------------FIN DE  CONTAR EL TIEMPO-----------------------
/*  printf("imprimo R");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("[%i][%i]= %0.0f\n",i,j,R[i*N+j]);
        }
    } */

    printf("RP: %.2f\n",RP);
    printf("resultados parciales maxA:%0.0f minA:%0.0f promA:%0.0f maxB:%0.0f minB:%0.1f promB:%0.1f \n",maxA,minA,promedioA,maxB,minB,promedioB);
    printf("N = %d\n",N);
    printf("imprimo primer y ultimo elemento del arreglo R, deben ser N c/u : [%0.0f] [%0.0f] \n",R[0],R[N*N-1]);
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