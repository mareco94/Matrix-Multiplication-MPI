# Matrix-Multiplication-MPI
C implementation of matrix multiplication program with message passing interface (MPI)

## Install Instrunctions
First install MPI, I used [MPICH](https://www.mpich.org/).

## How to run

### Compile
```bash
$ mpicc mpi-mult.c -o mpimult
```
### Run
```bash
$ mpirun -np <host-number> --hostfile <hosts-file> ./mpimult
```


## Author
Maria Oikonomidou 
