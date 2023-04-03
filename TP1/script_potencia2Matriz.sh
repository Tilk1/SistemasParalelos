#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o ./outputs/outputPotencia2Matriz.txt
#SBATCH -e ./outputs/erroresPotencia2Matriz.txt
./mayorMatriz 4096 #es codigo secuencial