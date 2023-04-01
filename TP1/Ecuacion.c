#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;
        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

int main(int argc, char *argv[]){
    double *A,*B,*C,*R;
    int *D;
    double time,tick;
    int i;

    tick = dwalltime();



    time = dwalltime() - tick;
    printf("Tiempo requerido solucion: %f\n",time);

}