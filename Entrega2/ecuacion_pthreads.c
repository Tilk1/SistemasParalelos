#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

//Declaracion funciones
double dwalltime();
void * mult_matricesAxB(void* ptr);
void * encontrar_valoresA(void* ptr);
void * encontrar_valoresB(void* ptr);


//variables compartidas
int N,cant_threads;
double *A,*B,*C,*D2,*CD,*AB,*R;
int *D;
double maxA,minA,maxB,minB,sumaA,sumaB;

//mutexs
pthread_mutex_t acceder_var;


int main(int argc, char *argv[]){
    double time,tick,promedioA,promedioB;
    int i,j,k,cantElementos,resultados[40];

    //Verificar parametro 
    if ((argc != 3)){
        printf("Param1: N - Param2: cantidad threads \n");
        exit(1);
    }
    N = atoi(argv[1]);
    cant_threads = atoi(argv[2]);
    cantElementos = N*N;
    pthread_attr_t attr;
    int ids[cant_threads]; //para guardar el id de cada thread 
    pthread_t threads[cant_threads]; //declaracion de los threads

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
            B[j*N+i]=rand()%100+1; //Ordenada por Columnas
            C[i*N+j]=1; //Ordenada Por Filas
            D[j*N+i]=rand()%40+1; //Ordenada por Columnas 1..40
        }
    }

    //para los promedios
    sumaA = 0; 
    sumaB = 0;
    maxA=A[0];
    minA=A[0];
    maxB=B[0];
    minB=B[0];

    pthread_attr_init(&attr); //inicializacion del atributo

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

    printf("imprimo A\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("[%i][%i]= %0.0f\n",i,j,A[i*N+j]);
        }
    }
    printf("imprimo B\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("[%i][%i]= %0.0f\n",i,j,B[j*N+i]);
        }
    }

    //EMPIEZA A CONTAR EL TIEMPO --------------------------------------
    tick = dwalltime();
    
    //1) Calcula max,min y promedio de A y B -> paralelo
    for(i=0;i<cant_threads;i++){
        ids[i]=i;
        pthread_create(&threads[i],&attr,encontrar_valoresA,&ids[i]); //1)
        pthread_create(&threads[i],&attr,encontrar_valoresB,&ids[i]); //1) ??
        pthread_create(&threads[i],&attr,mult_matricesAxB,&ids[i]);  //2) ??
    }

    for(i=0;i<cant_threads;i++){
        pthread_join(threads[i],NULL);
    }

    promedioA=sumaA/cantElementos;
    promedioB=sumaB/cantElementos;

    printf("imprimo AB\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("[%i][%i]= %0.0f\n",i,j,AB[i*N+j]);
        }
    } 

    /* printf("maxA: %f, minA: %f:, sumaA: %f \n",maxA,minA,sumaA);
    printf("maxB: %f, minB: %f:, sumaB: %f \n",maxB,minB,sumaB); */

    //2) AB = A X B -> paralelo. Ta el create arriba


    //3) D2= D^2 -> paralelo??
    for(i=1;i<=40;i++){
        resultados[i]= i*i;
    }

    //5) Calculo de RP -> secuencial
    double RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB)); //RP es un solo numero


    time = dwalltime() - tick;
    printf("El tiempo total de la ecuacion con N:%i y %i threads es: %f ",N,cant_threads,time);
    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    free(CD);
    free(AB);
    free(D2);
}

// Codigo funciones ----------------------------------------------


void * encontrar_valoresA(void * ptr){
    int *p,id,i,j;
    p=(int*) ptr;
    id=*p;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    double max=A[primera];
    double min=A[primera];
    double suma=0;
    for(i=primera; i<=ultima; i++){
        for(j=0;j<N;j++){
            int pos=i*N+j;
            if(A[pos] > max) max=A[pos];
            else if(A[pos] < min) min=A[pos];
            suma+=A[pos];
        }
    }
    pthread_mutex_lock(&acceder_var);
    if(max>maxA) maxA=max;
    if(min<minA) minA=min;
    sumaA+=suma;
    pthread_mutex_unlock(&acceder_var);
    pthread_exit(0);
}

void * encontrar_valoresB(void * ptr){
    int *p,id,i,j;
    p=(int*) ptr;
    id=*p;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    double max=B[primera];
    double min=B[primera];
    double suma=0;
    for(i=primera; i<=ultima; i++){
        for(j=0;j<N;j++){
            int pos=i*N+j;
            if(B[pos] > max) max=B[pos];
            else if(B[pos] < min) min=B[pos];
            suma+=B[pos];
        }
    }
    pthread_mutex_lock(&acceder_var);
    if(max>maxB) maxB=max;
    if(min<minB) minB=min;
    sumaB+=suma;
    pthread_mutex_unlock(&acceder_var);
    pthread_exit(0);
}

void * mult_matricesAxB(void * ptr){
    int *p,id,i,j,k;
    p=(int*) ptr;
    id=*p;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    for(i=primera; i<=ultima; i++){
        for(j=0;j<N;j++){
            double temp = 0; 
            for(k=0;k<N;k++){
                temp += A[i*N+k]*B[j*N+k]; 
            }
            AB[i*N+j] += temp;
        }
    }
    pthread_exit(0);
}


double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}