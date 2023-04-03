#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o ./outputs/outputMayorMatriz.txt
#SBATCH -e ./outputs/erroresMayorMatriz.txt
./mayorMatriz 4096 #es codigo secuencial