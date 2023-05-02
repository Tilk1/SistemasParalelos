#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h>

double dwalltime();

//variables compartidas
double maxA,minA,maxB,minB,sumaA=0,sumaB=0;


int main(int argc, char *argv[]){
    double time,tick,temp;
    int i,j,k,valori,valorj,cantElementos,resultados[40];
    double *A,*B,*C,*D2,*CD,*AB,*R;
    int *D;
    double promedioA,promedioB;

    //Verificar parametro 
    if ((argc != 3)){
        printf("Param1: N - Param2: cantidad threads \n");
        exit(1);
    }
    int N=atoi(argv[1]);
    int numThreads = atoi(argv[2]);
    omp_set_num_threads(numThreads);
    cantElementos=N*N;

    //Aloca memoria para las matrices
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
            A[i*N+j]=rand()%100+1; //Ordenada por Filas
            B[j*N+i]=rand()%50+1; //Ordenada por Columnas
            C[i*N+j]=1; //Ordenada Por Filas
            D[j*N+i]=rand()%40+1; //Ordenada por Columnas 1..40
            AB[i*N+j]=0;
        }
    }

    // para los promedios
    sumaA = 0; 
    sumaB = 0;
    maxA=A[0];
    minA=A[0];


    /*  PASOS

    1)  MinA = mínimo de A
        MaxA = máximo de A
        PromA = promedio de A
        MinB = mínimo de B
        MaxB = máximo de B
        PromB = promedio de B 
    2)  AB = A x B
    3)  D2 = D^2
    4)  CD = C x D2 
    5)  RP = (MaxA x MaxB - MinA x MinB) / (PromA x PromB)
    6)  AB = RP x AB 
    7)  R = AB + CD2 */

    //-------------------EMPIEZA A CONTAR EL TIEMPO----------------------------
    tick = dwalltime();

/*     printf("imprimo A\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("[%i][%i]= %0.0f\n",i,j,A[i*N+j]);
        }
    } */

    //1) Calcular max,min y prom de A y B
    #pragma omp parallel for default (none) shared(A) firstprivate(cantElementos) private(i) reduction(+:sumaA) reduction(min:minA) reduction(max:maxA)
    for(i=0;i<cantElementos;i++){
        sumaA+=A[i];
        if(A[i]>maxA) maxA=A[i]; 
        else if(A[i]<minA) minA=A[i];
    }
    
    printf("maxA: %f, minA: %f, sumaA: %f \n",maxA,minA,sumaA);

    //2) AB = A X B
    #pragma omp parallel for default(none) shared(A,B,AB,N) private(i,j,k,valori,valorj,temp)
    for(i=0;i<N;i++){
        valori=i*N;
        for(j=0;j<N;j++){
            temp=0;
            valorj=j*N;
            for(k=0;k<N;k++){
                temp+=A[valori+k]*B[valorj+k];
            }
            AB[valori+j]+=temp;
        }
    }

    //3)D2 = D^2

    //4)CD = C X D2

    //5) Calculo de RP -> secuencial
    double RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB)); //RP es un solo numero

    time = dwalltime() - tick;
    //-----------------------TERMINA CONTAR TIEMPO--------------------------
    printf("El tiempo total de la ecuacion con N:%i y %i threads es: %f ",N,numThreads,time);
    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    free(CD);
    free(AB);
    free(D2);
}

double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}