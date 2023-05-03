#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

//Declaracion funciones
double dwalltime();
void * calcular_ecuacion(void* ptr);
void encontrar_valoresA(int id);
void encontrar_valoresB(int id);
void mult_matricesAxB(int id);
void potencia_D(int id);
void mult_matricesCxD2(int id);
void sumar_AB_CD(int id);
void multiplicacion_ABxRP(int id);


//variables compartidas
int N,cant_threads,cantElementos;
double *A,*B,*C,*D2,*CD,*AB,*R;
int *D;
int resultados[41];
double maxA,minA,maxB,minB,sumaA,sumaB,promedioA,promedioB;
double RP;

//mutexs
pthread_mutex_t acceder_varA;
pthread_mutex_t acceder_varB;
pthread_barrier_t barrera1;
pthread_barrier_t barrera2;
pthread_barrier_t barrera3;

int main(int argc, char *argv[]){
    double time,tick;
    int i,j;

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

    pthread_attr_init(&attr); //inicializar atributo
    pthread_barrier_init(&barrera1,NULL,cant_threads); //inicializar barreras
    pthread_barrier_init(&barrera2,NULL,cant_threads); 
    pthread_barrier_init(&barrera3,NULL,cant_threads);

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
    
    for(i=1;i<=40;i++){ // se hace secuencial
        resultados[i]= i*i;
    }

    for(i=0;i<cant_threads;i++){
        ids[i]=i;
        pthread_create(&threads[i],&attr,calcular_ecuacion,&ids[i]);
    }

    for(i=0;i<cant_threads;i++){
        pthread_join(threads[i],NULL);
    }

    time = dwalltime() - tick;
    printf("El tiempo total de la ecuacion con N:%i y %i threads es: %f \n",N,cant_threads,time);
    printf("primera pos: %.0f y  ultima: %.0f ",R[0],R[N*N-1]);
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

void * calcular_ecuacion(void * ptr){
    int *p,id,i,j;
    p=(int*) ptr;
    id=*p;
    encontrar_valoresA(id);
    encontrar_valoresB(id);
    mult_matricesAxB(id);
    potencia_D(id);
    pthread_barrier_wait(&barrera1);
    if(id==0){
        promedioA=sumaA/cantElementos;
        promedioB=sumaB/cantElementos;
        RP = ((maxA * maxB - minA * minB) / (promedioA * promedioB));
    }
    mult_matricesCxD2(id);
    pthread_barrier_wait(&barrera2);
    multiplicacion_ABxRP(id);
    pthread_barrier_wait(&barrera3);
    sumar_AB_CD(id);
    pthread_exit(0);

}

//PASO 1
void encontrar_valoresA(int id){
    int i,j;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    double max=A[primera];
    double min=A[primera];
    double suma=0;
    for(i=primera; i<=ultima; i++){
        for(j=0;j<N;j++){
            int pos=i*N+j;
            if(A[pos] > max) 
                max=A[pos];
            else if(A[pos] < min) 
                min=A[pos];
            suma+=A[pos];
        }
    }
    pthread_mutex_lock(&acceder_varA);
    if(max>maxA) 
        maxA=max;
    if(min<minA) 
        minA=min;
    sumaA+=suma;
    pthread_mutex_unlock(&acceder_varA);
}

//PASO 1
void encontrar_valoresB(int id){
    int i,j;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    double max=B[primera];
    double min=B[primera];
    double suma=0;
    for(i=primera; i<=ultima; i++){
        for(j=0;j<N;j++){
            int pos=j*N+i;
            if(B[pos] > max) 
                max=B[pos];
            else if(B[pos] < min) 
                min=B[pos];
            suma+=B[pos];
        }
    }
    pthread_mutex_lock(&acceder_varB);
    if(max>maxB) 
        maxB=max;
    if(min<minB) 
        minB=min;
    sumaB+=suma;
    pthread_mutex_unlock(&acceder_varB);
}

//PASO 2
void mult_matricesAxB(int id){
    int i,j,k;
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
}

//PASO 3
void potencia_D(int id){
    int i,j;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    for(i=primera;i<=ultima;i++){
        for(j=0;j<N;j++){
            int valor = D[j*N+i];
            double v = resultados[valor];
            D2[j*N+i] = v; //ordenado por columna
        }
    }
}

//PASO 4
void mult_matricesCxD2(int id){
    int i,j,k;
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
}

//PASO 6
void multiplicacion_ABxRP(int id){
    int i, j;
    int primera = id * (N / cant_threads);
    int ultima = primera + (N / cant_threads) - 1;
    int inicio = primera * N;
    int fin = (ultima + 1) * N;
    for (i = inicio; i < fin; i++) {
        AB[i] = AB[i] * RP;
    }
}

//PASO 7
void sumar_AB_CD(int id){
    int i;
    int primera = id * (N / cant_threads);
    int ultima = primera + (N / cant_threads) - 1;
    int inicio = primera * N;
    int fin = (ultima + 1) * N;
    for (i = inicio; i < fin; i++) {
        R[i] = AB[i] + CD[i];
    }
}

double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}