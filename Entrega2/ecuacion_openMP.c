#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h>

double dwalltime();
static inline void mult_bloques(double *ablk, double *bblk, double *cblk);

//variables compartidas
double maxA,minA,maxB,minB,sumaA,sumaB,promedioA,promedioB,RP;
int tam_bloque,N,resultados[41];


int main(int argc, char *argv[]){
    double time,tick;
    int i,j,k,cantElementos,check=1;
    double *A,*B,*C,*D2,*CD,*AB,*R;
    int *D;

    //Verificar parametro 
    if ((argc != 4)){
        printf("Param1: N - Param2: tamanio bloque - Param3: cantidad threads \n");
        exit(1);
    }
    N=atoi(argv[1]);
    tam_bloque = atoi(argv[2]);
    int numThreads = atoi(argv[3]);
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
            A[i*N+j]=1; //Ordenada por Filas
            B[j*N+i]=1; //Ordenada por Columnas
            C[i*N+j]=1; //Ordenada Por Filas
            D[j*N+i]=1; //Ordenada por Columnas 1..40
        }
    }

    // para los promedios
    sumaA = 0; 
    sumaB = 0;
    maxA=A[0];
    minA=A[0];
    maxB=B[0];
    minB=B[0];


    /*  PASOS

    1)  MinA = mínimo de A
        MaxA = máximo de A
        PromA = promedio de A
        MinB = mínimo de B
        MaxB = máximo de B
        PromB = promedio de B 
    2)  RP = (MaxA x MaxB - MinA x MinB) / (PromA x PromB)
    3)  AB = A x B
    4)  D2 = D^2
    5)  CD = C x D2  
    6)  AB = RP x AB 
    7)  R = AB + CD2

    Orden matrices:
    -A: por filas
    -B: por columnas
    -C: por filas
    -D: por columnas
    -D2: por columas
    -AB: por filas
    -CD: por filas
    -R: por filas */

    //-------------------EMPIEZA A CONTAR EL TIEMPO----------------------------
    tick = dwalltime();

    for(i=1;i<=40;i++){ // se hace secuencial
        resultados[i]= i*i;
    }

    #pragma omp parallel shared(A,B,C,D,D2,AB,CD,R) 
    {
        //1) Buscar max,min y suma de A y B
        #pragma omp for nowait firstprivate(cantElementos) private(i) reduction(+:sumaA) reduction(min:minA) reduction(max:maxA) schedule(static)
        for(i=0;i<cantElementos;i++){
            sumaA+=A[i];
            if(A[i]>maxA) maxA=A[i]; 
            else if(A[i]<minA) minA=A[i];
        }

        #pragma omp for firstprivate(cantElementos) private(i) reduction(+:sumaB) reduction(min:minB) reduction(max:maxB) schedule(static)
        for(i=0;i<cantElementos;i++){
            sumaB+=A[i];
            if(B[i]>maxB) maxB=B[i]; 
            else if(B[i]<minB) minB=B[i];
        }

        //2) calcular RP
        #pragma omp single
        {
            printf("maxA: %f, minA: %f, sumaA: %f \n",maxA,minA,sumaA);
            printf("maxB: %f, minB: %f, sumaB: %f \n",maxB,minB,sumaB);
            promedioA=sumaA/cantElementos;
            promedioB=sumaB/cantElementos;
            RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB)); //RP es un solo numero
            printf("RP:%f\n",RP);
        }

        //3) AB = A x B
        #pragma omp for nowait schedule(static)
        for(i=0;i<N;i+=tam_bloque){
            int valori=i*N;
            for(j=0;j<N;j+=tam_bloque){
                int valorj=j*N;
                for(k=0;k<N;k+=tam_bloque){
                    mult_bloques(&A[valori+k], &B[valorj+k], &AB[valori+j]);
                }
            }  
        }

        //4) D2 = D^2
        #pragma omp for schedule(static)
        for(i=0;i<N;i++){
            for(j=0;j<N;j++){
                int valor = D[j*N+i];
                double v = resultados[valor];
                D2[j*N+i] = v; //ordenado por columna
            }
        }

        //5) CD = C x D2
        #pragma omp for nowait schedule(static)
        for(i=0;i<N;i+=tam_bloque){
            int valori=i*N;
            for(j=0;j<N;j+=tam_bloque){
                int valorj=j*N;
                for(k=0;k<N;k+=tam_bloque){
                    mult_bloques(&C[valori+k], &D2[valorj+k], &CD[valori+j]);
                }
            }  
        }


        //6) AB = AB * RP
        #pragma omp for nowait schedule(static)
        for (i=0;i<N*N;i++) {
            AB[i] = AB[i]*RP;
        }

        //7) R = AB + CD
        #pragma omp for nowait schedule(static)
        for (i=0;i<N*N;i++) {
            R[i] = AB[i] + CD[i];
        }
    
    }
    

    time = dwalltime() - tick;
    //-----------------------TERMINA CONTAR TIEMPO--------------------------
    printf("El tiempo total de la ecuacion con N:%i y %i threads es: %f \n",N,numThreads,time);
    //Corrobar resultado
    for(i=0;i<N*N;i++){
	    check=check&&(R[i]==N);
    }   

    if(check){
        printf("Calculo de la ecuacion correcto :) \n");
    }else{
        printf("Calculo de la ecuacion erroneo \n");
    }

/*      printf("imprimo R\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("[%i][%i]= %0.0f\n",i,j,R[i*N+j]);
        }
    } */
    
    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    free(CD);
    free(AB);
    free(D2);
}

//Multiplicacion de cada bloque
static inline void mult_bloques(double *ablk, double *bblk, double *cblk){
    int i,j,k; 
    for(i=0;i<tam_bloque;i++){
        int valori=i*N;
        for(j=0;j<tam_bloque;j++){
            double temp=0;
            int valorj=j*N;
            for(k=0;k<tam_bloque;k++){
                temp+=ablk[valori+k]*bblk[valorj+k];
            }
            cblk[valori+j]+=temp;
        }
    }
}

double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}