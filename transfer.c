#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>

#define N 4
#define M 4
int main(int argc, char **argv) {

	
    	int pid,nproc;
	int *matrix = NULL;
	int *matrix_local = NULL,*matrix_pack = NULL;
	int position = 0;
	char *buff = NULL; 
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
    	
    	int size = (int)(N*M)/nproc;
    	matrix_local = calloc(N*M/4,sizeof(int));
    	    	
//////////////////заполняем матрицу и вектор рандомными числами///////////////////////////////
    	if(id == 0){
    		matrix = calloc(N*M,sizeof(int));
    		srand ( time(NULL) ); 
    		for(int i = 0; i < N; ++i){
    			for(int j = 0; j < M; ++j){
    				matrix[i*N + j] = rand()%100;
    				printf("%d ", matrix[i*N + j]);
    			}
    			printf(" \n");
    		}
    	}
    	
//////////////////////////////Отправленяем данные блоками//////////////////////////////////////
    	MPI_Datatype blocktype;
    	MPI_Type_vector(N/2,M/2,M,MPI_INT,&blocktype);
    	MPI_Type_commit(&blocktype);
    	MPI_Status stat1;
    	MPI_Status stat2;    	
        MPI_Request req;
        if(pid == 0){
        	for(int i  = 0 ; i < nproc; ++i){
        		MPI_Cart_coords(comm_cart, i, 2, coord);
        		MPI_Send(&(matrix[coord[1]*N/2+N*M/2*coord[0]]),1,blocktype,i,1,MPI_COMM_WORLD);
        	     	}
        }      
    	MPI_Recv(matrix_local,M*N/4,MPI_INT,0,1,MPI_COMM_WORLD,&stat1);
    	MPI_Barrier(comm_cart);
    	printf("rank: %d -> %d %d %d %d \n",pid,matrix_local[0],matrix_local[1],matrix_local[2],matrix_local[3]);
    	
///////////////////////////////////Отправляем с помощью пак///////////////////////////////////////
	if(pid == 0){	
		buff = calloc(1000,sizeof(char));
		position = 0;
		MPI_Pack(matrix,N*M,MPI_INT,buff,1000,&position,MPI_COMM_WORLD);
		MPI_Send(buff,position,MPI_PACKED,1,123,MPI_COMM_WORLD);
	}
	MPI_Barrier(comm_cart);
	if(pid == 1){
		matrix_pack = calloc(N*M,sizeof(int));
		MPI_Recv(matrix_pack,M*N,MPI_INT,0,123,MPI_COMM_WORLD,&stat2);
		printf("Transfer matrix by Pack\n");
		for(int i = 0; i < N; ++i){
			for(int j = 0; j < M; ++j){
				printf("%d ",matrix_pack[N*i+j]);
			}
			printf("\n");
		}
	}
    	MPI_Type_free(&blocktype);
	MPI_Finalize();
	return 0;
}
