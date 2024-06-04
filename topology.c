#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>

#define N 4
#define M 4
int main(int argc, char **argv) {

	int pid,nproc;
	int *matrix = NULL,*vector = NULL,*result = NULL;
	int *matrix_local = NULL;
	int *vector_local = NULL;
	int *result_local = NULL;
	
	int *res = NULL;
	int *res_local = NULL;
	//int *vec_local = NULL;
	//int *mat_local = NULL;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
	if(nproc != 4)
        {
        	printf("This application is meant to be run with 4 processes.\n");
        	MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    	}
		
	const int ndims = 2;
	double t = 0;
	int dims[2] = {0,0};
	const int period[2] = {1,0};
	int remains[2]; 
	int reorder = 0;
	int coord[2], id;
	MPI_Comm comm_cart;

	MPI_Dims_create(nproc,ndims,dims);
	//создаем декартову топологию
	MPI_Cart_create(MPI_COMM_WORLD,ndims,dims,period,reorder,&comm_cart);
	MPI_Comm_rank(comm_cart,&id);
	MPI_Cart_coords(comm_cart, id, 2, coord);
    	//printf("Rank %d coordinates are (%d, %d)\n", pid, coord[0], coord[1]);
    	
    	int size = (int)(N*M)/nproc;
    	matrix_local = calloc(N*M/4,sizeof(int));
    	vector_local = calloc(M/2,sizeof(int));
    	result_local = calloc(N/2,sizeof(int));
    	
    	for(int i = 0; i < M; ++i){
    		 result_local[i] = 0;
    	}
    	
    	if(id == 0){
    		matrix = calloc(N*M,sizeof(int));
    		vector = calloc(M,sizeof(int));
    		result = calloc(N,sizeof(int));
    		srand ( time(NULL) ); 
    		for(int i = 0; i < N; ++i){
    			for(int j = 0; j < M; ++j){
    				matrix[i*N + j] = rand()%100;
    				printf("%d ", matrix[i*N + j]);
    			}
    			vector[i] = rand()%100;
    			result[i] = 0;
    			printf(" \n");
    		}
    		res = calloc(N/2,sizeof(int));
    		res_local = calloc(N,sizeof(int));
    	}
    	
    	MPI_Datatype blocktype;
    	MPI_Type_vector(2,M/2,M,MPI_INT,&blocktype);
    	MPI_Type_commit(&blocktype);
    	MPI_Status stat;
        MPI_Request req;
        MPI_Irecv(matrix_local,M*N/4,MPI_INT,0,1,MPI_COMM_WORLD,&req);
        if(pid == 0){
        	printf(" vector:%d %d %d %d\n",vector[0],vector[1],vector[2],vector[3]);
        	for(int i  = 0 ; i < nproc; ++i){
        		MPI_Cart_coords(comm_cart, i, 2, coord);
        		MPI_Send(&(matrix[coord[1]*N/2+N*M/2*coord[0]]),1,blocktype,i,1,MPI_COMM_WORLD);
        		MPI_Send(&vector[(coord[1]*M/2)],M/2,MPI_INT,i,2,MPI_COMM_WORLD);
        	}
        	t = MPI_Wtime();
        }      
    	MPI_Wait(&req,MPI_STATUS_IGNORE);
    	MPI_Recv(&vector_local[0],M/2,MPI_INT,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    	
    	for(int i = 0; i < N/2; i++){
    		result_local[i] = 0;
    		for(int j = 0; j < M/2; ++j){
    			result_local[i] += matrix_local[i*N/2 + j]*vector_local[j];
    			
    		}
    	}
    	MPI_Send(result_local,N/2,MPI_INT,0,3,MPI_COMM_WORLD);
    	
    	if(pid == 0){
    		for(int i = 0; i < nproc ;++i){
    			MPI_Cart_coords(comm_cart, i, 2, coord);
    			MPI_Recv(res_local,N/2,MPI_INT,i,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    			for(int i = 0; i < N/2; ++i){
    				result[i+coord[0]*N/2] += res_local[i];
    			}
    		}
    		t = MPI_Wtime();
    	}
    	
    	if(pid == 0){
    		printf("result: ");
    		for(int i = 0; i < M; ++i){
    			printf("%d ",result[i]);
    		}
    		printf("\ntime result: %.10f\n",MPI_Wtime() - t);
    	}    
    	if(pid == 0){
    		printf("check: ");
    		for(int i = 0; i < N; ++i){
    			res[i] = 0;
    			for(int j = 0; j < N; ++j){
    				res[i] +=matrix[i*N + j]*vector[j];
    			}
    			printf("%d  ",res[i]);
    		}
    		printf("\n time checl: %.10f\n",MPI_Wtime() - t);
    	}
    	/*
    	MPI_Datatype string_type;
    	MPI_Type_contiguous(size,MPI_FLOAT,&string_type);
    	MPI_Type_commit(&string_type);*/
    	
    	MPI_Type_free(&blocktype);
	MPI_Finalize();
	return 0;
}
