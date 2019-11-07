#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define N 1000 //matrix size

void print_array(int **array){
	int i = 0;
	int j = 0;
	for (i=0; i<N; i++){
		for(j=0; j<N;j++){
			printf("%d ",array[i][j]);
		}
		printf("\n");
	}
	return;
}

int main (int argc, char *argv[]){

	int rank, size, n;
	int to,rows, workload, rleft, offset;
	int i, j, k;
	struct timeval start, end;
	long double exec_time;

	int **arrayA = malloc(sizeof(int*)*N);
	for(i=0;i<N;i++){
		arrayA[i] = malloc(N*sizeof(int));
	}

	int **arrayB = malloc(sizeof(int*)*N);
	for(i=0;i<N;i++){
		arrayB[i] = malloc(N*sizeof(int));
	}
	
	int **resultArray = malloc(sizeof(int*)*N);
	for(i=0;i<N;i++){
		resultArray[i] = malloc(N*sizeof(int));
	}

	/* initialize arrays */
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			arrayA[i][j] = 1;
			arrayB[i][j] = 1;
		}
	}

	
	/* initialize MPI */
	MPI_Status status;
	MPI_Init(NULL,NULL);

	/* get the rank */
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	/* get how many threads there are */
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	/* get how many nodes there are */
	char node[MPI_MAX_PROCESSOR_NAME];
	MPI_Get_processor_name(node, &n);
	
	/* broadcast the second array to all nodes*/	 
	for(i=0;i<N;i++){
		MPI_Bcast(&arrayB[i][0],N,MPI_INT,0,MPI_COMM_WORLD);
	}
	

	/* master */
	if (rank == 0){
		
		/* start timer */
		gettimeofday(&start, NULL);

		workload = N/(size-1);
		rleft = N%(size-1);
		offset = 0;

		/* calculate for remaining rows to add in calculation */
		for (to=1; to < size; to++){
			if (to > rleft){
			rows = workload;
			}
			else{ 
			rows = workload+1;
			}

			/* master sends workload to workers 
			- rows offset and the specific "sub-array" for calculation */
			MPI_Send(&rows, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
			MPI_Send(&offset, 1, MPI_INT, to, 1, MPI_COMM_WORLD);
			for(j=offset;j<offset+rows;j++){
				MPI_Send(&arrayA[j][0], N, MPI_INT, to, 1, MPI_COMM_WORLD);
			}
			offset = offset + rows;
		}

		/* master receives results */
		for (i=1; i<size; i++){
			MPI_Recv(&rows, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			MPI_Recv(&offset, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			for(j=offset;j<offset+rows;j++){
				MPI_Recv(&resultArray[j][0], rows*N, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			}
		}
		/* end timer */
		gettimeofday(&end, NULL);
		exec_time = (((end.tv_sec - start.tv_sec) * 1000.0 )+ ((end.tv_usec - start.tv_usec) / 1000.0));

		printf("** Result Array **\n");
		// print_array(resultArray);
		printf("Execution time = %Lf ms \n",exec_time);

	}

	/* worker */
	else if(rank != 0){

		printf("Receiver node %s with rank:%d\n",node,rank);
		
		/* workers receive workload 
		- rows offset and the specific "sub-array" for calculation */
		MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		for(j=0;j<rows;j++){
			MPI_Recv(&arrayA[j][0],N, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		}

		/* calculations */
		k=0;
		while(k<N){
			for (i=0; i<rows; i++){
				resultArray[i][k] = 0;
				for (j=0; j<N; j++){
					resultArray[i][k] = resultArray[i][k] + arrayA[i][j] * arrayB[j][k];
				}
			}
			k++;
		}

		/* send master the results */
		MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		for(j=0;j<rows;j++){
			MPI_Send(&resultArray[j][0], N, MPI_INT, 0, 2, MPI_COMM_WORLD);
		}
	}

	/* terminating MPI */
	MPI_Finalize();

	return 0;
}

