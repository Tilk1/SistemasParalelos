#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define COORDINATOR 0

static inline void mult_matrices(double *A, double *B, double *C, int stripSize, int tam_bloque, int N);
static inline void mult_bloques(double *ablk, double *bblk, double *cblk, int tam_bloque, int N);
static inline void potencia_D(int *D,double *D2,int stripSize, double *resultados);
static inline void sumar_matrices(double *AB, double *CD, double *R, int stripSize, int N);


int main(int argc, char* argv[]){
	int i,j,k,cantProcesos,rank,N,stripSize,tam_bloque,check=1;
	double *A,*B,*C,*D2,*CD,*AB,*R;
    int *D;
    int *resultados[41];
	MPI_Status status;
	double commTimes[4], maxCommTimes[4], minCommTimes[4], commTime, totalTime;

    if(argc != 3){
	    printf("Param1: tamanio matriz - Param2: tamanio bloque: \n");
		exit(1);
	}
	tam_bloque=atoi(argv[2]);
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&cantProcesos);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if (N % cantProcesos != 0) {
		printf("El tamanio de la matriz debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

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


    //Calcular porcion de cada worker
	stripSize = N / cantProcesos;

    // Reservar memoria
	if (rank == COORDINATOR) {
		A = (double*) malloc(sizeof(double)*N*N);
		C = (double*) malloc(sizeof(double)*N*N);
        R = (double*) malloc(sizeof(double)*N*N);
 
	}
	else  {
		A = (double*) malloc(sizeof(double)*N*stripSize);
		C = (double*) malloc(sizeof(double)*N*stripSize);
        R = (double*) malloc(sizeof(double)*N*stripSize);
	}

    B = (double*) malloc(sizeof(double)*N*N);
    D = (int*) malloc(sizeof(int)*N*N);
    D2 = (double*) malloc(sizeof(double)*N*N);
    AB = (double*) malloc(sizeof(double)*N*N);
    CD = (double*) malloc(sizeof(double)*N*N);
    resultados = (double*) malloc(sizeof(double)*41);

    //Inicializar datos
	if(rank == COORDINATOR){
	    for(i=0;i<N;i++){
            for (j=0;j<N;j++){
				A[i*N+j]=1;
                B[j*N+i]=1;
                C[i*N+j]=1;
                D[j*N+i]=1;

            }
        }
	}
    
    //Espero a que el coordinador inicialice
    MPI_Barrier(MPI_COMM_WORLD);

    //DONDE INICIALIZAR VECTOR DE 40?
    for(i=1;i<=40;i++){ // se hace secuencial
        resultados[i]= i*i;
    }

    commTimes[0] = MPI_Wtime();

    //Distribuyo los datos
	MPI_Scatter(A,stripSize*N,MPI_DOUBLE,A,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Scatter(C,stripSize*N,MPI_DOUBLE,C,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);

	MPI_Bcast(B,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Bcast(D,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);

    commTimes[1] = MPI_Wtime();

    mult_matrices(A,B,AB,stripSize,tam_bloque,N);
    potencia_D(D,D2,stripSize);
	mult_matrices(C,D2,CD,stripSize,tam_bloque,N);
    sumar_matrices(AB,CD,R,stripSize,N);

    
    MPI_Reduce(commTimes, minCommTimes, 4, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 4, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
	MPI_Finalize();
	if (rank==COORDINATOR) {
		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				check=check&&(R[i*N+j]==3*N);
			}
		}
    }
	if(check){
		printf("Multiplicacion de matrices resultado correcto\n");
	}else{
		printf("Multiplicacion de matrices resultado erroneo\n");
	}

    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    free(CD);
    free(AB);
    free(D2);
    return 0;

}

static inline void mult_matrices(double *A, double *B, double *C, int stripSize,int tam_bloque, int N){
    int i,j,k;
	for (i=0;i<stripSize;i+=tam_bloque){
		int valori=i*N;
		for (j=0;j<N;j+=tam_bloque){
		    int valorj=j*N;
			for (k=0;k<N;k+=tam_bloque) { 
                mult_bloques(&A[valori+k], &B[valorj+k], &C[valori+j],tam_bloque,N);
			}
		}
	}
}

static inline void mult_bloques(double *ablk, double *bblk, double *cblk, int tam_bloque, int N){
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

static inline void potencia_D(int *D,double *D2,int stripSize,double *resultados){
    int i,j;
    for(i=0;i<stripSize;i++){
        for(j=0;j<N;j++){
            int valor = D[j*N+i];
            double v = resultados[valor];
            D2[j*N+i] = v; //ordenado por columna
        }
    }
}

static inline void sumar_matrices(double *AB, double *CD, double *R, int stripSize, int N){
	int i;
	for(i=0;i<stripSize*N;i++){
		R[i]=AB[i]+CD[i];
	}
}