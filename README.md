# Matrix-Multiplication-MPI
C implementation of matrix multiplication program with message passing (MPI)

## Install Instrunctions
First install MPI, I used [MPICH](https://www.mpich.org/).

## How to run

```bash
$ mpicc mpi-mult.c -o mpimult
$ mpirun -np 4 --hostfile <hosts-file> ./mpimult

```

## Author
Maria Oikonomidou 
