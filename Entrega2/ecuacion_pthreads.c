#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

//Declaracion funciones
double dwalltime();
void * encontrar_valoresA(void* ptr);
void * encontrar_valoresB(void* ptr);
void * mult_matricesAxB(void* ptr);
void * potencia_D(void* ptr);
void * mult_matricesCxD2(void* ptr);
void * sumar_AB_CD2(void* ptr);


//variables compartidas
int N,cant_threads;
double *A,*B,*C,*D2,*CD,*AB,*R;
int *D;
int resultados[40];
double maxA,minA,maxB,minB,sumaA,sumaB;

//mutexs
pthread_mutex_t acceder_var;


int main(int argc, char *argv[]){
    double time,tick,promedioA,promedioB;
    int i,j,k,cantElementos;

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
            C[i*N+j]=rand()%40+1; //Ordenada Por Filas
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
    7)  R = AB + CD
    
    Orden matrices:
    -A: por filas
    -B: por columnas
    -C: por filas
    -D: por columnas
    -D2: por columas
    -AB: por filas
    -CD: por filas
    -R: por filas */

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

    /* printf("maxA: %f, minA: %f:, sumaA: %f \n",maxA,minA,sumaA);
    printf("maxB: %f, minB: %f:, sumaB: %f \n",maxB,minB,sumaB); */

    //2) AB = A X B -> paralelo. Ta el create arriba

    //3) D2= D^2 -> paralelo??
    for(i=1;i<=40;i++){
        resultados[i]= i*i;
    }

    for(i=0;i<cant_threads;i++){
        ids[i]=i;
        pthread_create(&threads[i],&attr,potencia_D,&ids[i]);
    }

    for(i=0;i<cant_threads;i++){
        pthread_join(threads[i],NULL);
    }

    printf("imprimo C\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("[%i][%i]= %0.0f\n",i,j,C[i*N+j]);
        }
    }

    printf("imprimo D2\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("[%i][%i]= %0.0f\n",i,j,D2[j*N+i]);
        }
    }

    //4) CD = C X D2
    for(i=0;i<cant_threads;i++){
        ids[i]=i;
        pthread_create(&threads[i],&attr,mult_matricesCxD2,&ids[i]);
    }

    for(i=0;i<cant_threads;i++){
        pthread_join(threads[i],NULL);
    }

    printf("imprimo CD\n");
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            printf("[%i][%i]= %0.0f\n",i,j,CD[i*N+j]);
        }
    }

    //5) Calculo de RP -> secuencial
    double RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB)); //RP es un solo numero

    //6) AB = AB X RP(double)


    //7) R = AB + CD
    for(i=0;i<cant_threads;i++){
        ids[i]=i;
        pthread_create(&threads[i],&attr,sumar_AB_CD2,&ids[i]);
    }

    for(i=0;i<cant_threads;i++){
        pthread_join(threads[i],NULL);
    }

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

//PASO 1
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

//PASO 1
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

//PASO 2
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

//PASO 3
void * potencia_D(void * ptr){
    int *p,i,j;
    p=(int*) ptr;
    int id=*p;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    for(i=primera;i<=ultima;i++){
        for(j=0;j<N;j++){
            int valor = D[j*N+i];
            double v = resultados[valor];
            D2[j*N+i] = v; //ordenado por columna
        }
    }
    pthread_exit(0);
}

//PASO 4
void * mult_matricesCxD2(void* ptr){
    int *p,id,i,j,k;
    p=(int*) ptr;
    id=*p;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    for(i=primera; i<=ultima; i++){
        for(j=0;j<N;j++){
            double temp = 0; 
            for(k=0;k<N;k++){
                temp += C[i*N+k]*D2[j*N+k]; 
            }
            CD[i*N+j] += temp;
        }
    }
    pthread_exit(0);
}

//Paso 7
void * sumar_AB_CD2(void * ptr){
    int *p,id,i,j;
    p=(int*) ptr;
    id=*p;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    for(i=primera; i<=ultima; i++){
        int valori = i*N;
        for(j=0;j<N;j++){
            R[valori+j] = AB[valori+j] + CD[valori+j];
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